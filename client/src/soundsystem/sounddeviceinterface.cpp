#include "sounddeviceinterface.h"
#include "tools/array.h"
#include "audiosourceinterface.h"
#include "audiomixer.h"
#include <memory.h>
#include <stdio.h>

SoundDeviceInterface* SoundDeviceInterface::mInstance = 0;

SoundDeviceInterface::SoundDeviceInterface()
: mFrequency(0)
, mBlockSize(0)
, mBlockCount(0)
, mBuffer(0)
, mProcessingBlock(0)
, mMixer(0)
{
   if (mInstance)
   {
      printf("sound device instance already exists!");
   }
   else
   {
      mInstance= this;
   }
}

SoundDeviceInterface::~SoundDeviceInterface()
{
}

SoundDeviceInterface* SoundDeviceInterface::Instance()
{
   return mInstance;
}

bool SoundDeviceInterface::start()
{
   return false;
}

void SoundDeviceInterface::stop()
{
}

void SoundDeviceInterface::startCapture()
{
}

void SoundDeviceInterface::stopCapture()
{
}

int SoundDeviceInterface::postBlock(short*)
{
   return 0;
}

int SoundDeviceInterface::getTotalBlock() const
{
   return 0;
}

int SoundDeviceInterface::getCurrentBlock()
{
   return 0;
}


void SoundDeviceInterface::addAudioSource( AudioSourceInterface *source )
{
   mAudioSources.add( source );
}

void SoundDeviceInterface::removeAudioSource( AudioSourceInterface *source )
{
   mAudioSources.remove( source );
}

int SoundDeviceInterface::sourceCount() const
{
   return mAudioSources.size();
}

AudioSourceInterface* SoundDeviceInterface::source(int index) const
{
   if (index >= 0 && index < mAudioSources.size())
      return mAudioSources[index];
   else
      return 0;
}

void SoundDeviceInterface::processBlock()
{
   int blocksize= mBlockSize;
   int blockpos= 0;
   float buffer[4096];

   // clear temp mixing buffer
   memset(buffer, 0, blocksize*8);

   while (blocksize > 0)
   {
      // get minimum size
      int minBlock= blocksize;
      for (int i=0; i< sourceCount(); i++)
      {
         AudioSourceInterface *src= source(i);
         int size= src->capacity();
         if (size < minBlock) minBlock= size;
      }

      for (int i=0; i< sourceCount(); i++)
      {
         AudioSourceInterface *src= source(i);
         src->process( minBlock );
      }

      mMixer->process( buffer + blockpos, minBlock );

      blockpos += minBlock*2;
      blocksize-= minBlock;
   }

   short temp[4096];
   AudioMixer::Instance()->convert(temp, buffer, mBlockSize);

   postBlock(temp);
}


int SoundDeviceInterface::getFrequency() const
{
   return mFrequency;
}

short* SoundDeviceInterface::getBlock(int num) const
{
   return mBuffer + (mBlockSize*num*2);
}


int SoundDeviceInterface::getBlockCount() const
{
   return mBlockCount;
}

int SoundDeviceInterface::getBlockSize() const
{
   return mBlockSize;
}

int SoundDeviceInterface::getProcessingBlock() const
{
   return mProcessingBlock;
}

