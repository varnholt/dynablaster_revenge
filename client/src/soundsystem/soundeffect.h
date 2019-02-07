#pragma once

#include "tools/array.h"
#include "tools/string.h"

class AudioSample;
class Stream;

class SoundEffect
{
public:
   SoundEffect();
   ~SoundEffect();

   int getChannelCount() const;
   AudioSample* getChannel(int channel) const;
   bool load(Stream *stream);

   int getLeftVolume(int channel) const;
   int getRightVolume(int channel) const;

   const String& getFilename() const;

private:
   Array<AudioSample*> mChannels;
   String              mFilename;
};
