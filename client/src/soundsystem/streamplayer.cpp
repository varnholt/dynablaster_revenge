#include "streamplayer.h"
#include "wave.h"
#include "audiomixer.h"
#include "audiosample.h"
#include "tools/stream.h"
#include "sounddevice.h"
#include "streamsample.h"
#include "../mp3/mpegstream.h"

#include <QMutexLocker>

StreamPlayer::StreamPlayer()
: AudioSourceInterface()
, mStream(0)
, mChannels(0)
, mStreamData(0)
, mSize(4096) // storage size for one mp3 frame and some bonus
, mBack(0)
, mFront(0)
, mBlock(0)
, mBlockCount(16)
, mFinished(true)
{
   for (int chn=0; chn<2; chn++)
   {
      mSample[chn]= 0;
      mChannel[chn]= 0;
      mBuffer[chn]= 0;
   }
}

StreamPlayer::~StreamPlayer()
{
}

const Id3Tag& StreamPlayer::getId3() const
{
   QMutexLocker lock(&mMutex);
   return mStream->mId3Tag;
}

void StreamPlayer::setVolume(float volume)
{
   AudioSourceInterface::setVolume(volume);

   int vol = volume * 255;

   if (mChannel[0])
   {
      mChannel[0]->mLeftVolume  = vol;
      mChannel[0]->mRightVolume = 0;
   }

   if (mChannel[1])
   {
      mChannel[1]->mLeftVolume  = 0;
      mChannel[1]->mRightVolume = vol;
   }
}


bool StreamPlayer::play(char *buffer, int size)
{
   QMutexLocker lock(&mMutex);

   if (!mStream)
      mStream= new MpegStream();

   if (!mStream->open(buffer, size))
      return false;

   mFinished= false;

   // printf("sample-rate: %d channels: %d \n", mStream->frequency(), mStream->mChannels );

   int panning= 0;
   mChannels= mStream->mChannels;
   if (mChannels==1) panning=128;
   for (int chn=0; chn<mChannels; chn++)
   {
      if (mBuffer[chn]==0)
         mBuffer[chn]= new short[mSize];

      if (mSample[chn]==0)
         mSample[chn]= new StreamSample( mBlockCount * 256 );

      if (mChannel[chn]==0)
      {
         mChannel[chn]= AudioMixer::Instance()->addChannel();
         mChannel[chn]->SetSample( mSample[chn], mStream->frequency() );
         mChannel[chn]->mLeftVolume= (255-panning) * getVolume();
         mChannel[chn]->mRightVolume= (panning) * getVolume();
      }

      panning= 255-panning;
   }

   return true;
}

void StreamPlayer::stop()
{
   QMutexLocker lock(&mMutex);

   mFinished= true;

   if (mStream)
   {
      for (int chn=0; chn<mChannels; chn++)
      {
         if (mBuffer[chn])
         {
            memset(mBuffer[chn], 0, sizeof(short) * mSize);
         }

         if (mSample[chn])
         {
            mSample[chn]->clear();
         }

         if (mChannel[chn])
         {
            mChannel[chn]->mLeftVolume= 0;
            mChannel[chn]->mRightVolume= 0;
         }
      }

      delete mStream;
      mStream= 0;
   }
}

void StreamPlayer::pause()
{
   // TODO:
}

void StreamPlayer::storeData(short *dst, short *src, int size)
{
   if (mStream)
   {
      int step= mChannels;
      for (int i=0; i<size; i++)
         dst[i]= src[i*step];
   }
}

int StreamPlayer::available() const
{
   if (mBack >= mFront)
      return mBack-mFront;
   else
      return mSize - mFront + mBack;
}

void StreamPlayer::enqueue(short *src, int size)
{
   int chn= mChannels;
   while (size > 0)
   {
      int avail;
      if (mBack >= mFront)
         avail= mSize - mBack;
      else
         avail= mFront - mBack;

      int count= size;
      if (count > avail) count= avail;

      if (mBuffer[0])
         storeData(mBuffer[0] + mBack, src, count);
      if (mBuffer[1])
         storeData(mBuffer[1] + mBack, src+1, count);

      src+=count*chn;
      size-=count;
      mBack += count;
      if (mBack >= mSize)
         mBack= 0;
   }
}

void StreamPlayer::dequeue(int size)
{
   short *left= 0;
   short *right= 0;

   if (mSample[0])
      left= mSample[0]->mBuffer + mBlock*256;

   if (mSample[1])
      right= mSample[1]->mBuffer + mBlock*256;

   while (size > 0)
   {
      int avail;
      if (mFront > mBack)
         avail= mSize - mFront;
      else
         avail= mBack - mFront;

      int count= size;
      if (count > avail) count= avail;

      if (left)
         for (int i=0; i<count; i++)
            left[i]= mBuffer[0][mFront+i];

      if (right)
         for (int i=0; i<count; i++)
            right[i]= mBuffer[1][mFront+i];

      mFront+=count;
      size-=count;
      if (mFront >= mSize)
         mFront= 0;
   }
}

void StreamPlayer::decode(int size)
{
   while (size > 0)
   {
      short *audio;
      bool emitFinished= false;

      mMutex.lock();
      if (!mFinished && mStream)
      {
         int ret = mStream->decode_frame(&audio);

         if (ret > 0)
         {
            //         printf("decoded: %d \n", ret/2);
            if (mChannels==1)
               enqueue(audio, ret);
            else
               enqueue(audio, ret/2);

            size-=ret/2;
         }
         else
         {
            mFinished= true;
            emitFinished= true;
         }
      }
      mMutex.unlock();

      if (emitFinished)
         emit finished();

      // just output flatline
      if (mFinished || !mStream)
      {
         short tmp[512];
         memset(tmp, 0, 512*2);
         enqueue(tmp, 256);
         size-=256;
      }
   }
}

// fill the playing channel(s) with new data up to the current playing position
int StreamPlayer::process(int numSamples)
{
   if (mChannel[0]!=0)
   {
      int play= mChannel[0]->mMixPos >> 32;
      int playBlock= (play >> 8) % mBlockCount;

      while (mBlock != playBlock)
      {
         if (available() < 512)
            decode(256);

         dequeue( 256 );

         if (mBlock==0)
         {
            if (mSample[0])
               mSample[0]->mBuffer[mSample[0]->mLoopLen]= mSample[0]->mBuffer[0];
            if (mSample[1])
            mSample[1]->mBuffer[mSample[1]->mLoopLen]= mSample[1]->mBuffer[0];
         }

         mBlock++;
         if (mBlock >= mBlockCount)
            mBlock= 0;
      }
   }

   return numSamples;
}

int StreamPlayer::capacity() const
{
   return 1024;
}


void StreamPlayer::toggleMute()
{
   AudioSourceInterface::toggleMute();

   for (int i = 0; i < mChannels; i++)
      mChannel[i]->toggleMute();
}


bool StreamPlayer::isMuted() const
{
   return AudioSourceInterface::isMuted();
}


