#include "audiosample.h"
#include "tools/stream.h"
#include <memory.h>

AudioSample::AudioSample()
: mBuffer(0)
, mLength(0)
, mLoopStart(0)
, mLoopLen(0)
, mLoopMode(0)
, mSampleRate(0)
{
}

AudioSample::AudioSample(int size, int rate)
: mBuffer(0)
, mLength(size)
, mLoopStart(0)
, mLoopLen(size)
, mLoopMode(AudioSample::NoLoop)
, mSampleRate(rate)
{
   mBuffer= new short[size+1];
   memset(mBuffer, 0, (size+1)*sizeof(short));
}

AudioSample::~AudioSample()
{
   delete[] mBuffer;
}

bool AudioSample::load(Stream * /*stream*/)
{
   return false;
}

bool AudioSample::save(Stream *)
{
   return false;
}

void AudioSample::clear()
{
   if (mBuffer)
      memset(mBuffer, 0, (mLength+1)*sizeof(short));
}
