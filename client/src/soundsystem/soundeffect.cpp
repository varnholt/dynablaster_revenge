#include "soundeffect.h"
#include "wave.h"
#include "tools/stream.h"
#include "audiosample.h"
#include <string.h>

SoundEffect::SoundEffect()
{
}

SoundEffect::~SoundEffect()
{
   for (int i=0; i<mChannels.size(); i++)
      delete mChannels[i];
}

int SoundEffect::getChannelCount() const
{
   return mChannels.size();
}

AudioSample* SoundEffect::getChannel(int channel) const
{
   if (channel>=0 && channel<mChannels.size())
      return mChannels[channel];
   else
      return 0;
}

int SoundEffect::getLeftVolume(int channel) const
{
   if (mChannels.size()==1)
      return 255;
   else
      return channel*255;
}

int SoundEffect::getRightVolume(int channel) const
{
   if (mChannels.size()==1)
      return 255;
   else
      return 255 - channel*255;
}

const String& SoundEffect::getFilename() const
{
   return mFilename;
}

bool SoundEffect::load(Stream *stream)
{
   Wave::Header header;

   // borked?
   if (!header.load(stream))
      return false;

   // invalid file format?
   if (!(header.channels>=1 && header.channels<=2 && header.bits==16))
      return false;

   int size= header.datasize >> (header.channels);

   Array<AudioSample*> temp( header.channels );

   // allocate sample data for each channel
   for (int chn=0; chn<header.channels; chn++)
   {
      AudioSample *sample= new AudioSample(size, header.rate);
      temp.add(sample);
   }

   // load interleaved sample data into channels
   for (int i=0; i<size; i++)
   {
      for (int chn=0; chn<header.channels; chn++)
      {
         temp[chn]->mBuffer[i]= stream->getShort();
      }
   }

   // copy last value of sample to make filtering easier
   for (int chn=0; chn<header.channels; chn++)
      temp[chn]->mBuffer[size]= temp[chn]->mBuffer[size-1];

   // set filename
   mFilename= stream->getPath();

   // TODO: delete old samples
   mChannels= temp;

   return true;
}
