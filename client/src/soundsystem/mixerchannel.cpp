#include "mixerchannel.h"
#include "sounddeviceinterface.h"
#include "audiomixer.h"


MixerChannel::MixerChannel()
: mVolume(0),
  mFrequency(0),
  mPanning(0),
  mSampleOffset(0),
  mLeftVolume(0),
  mRightVolume(0),
  mMixPos(0),
  mSpeed(0),
  mSpeedDir(0),
  mRampLeftTarget(0),
  mRampRightTarget(0),
  mRampLeftVolume(0),
  mRampRightVolume(0),
  mRampLeftSpeed(0.0f),
  mRampRightSpeed(0.0f),
  mRampCount(0),
  mSample(0),
  mMute(false)
{
}

unsigned int MixerChannel::SpeedDir() const
{
   return mSpeedDir;
}

void MixerChannel::SetSpeedDir(unsigned int dir)
{
   mSpeedDir= dir;
}

AudioSample* MixerChannel::Sample() const
{
   return mSample;
}

void MixerChannel::SetSample(AudioSample* sample, int sampleRate)
{
   mSample= sample;
   mSampleOffset = 0;
   mSpeedDir= AudioMixer::Forwards;
   mMixPos= 0;
   mRampLeftVolume	= 0;
   mRampRightVolume	= 0;
   mRampCount		= 0;

   if (sampleRate>=0)
   {
      mFrequency= sampleRate;
      int rate= SoundDeviceInterface::Instance()->getFrequency();
      mSpeed= (long long) (65536.0 * 65536.0 * mFrequency / rate);
   }
}

bool MixerChannel::isMuted() const
{
   return mMute;
}

void MixerChannel::setMuted(bool mute)
{
   mMute= mute;
}


void MixerChannel::toggleMute()
{
   setMuted(!isMuted());
}


unsigned int MixerChannel::getLeftVolume() const
{
   return mLeftVolume;
}


unsigned int MixerChannel::getRightVolume() const
{
   return mRightVolume;
}


void MixerChannel::setLeftVolume(unsigned int volume)
{
   mLeftVolume = volume;
}


void MixerChannel::setRightVolume(unsigned int volume)
{
   mRightVolume = volume;
}

