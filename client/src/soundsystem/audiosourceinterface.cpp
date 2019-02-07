#include "audiosourceinterface.h"

AudioSourceInterface::AudioSourceInterface()
 : mVolume(1.0f),
   mMuted(false)
{
}

AudioSourceInterface::~AudioSourceInterface()
{
}


void AudioSourceInterface::setVolume(float volume)
{
   mVolume = volume;
}


float AudioSourceInterface::getVolume() const
{
   return mVolume;
}


void AudioSourceInterface::toggleMute()
{
   mMuted = !mMuted;
}


bool AudioSourceInterface::isMuted() const
{
   return mMuted;
}
