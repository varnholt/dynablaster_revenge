#include "mpegstream.h"
#include "debug.h"
#include "decode.h"
#include "tools/bitstream.h"
#include <memory.h>
#include <math.h>
#include <string.h>

#ifndef __APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif

static void* aligned_pointer(void *base, unsigned int alignment)
{
   long long baseval = (long long)base;
   long long aoff = baseval % alignment;

   if(aoff) return (char*)base+alignment-aoff;
   else     return base;
}

enum parse_codes
{
    PARSE_MORE = MP3_NEED_MORE
   ,PARSE_ERR  = MP3_ERR
   ,PARSE_END  = 10 /* No more audio data to find. */
   ,PARSE_GOOD = 1 /* Everything's fine. */
   ,PARSE_BAD  = 0 /* Not fine (invalid data). */
   ,PARSE_RESYNC = 2 /* Header not good, go into resync. */
   ,PARSE_AGAIN  = 3 /* Really start over, throw away and read a new header, again. */
};

int MpegStream::initialized = 0;

MpegStream::MpegStream()
{
   wordpointer= 0;
   if (!initialized)
   {
      layer2.init();
      layer3.init();

      prepare_decode_tables();
      initialized = 1;
   }

   lastscale= 0.0;
   decwin= NULL;
   init( NULL );
}

MpegStream::~MpegStream()
{
   close();
   frame_reset();
}

bool MpegStream::open(char *buffer, int size)
{
   close();
   frame_reset();

   rdat.buffer = buffer;
   rdat.filepos = 0;
   rdat.filelen = size;

   int ret;
   int tries= 0;
   do {
      ret = decode_frame( 0 );
      tries++;
   } while (ret != MP3_NEW_FORMAT && tries < 10);

   if (tries>=10)
      return false;

   return true;
}

void MpegStream::set_pointer(long backstep)
{
   wordpointer = bsbuf + ssize - backstep;
   if (backstep)
   {
      memcpy(wordpointer, bsbufold+mOldFrameSize-backstep, backstep);
   }
   mBitIndex = 0;
}

bool MpegStream::frame_output_format()
{
   // is there a change?
   if(mCurSampleRate == frequency() && mCurChannels == mChannels)
   {
      return false; // no change
   }
   else // a new format
   {
      mCurSampleRate = frequency();
      mCurChannels = mChannels;
      return true;
   }
}

