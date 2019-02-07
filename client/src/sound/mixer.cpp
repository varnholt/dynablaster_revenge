#include "mixer.h"


Mixer::Mixer()
 : QObject(),
   mMusicVolume(0.0f),
   mSfxVolume(0.0f)
{
}


Mixer::~Mixer()
{
}


void Mixer::setMusicVolume(float volume)
{
   mMusicVolume = volume;
}


void Mixer::setSfxVolume(float volume)
{
   mSfxVolume = volume;
}
