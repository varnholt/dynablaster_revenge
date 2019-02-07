// header
#include "openalmixer.h"

// mixer
#include "openalmixerstreamthread.h"

// std
#include <stdlib.h>
#include <stdio.h>
#include "math.h"

// openal
#include <AL/alc.h>
#include <AL/alut.h>



void cleanUp()
{
//   alDeleteBuffers(NUM_BUFFERS, &Buffers[0]);
//   alDeleteSources(NUM_SOURCES, &Sources[0]);
   alutExit();
}



OpenAlMixer::OpenAlMixer()
   : mStreamThread(0)
{
   // initialize openal and clear the error bit
   alutInit(NULL, 0);
   alGetError();

   // setup an exit procedure
   atexit(cleanUp);

   // init stream thread
   mStreamThread = new OpenAlMixerStreamThread();

   connect(
      mStreamThread,
      SIGNAL(finished()),
      this,
      SIGNAL(musicFinished())
   );

   connect(
      mStreamThread,
      SIGNAL(playing(QString,QString,QString)),
      this,
      SIGNAL(playing(QString,QString,QString)),
      Qt::QueuedConnection
   );
}


OpenAlMixer::~OpenAlMixer()
{
   delete mStreamThread;
   mStreamThread = 0;
}


void OpenAlMixer::playMusic(const QString& fileName)
{
   mStreamThread->playMusic(fileName);
}


void OpenAlMixer::pauseMusic()
{
   mStreamThread->pauseMusic();
}


void OpenAlMixer::stopMusic()
{
   mStreamThread->stopMusic();
}


void OpenAlMixer::playSoundEffect(
   int bufferId,
   const Vector& position,
   const Vector& velocity,
   const Vector& direction,
   float pitch,
   float gain
)
{
   // emit the same sound at different positions
   // at the same time
   //
   // => generate the source at play time
   ALuint source = 0;
   alGenSources(1, &source);

   // bind buffer to audio source
   alSourcei(source, AL_BUFFER, bufferId);

   // setup source
   alSourcefv(source, AL_POSITION, position.data());
   alSourcefv(source, AL_VELOCITY, velocity.data());
   alSourcefv(source, AL_DIRECTION, direction.data());
   alSourcef(source, AL_PITCH, pitch);
   alSourcef(source, AL_GAIN, gain);
   alSourcei(source, AL_LOOPING, false);

   // play sound
   alSourcePlay(source);

   // TODO: clean up source

}


void OpenAlMixer::initListener(
   const Vector& position,
   const Vector& velocity,
   const Vector& orientation,
   const Vector& orientationUp
)
{
   mListenerPosition = position;
   mListenerVelocity = velocity;
   mListenerOrientation = orientation;
   mListenerOrientationUp = orientationUp;

   // build combined vector for the listener
   ALfloat listenerOrientation[] =
   {
      mListenerOrientation.x,
      mListenerOrientation.y,
      mListenerOrientation.z,
      mListenerOrientationUp.x,
      mListenerOrientationUp.y,
      mListenerOrientationUp.z
   };

   alListenerfv(AL_POSITION, mListenerPosition.data());
   alListenerfv(AL_VELOCITY, mListenerVelocity.data());
   alListenerfv(AL_ORIENTATION, listenerOrientation);
}


void OpenAlMixer::loadSoundEffects(
   const QList<QString> fileNames
)
{   
   int effectCount = fileNames.size();

   // resize buffers
   mSoundEffectBuffers.resize(effectCount);

   // generate buffers
   alGenBuffers(effectCount, mSoundEffectBuffers.data());

   if (alGetError() == AL_NO_ERROR)
   {
      ALenum format = 0;
      ALsizei size = 0;
      ALvoid* data = 0;
      ALsizei frequency = 0;
      ALboolean loop = false;

      for (int i = 0; i < effectCount; i++)
      {
         alutLoadWAVFile(
            (ALbyte*)qPrintable(fileNames[i]),
            &format,
            &data,
            &size,
            &frequency,
            &loop
         );

         alBufferData(
            mSoundEffectBuffers[i],
            format,
            data,
            size,
            frequency
         );

         alutUnloadWAV(
            format,
            data,
            size,
            frequency
         );

         // init default settings
         alSource3f(i, AL_POSITION,        0.0, 0.0, 0.0);
         alSource3f(i, AL_VELOCITY,        0.0, 0.0, 0.0);
         alSource3f(i, AL_DIRECTION,       0.0, 0.0, 0.0);
         alSourcef(i,  AL_ROLLOFF_FACTOR,  0.0          );
         alSourcef(i,  AL_PITCH,           1.0          );
         alSourcef(i,  AL_GAIN,            1.0          );
         alSourcei(i,  AL_SOURCE_RELATIVE, AL_TRUE      );
         alSourcei(i,  AL_LOOPING,         AL_FALSE     );

         // store filename and id in the map
         mSoundEffects.insert(fileNames[i], mSoundEffectBuffers[i]);
      }
   }
}


int OpenAlMixer::getSoundEffectId(const QString& name)
{
   int id = -1;

   if (mSoundEffects.contains(name))
      id = mSoundEffects[name];

   return id;
}
