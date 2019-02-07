#include "sampleplayer.h"
#include "audiomixer.h"
#include "audiosample.h"
#include "tools/filestream.h"
#include "sounddevice.h"
#include "soundeffect.h"

#include <QFileSystemWatcher>

SamplePlayer::SamplePlayer()
: AudioSourceInterface()
, mFilename(0)
, mFileWatch(0)
{
   // TODO: move to some management class to keep this qt free
   mFileWatch= new QFileSystemWatcher();
   // connect(mFileWatch, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
}

SamplePlayer::~SamplePlayer()
{
   if (mFileWatch)
      delete mFileWatch;
}

void SamplePlayer::fileChanged(const QString& fileName)
{
   String name( fileName.toLatin1() );
   for (int i=0; i<mSamples.size(); i++)
   {
      SoundEffect* sfx= mSamples[i];
      if ( sfx->getFilename() == name )
      {
         mFileWatch->removePath( fileName );
         printf("reloading sample '%s' \n", (const char*)sfx->getFilename());

         sfx= new SoundEffect();

         FileStream stream;
         if ( stream.open(name) )
         {
            if (sfx->load(&stream))
            {
               mSamples[i]= sfx;
            }
            stream.close();
         }

         mFileWatch->addPath( fileName );

         // TODO: delete old sfx
      }
   }
}

int SamplePlayer::addSample(const char *filename)
{
   int res= -1;

   FileStream stream;
   if (stream.open(filename))
   {
      SoundEffect *sfx= new SoundEffect();
      if (sfx->load(&stream))
      {
         res= mSamples.add(sfx);
         
         mFileWatch->addPath( QString( stream.getPath() ) );
      }
      else
      {
         printf("SamplePlayer::addSample: unable to open %s\n", filename);
      }

      stream.close();
   }

   return res;
}

void SamplePlayer::play(int id, const Vector& /*pos*/)
{
   float volume = 0.0f;
   if (!isMuted())
      volume = getVolume();

   if (id>=0 && id<mSamples.size())
   {
      SoundEffect *sfx= mSamples[id];

      for (int chn=0; chn<sfx->getChannelCount(); chn++)
      {
         AudioSample* sample= sfx->getChannel(chn);
         MixerChannel *channel= AudioMixer::Instance()->addChannel();
         channel->SetSample( sample, sample->mSampleRate );               
         channel->mLeftVolume= sfx->getLeftVolume(chn) * volume;
         channel->mRightVolume= sfx->getRightVolume(chn) * volume;

         mChannels.add( channel );
      }
   }
}

int SamplePlayer::process(int numSamples)
{
   for (int chn=0; chn<mChannels.size(); )
   {
      MixerChannel *channel= mChannels[chn];

      if ((channel->mMixPos>>32) >= channel->Sample()->mLoopLen-1)
      {
         // finished playing
         // printf("sample finished.\n");
         mChannels.remove(channel);
         AudioMixer::Instance()->removeChannel(channel);
      }
      else
         chn++;
   }
   return numSamples;
}

int SamplePlayer::capacity() const
{
   return 1024;
}

int SamplePlayer::getChannelCount() const
{
   return mChannels.size();
}


void SamplePlayer::toggleMute()
{
   AudioSourceInterface::toggleMute();

   for (int i = 0; i < mChannels.size(); i++)
      mChannels[i]->toggleMute();
}


bool SamplePlayer::isMuted() const
{
   return AudioSourceInterface::isMuted();
}