int MpegStream::bitrate() const
{
   static const int bitrateTable[2][3][16] =
   {
      { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
        {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
        {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },
      { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
   };

   return bitrateTable[lsf][mLayerId-1][mBitrateIndex];
}

int MpegStream::frequency() const
{
   static const long freqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };
   return freqs[mSampleRate];
}

int MpegStream::read_frame_body(unsigned char *buf, int size)
{
   long l;

   l= fullread(buf, size);
   if (l != size)
   {
      long ll = l;
      if(ll <= 0) ll = 0;
      return MP3_NEED_MORE;
   }
   return l;
}

int MpegStream::fullread(unsigned char *buf, int count)
{
   int avail = (rdat.filelen - rdat.filepos);
   if (avail < count)
      return MP3_ERR;
   memcpy(buf, rdat.buffer + rdat.filepos, count);
   rdat.filepos += count;
   return count;
}

int MpegStream::tell()
{
   return rdat.filepos;
}

void MpegStream::backbits(int nob)
{
   mBitIndex    -= nob;
   wordpointer += (mBitIndex>>3);
   mBitIndex    &= 0x7;
}

int MpegStream::getbitoffset()
{
   return (-mBitIndex)&0x7;
}

unsigned char MpegStream::getbyte()
{
   return (*wordpointer++);
}

unsigned int MpegStream::getbits( int number_of_bits )
{
  unsigned long rval;

  {
    rval = wordpointer[0];
    rval <<= 8;
    rval |= wordpointer[1];
    rval <<= 8;
    rval |= wordpointer[2];

    rval <<= mBitIndex;
    rval &= 0xffffff;

    mBitIndex += number_of_bits;

    rval >>= (24-number_of_bits);

    wordpointer += (mBitIndex>>3);
    mBitIndex &= 7;
  }

  return rval;
}


void MpegStream::skipbits(int nob)
{
   ultmp = wordpointer[0];
   ultmp <<= 8;
   ultmp |= wordpointer[1];
   ultmp <<= 8;
   ultmp |= wordpointer[2];
   ultmp <<= mBitIndex;
   ultmp &= 0xffffff;
   mBitIndex += nob;
   ultmp >>= (24-nob);
   wordpointer += (mBitIndex>>3);
   ultmp= mBitIndex &= 7;
}

unsigned int MpegStream::getbits_fast(int nob)
{
   ultmp = (unsigned char) (wordpointer[0] << mBitIndex);
   ultmp |= ((unsigned long) wordpointer[1]<<mBitIndex)>>8;
   ultmp <<= nob;
   ultmp >>= 8;
   mBitIndex += nob;
   wordpointer += (mBitIndex>>3);
   mBitIndex &= 7;
   return ultmp;
}

unsigned int MpegStream::get1bit()
{
   uctmp = *wordpointer << mBitIndex;
   mBitIndex++;
   wordpointer += (mBitIndex>>3);
   mBitIndex &= 7;
   return uctmp>>7;
}

/* return FALSE on error, TRUE on success, MP3_NEED_MORE on occasion */
bool MpegStream::head_read(unsigned long *newhead)
{
   unsigned char hbuf[4];
   int ret = fullread(hbuf,4);

   if(ret != 4)
      return false;

   *newhead = ((unsigned long) hbuf[3] << 24) |
              ((unsigned long) hbuf[2] << 16) |
              ((unsigned long) hbuf[1] << 8)  |
               (unsigned long) hbuf[0];

   return true;
}

void MpegStream::close()
{
   mCurChannels= 0;
   mCurSampleRate= 0;
}

/* return FALSE on error, TRUE on success, MP3_NEED_MORE on occasion */
bool MpegStream::head_shift(unsigned long *head)
{
   unsigned char hbuf;
   int ret = fullread(&hbuf,1);
   if(ret != 1) return false;

   *head= ((*head)>>8) | ((unsigned long)hbuf<<24);
   return true;
}

/* returns reached position... negative ones are bad... */
int MpegStream::skip_bytes(int len)
{
   if (rdat.filepos + len > rdat.filelen)
      return -1;
   else
      rdat.filepos += len;
   return rdat.filepos;
}

int MpegStream::back_bytes(int len)
{
   if (rdat.filepos - len < 0)
      return -1;
   rdat.filepos -= len;
   return rdat.filepos;
}


/* Not just for feed reader, also for self-feeding buffered reader. */
void MpegStream::forget()
{
}

void MpegStream::init(FrameParameter *mp)
{
   buffer.data = NULL;
   buffer.fill = 0;
   rawbuffs = NULL;
   layerscratch = NULL;
   mId3Tag.reset();

   mCurChannels= 0;
   mCurSampleRate= 0;

   decoder_change = 1;
   err = MP3_OK;
   if(mp == NULL)
      p.default_pars();
   else
      memcpy(&p, mp, sizeof(FrameParameter));

   fixed_reset(); /* Reset only the fixed data, dynamic buffers are not there yet! */
}


int MpegStream::frame_outbuffer()
{
   if(buffer.data == NULL)
   {
      buffer.data = (short*) malloc(20000);
   }
   buffer.fill = 0;
   return MP3_OK;
}

void MpegStream::decode_buffers_reset()
{
   if (rawbuffs)
      memset(rawbuffs, 0, 2*2*0x110*sizeof(float));
}

int MpegStream::frame_buffers()
{
   int buffssize= 2*2*0x110*sizeof(float);
   buffssize += 15; /* For 16-byte alignment (SSE likes that). */

   if(rawbuffs == NULL)
      rawbuffs = (unsigned char*) malloc(buffssize);

   real_buffs[0][0] = (float*)aligned_pointer(rawbuffs,16);
   real_buffs[0][1] = real_buffs[0][0] + 0x110;
   real_buffs[1][0] = real_buffs[0][1] + 0x110;
   real_buffs[1][1] = real_buffs[1][0] + 0x110;

   /* now the different decwins... all of the same size, actually */
   if(decwin == NULL)
      decwin = (float*) malloc( (512+32)*sizeof(float) );

   /* Layer scratch buffers are of compile-time fixed size, so allocate only once. */
   if(layerscratch == NULL)
   {
      size_t scratchsize = 0;
      float *scratcher;
#ifndef NO_LAYER1
      scratchsize += sizeof(float) * 2 * SBLIMIT;
#endif
#ifndef NO_LAYER2
      scratchsize += sizeof(float) * 2 * 4 * SBLIMIT;
#endif
#ifndef NO_LAYER3
      scratchsize += sizeof(float) * 2 * SBLIMIT * SSLIMIT; /* hybrid_in */
      scratchsize += sizeof(float) * 2 * SSLIMIT * SBLIMIT; /* hybrid_out */
#endif

      layerscratch = (float*)malloc(scratchsize+63);
      if(layerscratch == NULL) return -1;

      scratcher = (float*)aligned_pointer(layerscratch,64);

      layer1.fraction = (float(*)[SBLIMIT])scratcher;
      scratcher += 2 * SBLIMIT;

      layer2.fraction = (float(*)[4][SBLIMIT])scratcher;
      scratcher += 2 * 4 * SBLIMIT;

      layer3.hybrid_in = (float(*)[SBLIMIT][SSLIMIT])scratcher;
      scratcher += 2 * SBLIMIT * SSLIMIT;
      layer3.hybrid_out = (float(*)[SSLIMIT][SBLIMIT])scratcher;
      scratcher += 2 * SSLIMIT * SBLIMIT;
   }

   decode_buffers_reset();

   debug1("frame %p buffer done", (void*)fr);
   return 0;
}

int MpegStream::buffers_reset()
{
   buffer.fill = 0; /* hm, reset buffer fill... did we do a flush? */
   bsnum = 0;
   bsbuf = bsspace[1];
   bsbufold = bsbuf;
   bitreservoir = 0;
   decode_buffers_reset();
   memset(bsspace, 0, 2*(MAXFRAMESIZE+512));
   hybrid_blc[0] = hybrid_blc[1] = 0;
   memset(hybrid_block, 0, sizeof(float)*2*2*SBLIMIT*SSLIMIT);
   return 0;
}

int MpegStream::frame_reset()
{
   buffers_reset();
   fixed_reset();

   return 0;
}

void MpegStream::fixed_reset()
{
   to_decode = 0;
   mFrameNum = -1;
   clip = 0;
   mOldHead = 0;
   mFirstHead = 0;
   mFrameSize=0;
   lastscale = -1;
   rva.level[0] = -1;
   rva.level[1] = -1;
   rva.gain[0] = 0;
   rva.gain[1] = 0;
   rva.peak[0] = 0;
   rva.peak[1] = 0;
   mOldFrameSize = 0;
   firstframe = 0;
   ignoreframe = firstframe - p.preframes;
   new_format = 0;
   bo = 1;
   mId3Tag.reset();
   mErrorProtection = 0;
   mFreeFormatFrameSize = -1;
}

void MpegStream::free_buffers()
{
   if(rawbuffs != NULL)
      free(rawbuffs);
   rawbuffs = NULL;

   if (decwin != NULL)
      free(decwin);
   decwin = NULL;
   if (layerscratch != NULL)
      free(layerscratch);
}

void MpegStream::frame_exit()
{
   if(buffer.data != NULL)
   {
      free(buffer.data);
   }
   free_buffers();
   mId3Tag.reset();
}

int MpegStream::frame_outs(int num)
{
   return spf()*num;
}

int MpegStream::frame_expect_outsamples()
{
   int outs = 0;
   outs = spf();
   return outs;
}

void MpegStream::frame_skip()
{
   if(mLayerId == 3)
      set_pointer(512);
}

int MpegStream::get_rva(double *peak, double *gain)
{
   double p0 = -1;
   double g = 0;
   int ret = 0;

   if (p.rva)
   {
      int rt = 0;
      if(p.rva == 2 && rva.level[1] != -1) rt = 1;
      if(rva.level[rt] != -1)
      {
         p0 = rva.peak[rt];
         g = rva.gain[rt];
         ret = 1; /* Success. */
      }
   }
   if(peak != NULL) *peak = p0;
   if(gain != NULL) *gain = g;
   return ret;
}

int MpegStream::spf() const
{
   switch (mLayerId)
   {
   case 1:
      return 384;
      break;
   case 2:
      return 1152;
      break;
   default:
      if (lsf || mMpeg25)
         return 576;
      break;
   };
   return 1152;
}

/* adjust the volume, taking both outscale and rva values into account */
void MpegStream::do_rva()
{
   double peak = 0;
   double gain = 0;
   double newscale;
   double rvafact = 1;

   if(get_rva(&peak, &gain))
   {
      rvafact = pow(10,gain/20);
   }

   newscale = p.outscale*rvafact;

   /* if peak is unknown (== 0) this check won't hurt */
   if((peak*newscale) > 1.0)
   {
      newscale = 1.0/peak;
   }
   /* first rva setting is forced with lastscale < 0 */
   if(newscale != lastscale || decoder_change)
   {
      debug3("changing scale value from %f to %f (peak estimated to %f)", lastscale != -1 ? lastscale : p.outscale, newscale, (double) (newscale*peak));
      lastscale = newscale;
      /* It may be too early, actually. */
      make_decode_tables(this); /* the actual work */
   }
}

int MpegStream::do_layer()
{
   switch (mLayerId)
   {
   case 1:
      layer1.process(this);
      break;
   case 2:
      layer2.process(this);
      break;
   case 3:
      layer3.process(this);
      break;
   }
   return 0;
}

void MpegStream::decode_the_frame()
{
   size_t neededSamples = frame_expect_outsamples();

   clip += do_layer();

   if(buffer.fill < neededSamples)
   {
         memset( buffer.data + buffer.fill, 0, neededSamples - buffer.fill );
         buffer.fill = neededSamples;
   }
}


/*
   Put _one_ decoded frame into the frame structure's buffer, accessible at the location stored in <audio>, with <bytes> bytes available.
   The buffer contents will be lost on next call to mpg123_decode_frame.
   MP3_OK -- successfully decoded the frame, you get your output data
   MPg123_DONE -- This is it. End.
   MP3_ERR -- some error occured...
   MP3_NEW_FORMAT -- new frame was read, it results in changed output format -> will be decoded on next call
   MP3_NEED_MORE  -- that should not happen as this function is intended for in-library stream reader but if you force it...
   MP3_NO_SPACE   -- not enough space in buffer for safe decoding, also should not happen

   num will be updated to the last decoded frame number (may possibly _not_ increase, p.ex. when format changed).
*/
int MpegStream::decode_frame(short **audio)
{
   buffer.fill = 0; /* always start fresh */

   while( true )
   {
      /* decode if possible */
      if(to_decode)
      {
         if(new_format)
         {
            debug("notifiying new format");
            new_format = 0;
            return MP3_NEW_FORMAT;
         }

         decode_the_frame();

         to_decode = 0;
         if(audio != NULL) *audio = buffer.data;

         return buffer.fill;
      }
      else
      {
         int b = get_next_frame();
         if(b < 0) return b;
         debug1("got next frame, %i", to_decode);
      }
   }
}

/* Just tell if the header is some mono. */
static bool header_mono(unsigned long newhead)
{
   return (HDR_CHANNEL_VAL(newhead) == MPG_MD_MONO);
}

/* compiler is smart enought to inline this one or should I really do it as macro...? */
static bool head_check(unsigned long head)
{
   int sync= head & HDR_SYNC;
   int layer= HDR_LAYER_VAL(head);
   int bitrate= HDR_BITRATE_VAL(head);
   int smprate= HDR_SAMPLERATE_VAL(head);

   if (  (sync != HDR_SYNC)
      || !layer        // layer: 01,10,11 is 1,2,3; 00 is reserved
      || (bitrate == 0xf) // 1111 means bad bitrate
      || (smprate == 0x3) // sampling freq: 11 is reserved
   )
   {
      return false;
   }
   /* if no check failed, the header is valid (hopefully)*/
   else
   {
      return true;
   }
}


int MpegStream::guess_freeformat_framesize()
{
   long i;
   unsigned long head;

   if (!head_read(&head))
      return MP3_NEED_MORE;

   /* We are already 4 bytes into it */
   for(i=4;i<MAXFRAMESIZE+4;i++)
   {
      if (!head_shift(&head))
         return MP3_NEED_MORE;

      /* No head_check needed, the mask contains all relevant bits. */
      if((head & HDR_SAMEMASK) == (mOldHead & HDR_SAMEMASK))
      {
         back_bytes(i+1);
         mFrameSize = i-3;
         return PARSE_GOOD; /* Success! */
      }
   }
   back_bytes(i);
   return PARSE_BAD;
}


int MpegStream::decode_header(unsigned long newhead)
{
   int freeformat_count= 0;

   // parse header as described in http://de.wikipedia.org/wiki/MP3#Spezifikation
   BitStream stream(&newhead, 4);
   stream.skipBits(11);

   int id= stream.getBits(2);

   if(id & 0x2)
   {
      lsf = (id & 0x1) ? 0 : 1;
      mMpeg25 = 0;
   }
   else
   {
      lsf = 1;
      mMpeg25 = 1;
   }

   mLayerId          = 4 - stream.getBits(2);
   mErrorProtection  = 1-stream.getBits(1);
   mBitrateIndex     = stream.getBits(4);
   mSampleRate       = stream.getBits(2);

   if(mMpeg25)
      mSampleRate = 6 + mSampleRate;
   else
      mSampleRate = mSampleRate + (lsf*3);

   mPadding          = stream.getBits(1);
   stream.skipBits(1); // Private flag
   mMode             = stream.getBits(2);
   mModeExt          = stream.getBits(2);
   stream.skipBits(1); // Copyright flag
   stream.skipBits(1); // Original flag

   mChannels = (mMode == MPG_MD_MONO) ? 1 : 2;

   /* we can't use tabsel_123 for freeformat, so trying to guess framesize... */
   if(mBitrateIndex==0)
   {
      /* when we first encounter the frame with freeformat, guess framesize */
      if(mFreeFormatFrameSize < 0)
      {
         int ret;
         freeformat_count++;
         if(freeformat_count > 5)
            return PARSE_BAD;

         ret = guess_freeformat_framesize();
         if(ret == PARSE_GOOD)
         {
            mFreeFormatFrameSize = mFrameSize - mPadding;
            fprintf(stderr, "Note: free format frame size %li\n", mFreeFormatFrameSize);
         }
         else
         {
            return ret;
         }
      }
      /* freeformat should be CBR, so the same framesize can be used at the 2nd reading or later */
      else
      {
         mFrameSize = mFreeFormatFrameSize + mPadding;
      }
   }

   switch(mLayerId)
   {
      case 1:
         if(mBitrateIndex)
         {
            mFrameSize  = bitrate() * 12000;
            mFrameSize /= frequency();
            mFrameSize  = ((mFrameSize+mPadding)<<2)-4;
         }
      break;

      case 2:
         if(mBitrateIndex)
         {
            debug2("bitrate index: %i (%i)", mBitrateIndex, bitrate() );
            mFrameSize = (long) bitrate() * 144000;
            mFrameSize /= frequency();
            mFrameSize += mPadding - 4;
         }
      break;

      case 3:
         if(lsf)
         ssize = (mChannels == 1) ? 9 : 17;
         else
         ssize = (mChannels == 1) ? 17 : 32;

         if(mErrorProtection)
         ssize += 2;

         if(mBitrateIndex)
         {
            mFrameSize  = (long) bitrate() * 144000;
            mFrameSize /= frequency()<<(lsf);
            mFrameSize = mFrameSize + mPadding - 4;
         }
      break;

      default:
         return PARSE_BAD;
   }
   if (mFrameSize > MAXFRAMESIZE)
   {
      printf("Frame size too big: %d \n", mFrameSize+4-mPadding);

      return PARSE_BAD;
   }
   return PARSE_GOOD;
}

int MpegStream::handle_id3v2(unsigned long /*newhead*/)
{
   int ret;
   mOldHead = 0; /* Think about that. Used to be present only for skipping of junk, not resync-style wetwork. */
   ret = mId3Tag.parse(this);
   if (ret < 0) return ret;

   return PARSE_AGAIN;
}



/* The newhead is bad, so let's check if it is something special, otherwise just resync. */
int MpegStream::wetwork(unsigned long *newheadp)
{
   int ret = PARSE_ERR;
   unsigned long newhead = *newheadp;
   *newheadp = 0;

   /* Classic ID3 tags. Read, then start parsing again. */
   if((newhead & 0xffffff00) == ('T'<<24)+('A'<<16)+('G'<<8))
   {
      unsigned char dummy[128];
      ret= fullread(dummy, 124);
      if (ret < 0) return ret;

//		rdat.flags |= READER_ID3TAG; /* that marks id3v1 */
      return PARSE_AGAIN;
   }
   /* This is similar to initial junk skipping code... */
   /* Check for id3v2; first three bytes (of 4) are "ID3" */
   if((newhead & (unsigned long) 0xffffff00) == (unsigned long) 0x49443300)
   {
      return handle_id3v2(newhead);
   }

   /* Now we got something bad at hand, try to recover. */

   if((newhead & 0xffffff00) == ('b'<<24)+('m'<<16)+('p'<<8)) fprintf(stderr,"Note: Could be a BMP album art.\n");

   long tries = 0;
   long limit = p.resync_limit;

   /* If a resync is needed the bitreservoir of previous frames is no longer valid */
   bitreservoir = 0;

   do /* ... shift the header with additional single bytes until be found something that could be a header. */
   {
      ++tries;
      if(limit >= 0 && tries >= limit) break;

      if (!head_shift(&newhead))
      {
         *newheadp = newhead;
         fprintf (stderr, "Note: Hit end of (available) data during resync.\n");

         return PARSE_END;
      }
   } while(!head_check(newhead));

   *newheadp = newhead;

   /* Now we either got something that could be a header, or we gave up. */
   if(limit >= 0 && tries >= limit)
   {
      return PARSE_ERR;
   }
   else
   {
      debug1("Found possibly valid header 0x%lx... unsetting firsthead to reinit stream.", newhead);
      mFirstHead = 0;
      return PARSE_RESYNC;
   }
}

int MpegStream::check_lame_tag()
{
   /*
      going to look for Xing or Info at some position after the header
                                         MPEG 1  MPEG 2/2.5 (LSF)
      Stereo, Joint Stereo, Dual Channel  32      17
      Mono                                17       9

      Also, how to avoid false positives? I guess I should interpret more of the header to rule that out(?).
      I hope that ensuring all zeros until tag start is enough.
   */
   int lame_offset = (mChannels == 2) ? (lsf ? 17 : 32 ) : (lsf ? 9 : 17);

   /* Note: CRC or not, that does not matter here. */
   if(mFrameSize >= 120+lame_offset) /* traditional Xing header is 120 bytes */
   {
      int i;
      int lame_type = 0;
      debug("do we have lame tag?");
      /* only search for tag when all zero before it (apart from checksum) */
      for(i=2; i < lame_offset; ++i) if(bsbuf[i] != 0) break;
      if(i == lame_offset)
      {
         debug("possibly...");
         if
         (
                  (bsbuf[lame_offset] == 'I')
            && (bsbuf[lame_offset+1] == 'n')
            && (bsbuf[lame_offset+2] == 'f')
            && (bsbuf[lame_offset+3] == 'o')
         )
         {
            lame_type = 1; /* We still have to see what there is */
         }
         else if
         (
                  (bsbuf[lame_offset] == 'X')
            && (bsbuf[lame_offset+1] == 'i')
            && (bsbuf[lame_offset+2] == 'n')
            && (bsbuf[lame_offset+3] == 'g')
         )
         {
            lame_type = 2;
         }
         if(lame_type)
         {
            unsigned long xing_flags;

            /* we have one of these headers... */
            /* now interpret the Xing part, I have 120 bytes total for sure */
            /* there are 4 bytes for flags, but only the last byte contains known ones */
            lame_offset += 4; /* now first byte after Xing/Name */
            /* 4 bytes dword for flags */
            #define make_long(a, o) ((((unsigned long) a[o]) << 24) | (((unsigned long) a[o+1]) << 16) | (((unsigned long) a[o+2]) << 8) | ((unsigned long) a[o+3]))
            /* 16 bit */
            #define make_short(a,o) ((((unsigned short) a[o]) << 8) | ((unsigned short) a[o+1]))
            xing_flags = make_long(bsbuf, lame_offset);
            lame_offset += 4;
            debug1("Xing: flags 0x%08lx", xing_flags);
            if(xing_flags & 1) /* frames */
            {
               lame_offset += 4;
            }
            if(xing_flags & 0x2) /* bytes */
            {
               /* We assume that this is the _total_ size of the file, including Xing frame ... and ID3 frames...
                  It's not that clearly documented... */
               unsigned long xing_bytes = make_long(bsbuf, lame_offset);

               if(rdat.filelen < 1)
                  rdat.filelen = (int) 0;//xing_bytes; /* One could start caring for overflow here. */
               else
               {
                  if((int) xing_bytes != rdat.filelen)
                  {
                     double diff = 1.0/rdat.filelen * (rdat.filelen - (int)xing_bytes);
                     if(diff < 0.) diff = -diff;
                  }
               }

               lame_offset += 4;
            }
            if(xing_flags & 0x4) /* TOC */
            {
               lame_offset += 100; /* just skip */
            }
            if(xing_flags & 0x8) /* VBR quality */
            {
               lame_offset += 4;
            }
            /* I guess that either 0 or LAME extra data follows */
            if(bsbuf[lame_offset] != 0)
            {
               // unsigned char lame_vbr;
               float replay_gain[2] = {0,0};
               // float peak = 0;
               float gain_offset = 0; /* going to be +6 for old lame that used 83dB */
               char nb[10];
               memcpy(nb, bsbuf+lame_offset, 9);
               nb[9] = 0;
               if(!strncmp("LAME", nb, 4))
               {
                  /* Lame versions before 3.95.1 used 83 dB reference level, later versions 89 dB.
                     We stick with 89 dB as being "normal", adding 6 dB. */
                  unsigned int major, minor;
                  char rest[6];
                  rest[0] = 0;
                  if(sscanf(nb+4, "%u.%u%s", &major, &minor, rest) >= 2)
                  {
                     debug3("LAME: %u/%u/%s", major, minor, rest);
                     /* We cannot detect LAME 3.95 reliably (same version string as 3.95.1), so this is a blind spot.
                        Everything < 3.95 is safe, though. */
                     if(major < 3 || (major == 3 && minor < 95))  /* || (major == 3 && minor == 95 && rest[0] == 0)) */
                     {
                        gain_offset = 6;
                     }
                  }
               }
               lame_offset += 9;
               /* the 4 big bits are tag revision, the small bits vbr method */
               // lame_vbr = bsbuf[lame_offset] & 15;
               lame_offset += 1;
               /* skipping: lowpass filter value */
               lame_offset += 1;
               /* replaygain */
               /* 32bit float: peak amplitude -- why did I parse it as int before??*/
               /* Ah, yes, lame seems to store it as int since some day in 2003; I've only seen zeros anyway until now, bah! */
               if
               (
                      (bsbuf[lame_offset] != 0)
                  || (bsbuf[lame_offset+1] != 0)
                  || (bsbuf[lame_offset+2] != 0)
                  || (bsbuf[lame_offset+3] != 0)
               )
               {
                  debug("Wow! Is there _really_ a non-zero peak value? Now is it stored as float or int - how should I know?");
                  /* byte*peak_bytes = (byte*) &peak;
                  ... endianess ... just copy bytes to avoid floating point operation on unaligned memory?
                  peak_bytes[0] = ...
                  peak = *(float*) (bsbuf+lame_offset); */
               }
               // peak = 0; /* until better times arrived */
               lame_offset += 4;
               /*
                  ReplayGain values - lame only writes radio mode gain...
                  16bit gain, 3 bits name, 3 bits originator, sign (1=-, 0=+), dB value*10 in 9 bits (fixed point)
                  ignore the setting if name or originator == 000!
                  radio 0 0 1 0 1 1 1 0 0 1 1 1 1 1 0 1
                  audiophile 0 1 0 0 1 0 0 0 0 0 0 1 0 1 0 0
               */

               for(i =0; i < 2; ++i)
               {
                  unsigned char origin = (bsbuf[lame_offset] >> 2) & 0x7; /* the 3 bits after that... */
                  if(origin != 0)
                  {
                     unsigned char gt = bsbuf[lame_offset] >> 5; /* only first 3 bits */
                     if(gt == 1) gt = 0; /* radio */
                     else if(gt == 2) gt = 1; /* audiophile */
                     else continue;
                     /* get the 9 bits into a number, divide by 10, multiply sign... happy bit banging */
                     replay_gain[gt] = (float) ((bsbuf[lame_offset] & 0x2) ? -0.1 : 0.1) * (make_short(bsbuf, lame_offset) & 0x1ff);
                     /* If this is an automatic value from LAME (or whatever), the automatic gain offset applies.
                        If a user or whoever set the value, do not touch it! 011 is automatic origin. */
                     if(origin == 3) replay_gain[gt] += gain_offset;
                  }
                  lame_offset += 2;
               }
               fprintf(stderr, "Note: Info: Radio Gain = %03.1fdB\n", replay_gain[0]);
               fprintf(stderr, "Note: Info: Audiophile Gain = %03.1fdB\n", replay_gain[1]);
               for(i=0; i < 2; ++i)
               {
                  if(rva.level[i] <= 0)
                  {
                     rva.peak[i] = 0; /* at some time the parsed peak should be used */
                     rva.gain[i] = replay_gain[i];
                     rva.level[i] = 0;
                  }
               }
               lame_offset += 1; /* skipping encoding flags byte */
               lame_offset += 1;
            }
            /* switch buffer back ... */
            bsbuf = bsspace[bsnum]+512;
            bsnum = (bsnum + 1) & 1;
            return 1; /* got it! */
         }
      }
   }
   return 0; /* no lame tag */
}

/*
   That's a big one: read the next frame. 1 is success, <= 0 is some error
   Special error READER_MORE means: Please feed more data and try again.
*/
int MpegStream::read_frame()
{
   /* TODO: rework this thing */
   unsigned long newhead;
   // int framepos;
   int ret= 0;
   /* stuff that needs resetting if complete frame reading fails */
   int oldsize  = mFrameSize;

   /* The counter for the search-first-header loop.
      It is persistent outside the loop to prevent seemingly endless loops
      when repeatedly headers are found that do not have valid followup headers. */
   long headcount = 0;

   mOldFrameSize=mFrameSize;       /* for Layer3 */

read_again:
   /* In case we are looping to find a valid frame, discard any buffered data before the current position.
      This is essential to prevent endless looping, always going back to the beginning when feeder buffer is exhausted. */
   forget();

   debug2("trying to get frame %"OFF_P" at %"OFF_P, (off_p)mFrameNum+1, (off_p)tell());
   if (!head_read(&newhead))
   {
      ret= MP3_DONE;
      goto read_frame_bad;
   }

init_resync:

   mHeaderChange = 2; /* output format change is possible... */
   if(mOldHead)        /* check a following header for change */
   {
      if(mOldHead == newhead) mHeaderChange = 0;
      else
      /* If they have the same sample rate. Note that only is _not_ the case for the first header, as we enforce sample rate match for following frames.
          So, during one stream, only change of stereoness is possible and indicated by header_change == 2. */
      if((mOldHead & HDR_SAMPMASK) == (newhead & HDR_SAMPMASK))
      {
         /* Now if both channel modes are mono or both stereo, it's no big deal. */
         if( header_mono(mOldHead) == header_mono(newhead))
            mHeaderChange = 1;
      }
   }

   if(!mFirstHead && !head_check(newhead))
   {
      ret = skip_junk(&newhead, &headcount);
      if(ret < 0){ goto read_frame_bad; }
      else if(ret == PARSE_AGAIN) goto read_again;
      else if(ret == PARSE_RESYNC) goto init_resync;
      else if(ret == PARSE_END) goto read_frame_bad;
   }

   ret = head_check(newhead);
   if(ret) ret = decode_header(newhead);

   if(ret < 0){ goto read_frame_bad; }
   else if(ret == PARSE_AGAIN) goto read_again;
   else if(ret == PARSE_RESYNC) goto init_resync;
   else if(ret == PARSE_END) goto read_frame_bad;

   if(ret == PARSE_BAD)
   { /* Header was not good. */
      ret = wetwork(&newhead); /* Messy stuff, handle junk, resync ... */
      if(ret < 0){ goto read_frame_bad; }
      else if(ret == PARSE_AGAIN) goto read_again;
      else if(ret == PARSE_RESYNC) goto init_resync;
      else if(ret == PARSE_END) goto read_frame_bad;
      /* Normally, we jumped already. If for some reason everything's fine to continue, do continue. */
      if(ret != PARSE_GOOD) goto read_frame_bad;
   }

   if(!mFirstHead)
   {
      ret = do_readahead(newhead);
      /* readahead can fail mit NEED_MORE, in which case we must also make the just read header available again for next go */
      if(ret < 0) back_bytes(4);
      if(ret < 0){ goto read_frame_bad; }
      else if(ret == PARSE_AGAIN) goto read_again;
      else if(ret == PARSE_RESYNC) goto init_resync;
      else if(ret == PARSE_END) goto read_frame_bad;
   }

   /* Now we should have our valid header and proceed to reading the frame. */

   /* if filepos is invalid, so is framepos */
   // framepos = tell() - 4;
   /* flip/init buffer for Layer 3 */
   {
      unsigned char *newbuf = bsspace[bsnum]+512;
      /* read main data into memory */
      if((ret=read_frame_body(newbuf,mFrameSize))<0)
      {
         /* if failed: flip back */
         debug("need more?");
         goto read_frame_bad;
      }
      bsbufold = bsbuf;
      bsbuf = newbuf;
   }
   bsnum = (bsnum + 1) & 1;

   if(!mFirstHead)
   {
      mFirstHead = newhead; /* _now_ it's time to store it... the first real header */
      /* This is the first header of our current stream segment.
         It is only the actual first header of the whole stream when num is still below zero!
         Think of resyncs where firsthead has been reset for format flexibility. */
      if(mFrameNum < 0)
      {
         if(mLayerId == 3 && check_lame_tag() == 1)
         {
            forget();

            mOldHead = 0;
            goto read_again;
         }
         /* now adjust volume */
         do_rva();
      }
   }

   mBitIndex = 0;
   wordpointer = (unsigned char *) bsbuf;
   mFrameNum++;

   forget();

   to_decode = 1;
   if(mErrorProtection)
      getbits(16); // skip 16bit crc

   mOldHead = newhead;

   return 1;

read_frame_bad:
   /* Also if we searched for valid data in vein, we can forget skipped data.
      Otherwise, the feeder would hold every dead old byte in memory until the first valid frame! */
   forget();

   mFrameSize = oldsize;
   /* That return code might be inherited from some feeder action, or reader error. */
   return MP3_ERR;
}

/* watch out for junk/tags on beginning of stream by invalid header */
int MpegStream::skip_junk(unsigned long *newheadp, long *headcount)
{
   int ret;
   long limit = 65536;
   unsigned long newhead = *newheadp;
   /* check for id3v2; first three bytes (of 4) are "ID3" */
   if((newhead & (unsigned long) 0x00ffffff) == (unsigned long) 0x334449)
   {
      return handle_id3v2(newhead);
   }

   /* I even saw RIFF headers at the beginning of MPEG streams ;( */
   if(newhead == ('R'<<24)+('I'<<16)+('F'<<8)+'F')
   {
      if (!head_read(&newhead))
         return MP3_NEED_MORE;

      while(newhead != ('d'<<24)+('a'<<16)+('t'<<8)+'a')
      {
         if (!head_shift(&newhead))
            return MP3_NEED_MORE;
      }
      if (!head_read(&newhead))
         return MP3_NEED_MORE;

      mOldHead = 0;
      *newheadp = newhead;
      return PARSE_AGAIN;
   }

   /*
      Unhandled junk... just continue search for a header, stepping in single bytes through next 64K.
      This is rather identical to the resync loop.
   */
   debug("searching for header...");
   *newheadp = 0; /* Invalidate the external value. */
   ret = 0; /* We will check the value after the loop. */

   /* We prepare for at least the 64K bytes as usual, unless
      user explicitly wanted more (even infinity). Never less. */
   if(p.resync_limit < 0 || p.resync_limit > limit)
   limit = p.resync_limit;

   do
   {
      ++(*headcount);
      if(limit >= 0 && *headcount >= limit) break;

      if (!head_shift(&newhead))
         return MP3_NEED_MORE;

      if(head_check(newhead) && (ret=decode_header(newhead))) break;
   } while(1);
   if(ret<0) return ret;

   if(limit >= 0 && *headcount >= limit)
   {
      return PARSE_END;
   }
   else
   {
      debug1("hopefully found one at %"OFF_P, (off_p)tell());
   }

   /* If the new header ist good, it is already decoded. */
   *newheadp = newhead;
   return PARSE_GOOD;
}

int MpegStream::do_readahead(unsigned long newhead)
{
   unsigned long nexthead = 0;
   int start, oret;
   int ret;

   if( ! (!mFirstHead) )
      return PARSE_GOOD;

   start = tell();

   /* step framesize bytes forward and read next possible header*/
   if((oret=skip_bytes(mFrameSize))<0)
   {
      return oret == MP3_NEED_MORE ? PARSE_MORE : PARSE_ERR;
   }

   /* Read header, seek back. */
   bool hd = head_read(&nexthead);
   if( back_bytes(tell()-start) < 0 )
   {
      return PARSE_ERR;
   }
   if (!hd) return PARSE_MORE;

   debug2("does next header 0x%08lx match first 0x%08lx?", nexthead, newhead);
   if(!head_check(nexthead) || (nexthead & HDR_CMPMASK) != (newhead & HDR_CMPMASK))
   {
      debug("No, the header was not valid, start from beginning...");
      mOldHead = 0; /* start over */
      /* try next byte for valid header */
      if((ret=back_bytes(3))<0)
      {
         return PARSE_ERR;
      }
      return PARSE_AGAIN;
   }
   else return PARSE_GOOD;
}

/* Read in the next frame we actually want for decoding.
   This includes skipping/ignoring frames, in additon to skipping junk in the parser. */
int MpegStream::get_next_frame()
{
   int change = decoder_change;

   do
   {
      to_decode = 0;
      int b = read_frame(); /* That sets to_decode only if a full frame was read. */
      if(b <= 0)
      {
         if(b==0 || (rdat.filelen >= 0 && rdat.filepos == rdat.filelen))
         {
            // We simply reached the end.
            return MP3_DONE;
         }
         else return MP3_ERR; /* Some real error. */
      }
      /* Now, there should be new data to decode ... and also possibly new stream properties */
      if(mHeaderChange > 1)
      {
         debug("big header change");
         change = 1;
      }
      /* Plain skipping without decoding, only when frame is not ignored on next cycle. */
      if(mFrameNum < firstframe)
      {
         if(!(0 && mFrameNum < firstframe && mFrameNum >= ignoreframe))
         {
            frame_skip();
         }
      }
   } while( mFrameNum < firstframe );

   /* If we reach this point, we got a new frame ready to be decoded.
      All other situations resulted in returns from the loop. */
   if(change)
   {
      if(decode_update() < 0)  /* dito... */
         return MP3_ERR;

      decoder_change = 0;
   }
   return MP3_OK;
}

int MpegStream::decode_update()
{
   // int b;

   if(mFrameNum < 0)
   {
      return MP3_ERR;
   }

   // Select the new output format based on given constraints.
   if (frame_output_format())
      new_format = 1; // Store for later...

   if(frame_buffers() != 0)
   {
      return MP3_ERR;
   }

   layer3.initStuff();
   init_layer12_stuff(this);

   /* We allocated the table buffers just now, so (re)create the tables. */
   make_decode_tables(this);

   /* The needed size of output buffer may have changed. */
   if (frame_outbuffer() != MP3_OK) return -1;

   do_rva();

   return 0;
}

