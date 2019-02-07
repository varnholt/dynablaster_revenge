#pragma once

#include <stdio.h>
#include "layer1.h"
#include "layer2.h"
#include "layer3.h"
#include "id3.h"
#include "frameparameter.h"

enum MpegErrorState
{
	MP3_DONE=-4,
	MP3_NEW_FORMAT=-3,
	MP3_NEED_MORE=-2,
	MP3_ERR=-1,
	MP3_OK=0
};

#define SINGLE_STEREO -1
#define SINGLE_LEFT    0
#define SINGLE_RIGHT   1
#define SINGLE_MIX     3

#define HDR_SYNC              0x0000e0ff

// 00000000 00000000 00011000 00000000
#define HDR_VERSION           0x00001800
#define HDR_VERSION_VAL(h)    (((h)&HDR_VERSION) >> 11)

// 00000000 00000000 00000110 00000000
#define HDR_LAYER             0x00000600
#define HDR_LAYER_VAL(h)      (((h)&HDR_LAYER) >> 9)

// 00000000 00000000 00000001 00000000
#define HDR_CRC               0x00000100
#define HDR_CRC_VAL(h)        (((h)&HDR_CRC) >> 8)

// 00000000 11110000 00000000 00000000
#define HDR_BITRATE           0x00f00000
#define HDR_BITRATE_VAL(h)    (((h)&HDR_BITRATE) >> 20)

// 00000000 00001100 00000000 00000000
#define HDR_SAMPLERATE        0x000c0000
#define HDR_SAMPLERATE_VAL(h) (((h)&HDR_SAMPLERATE) >> 18)

// 00000000 00000010 00000000 00000000
#define HDR_PADDING           0x00020000
#define HDR_PADDING_VAL(h)    (((h)&HDR_PADDING) >> 17)

// 11000000 00000000 00000000 00000000
#define HDR_CHANNEL           0xc0000000
#define HDR_CHANNEL_VAL(h)    (((h)&HDR_CHANNEL) >> 30)

// 00110000 00000000 00000000 00000000
#define HDR_CHANEX            0x30000000
#define HDR_CHANEX_VAL(h)     (((h)&HDR_CHANEX) >> 28)

#define HDR_CMPMASK (HDR_SYNC|HDR_VERSION|HDR_LAYER|HDR_SAMPLERATE)

/* A stricter mask, for matching free format headers. */
#define HDR_SAMEMASK (HDR_SYNC|HDR_VERSION|HDR_LAYER|HDR_BITRATE|HDR_SAMPLERATE|HDR_CHANNEL|HDR_CHANEX)

/* Free format headers have zero bitrate value. */
#define HDR_FREE_FORMAT(head) (!(head & HDR_BITRATE))

/* A mask for changed sampling rate (version or rate bits). */
#define HDR_SAMPMASK (HDR_VERSION|HDR_SAMPLERATE)

/* max = 1728 */
#define MAXFRAMESIZE 3456

struct al_table
{
  short bits;
  short d;
};

/* the output buffer, used to be pcm_sample, pcm_point and audiobufsize */
struct outbuffer
{
	short *data; /* main data pointer, aligned */
	size_t fill; /* fill from read pointer */
};

class ReaderData
{
public:
   int filelen; /* total file length or total buffer size */
   int filepos; /* position in file or position in buffer chain */
   char* buffer; /* Not dynamically allocated, these few struct bytes aren't worth the trouble. */
};


/* There is a lot to condense here... many ints can be merged as flags; though the main space is still consumed by buffers. */
class MpegStream
{
public:
   MpegStream();
   ~MpegStream();

   bool open(char *buffer, int size);
   int read_frame_body(unsigned char *buf, int size);
   int fullread(unsigned char *buf, int count);
   int tell();
   bool head_read(unsigned long *newhead);
   bool head_shift(unsigned long *head);
   int skip_bytes(int len);
   int back_bytes(int bytes);
   void forget();
   void close();
   int bitrate() const;
   int frequency() const;
   void fixed_reset();
   void free_buffers();
   void decode_buffers_reset();

   void init(FrameParameter *mp);
   int buffers_reset();
   void do_rva();
   int get_rva(double *peak, double *gain);

   // TODO: frame class
   int frame_reset();
   void frame_exit();
   int frame_outs(int mFrameNum);
   int frame_expect_outsamples();
   void frame_skip();
   bool frame_output_format();
   int  frame_outbuffer();
   int frame_buffers();
   int read_frame();
   int get_next_frame();
   int decode_frame(short **audio);
   void decode_the_frame();

   // TODO: Bitstream class
   int getbitoffset();
   unsigned char getbyte();
   void backbits(int nob);
   unsigned int getbits( int number_of_bits );
   unsigned int getbits_fast(int nob);
   unsigned int get1bit();
   void skipbits(int nob);

   int spf() const;
   int skip_junk(unsigned long *newheadp, long *headcount);
   int do_readahead(unsigned long newhead);
   int decode_update();
   int wetwork(unsigned long *newheadp);
   int decode_header(unsigned long newhead);
   int guess_freeformat_framesize();
   int handle_id3v2(unsigned long newhead);
   int check_lame_tag();
   void set_pointer(long backstep);

   int do_layer();

	int new_format;
	float hybrid_block[2][2][SBLIMIT*SSLIMIT];
	int hybrid_blc[2];

	float *real_buffs[2][2];
	unsigned char *rawbuffs;

	int bo; /* Just have it always here. */
	float *decwin; /* _the_ decode table */

   Layer1 layer1;
   Layer2 layer2;
   Layer3 layer3;

	/* layer2 */
	float muls[27][64];	/* also used by layer 1 */

	const struct al_table *alloc;

	int mChannels; /* I _think_ 1 for mono and 2 for stereo */
	int mJointStereo;
	int lsf; /* 0: MPEG 1.0; 1: MPEG 2.0/2.5 -- both used as bool and array index! */
	/* Many flags in disguise as integers... wasting bytes. */
   int mMpeg25;
   int mHeaderChange;
   int mLayerId;
   int mErrorProtection;
   int mBitrateIndex;
   int mSampleRate;
   int mPadding;
   int mMode;
   int mModeExt;
   int mFrameSize; /* computed framesize */
   int mFrameNum; /* frame offset ... */
   long mFreeFormatFrameSize;

	/* bitstream info; bsi */
   int mBitIndex;

   // bit stuff
	unsigned char *wordpointer;
	unsigned long ultmp;
	unsigned char uctmp;

	double lastscale;
	struct
	{
		int level[2];
		float gain[2];
		float peak[2];
	} rva;

	/* input data */
   int mOldFrameSize;
	int ssize;
	unsigned int bitreservoir;
	unsigned char bsspace[2][MAXFRAMESIZE+512]; /* MAXFRAMESIZE */
	unsigned char *bsbuf;
	unsigned char *bsbufold;
	int bsnum;
   unsigned long mOldHead;
   unsigned long mFirstHead;

	/* output data */
	struct outbuffer buffer;
   int mCurSampleRate;
   int mCurChannels;

	int to_decode;   /* this frame holds data to be decoded */
	int firstframe;  /* start decoding from here */
	int ignoreframe; /* frames to decode but discard before firstframe */
	ReaderData rdat; /* reader data and state info */
	FrameParameter p;
	int err;
	int decoder_change;
	long clip;

   Id3Tag mId3Tag;

   float *layerscratch;
   static int initialized;
};
