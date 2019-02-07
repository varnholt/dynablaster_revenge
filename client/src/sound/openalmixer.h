#ifndef OPENALMIXER_H
#define OPENALMIXER_H

#include <QTime>
#include <QTimer>
#include <QMap>
#include <QVector>

#include <AL/al.h>

// framework
#include "mixer.h"
#include "vector.h"

class OpenAlMixerStreamThread;

class OpenAlMixer : public Mixer
{
   Q_OBJECT

   public:

      //! constructor
      OpenAlMixer();

      //! destructor
      virtual ~OpenAlMixer();

      //! initialize listener properties
      void initListener(
         const Vector& position = Vector(0.0, 0.0, 0.0),
         const Vector& velocity = Vector(0.0, 0.0, 0.0),
         const Vector& orientation = Vector(0.0, 0.0, -1.0),
         const Vector& orientationUp = Vector(0.0, 1.0, 0.0)
      );

      //! stream a track
      void playMusic(const QString& fileName);

      //! pause a track
      void pauseMusic();

      //! stop a track
      void stopMusic();

      //! preload sound effects to ram
      void loadSoundEffects(
         const QList<QString> fileNames
      );

      //! play a soundeffect
      void playSoundEffect(
         int bufferId,
         const Vector& position = Vector(0.0, 0.0, 0.0),
         const Vector& velocity = Vector(0.0, 0.0, 0.0),
         const Vector& direction = Vector(0.0, 0.0, 0.0),
         float pitch = 1.0f,
         float gain = 1.0f
      );

      //! getter for soundeffect ids
      int getSoundEffectId(const QString&);


   private:

      //! listener's position
      Vector mListenerPosition;

      //! listener's velocity
      Vector mListenerVelocity;

      //! listener's orientation
      Vector mListenerOrientation;

      //! listener orientation's up vector
      Vector mListenerOrientationUp;



      //! buffers storing sound effects
      QVector<ALuint> mSoundEffectBuffers;

      //! map containing filenames pointing to buffer ids
      QMap<QString, int> mSoundEffects;


      // music stream thread
      OpenAlMixerStreamThread* mStreamThread;
};

#endif // OPENALMIXER_H
