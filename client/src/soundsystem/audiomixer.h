#ifndef MIXER_H
#define MIXER_H

#include "mixerchannel.h"
#include "tools/array.h"
#include "tools/singleton.h"

#define MIXER_VOLUMERAMP_STEPS	128		// at 44.1khz

class SoundDevice;

class AudioMixer : public Singleton<AudioMixer>
{
public:
   enum Direction
   {
      Forwards = 1,
      Backwards = 2
   };

   AudioMixer();

   MixerChannel*  addChannel();
   bool           removeChannel(MixerChannel *channel);

   void           convert(short *dst, float *src, int len);
   void           process(float *mixptr, int len);

private:
   void           processLoop(long long& lmixpos, AudioSample *pSample, MixerChannel* pChannel);

   int            mFrequency;
   Array<MixerChannel*>	mChannels;
   int            mRampSteps;
   float          mInvRampSteps;
};

#endif
