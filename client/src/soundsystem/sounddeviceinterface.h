#pragma once

#include "tools/array.h"
#include "tools/singleton.h"

class AudioSourceInterface;
class AudioMixer;

class SoundDeviceInterface
{
public:
   SoundDeviceInterface();
   virtual ~SoundDeviceInterface();

   static SoundDeviceInterface* Instance();

   virtual bool init(unsigned int hz=44100, int blocksize= 256, int blockcount= 64) = 0;
   virtual bool start();
   virtual void stop();
   virtual int postBlock(short*);
   virtual int getTotalBlock() const;
   virtual int getCurrentBlock();

   virtual void startCapture();
   virtual void stopCapture();

   void addAudioSource( AudioSourceInterface *source );
   void removeAudioSource( AudioSourceInterface *source );

   int sourceCount() const;
   AudioSourceInterface* source(int index) const;

   int getFrequency() const;
   int getProcessingBlock() const;
   int getBlockCount() const;
   int getBlockSize() const;
   short* getBlock(int num) const;

   void processBlock();

protected:
   Array<AudioSourceInterface*> mAudioSources;

   unsigned int  mFrequency;
   unsigned long mBlockSize;
   int           mBlockCount;
   short*        mBuffer;
   int           mProcessingBlock;
   AudioMixer*   mMixer;

   static SoundDeviceInterface* mInstance;
};
