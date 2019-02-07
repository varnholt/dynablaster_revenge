#ifndef MIXER_H
#define MIXER_H

// Qt
#include <QObject>
#include <QString>

class Mixer : public QObject
{   
   Q_OBJECT

   public:

      //! constructor
      Mixer();

      //! destructor
      virtual ~Mixer();

      //! stream a track
      virtual void playMusic(const QString& fileName) = 0;

      //! pause a track
      virtual void pauseMusic() = 0;

      //! stop a track
      virtual void stopMusic() = 0;


   public slots:

      //! setter for music volume
      void setMusicVolume(float);

      //! setter for sfx volume
      void setSfxVolume(float);


   signals:

      //! music has ended
      void musicFinished();

      //! information about the track that is currently played
      void playing(
         const QString& artist,
         const QString& album,
         const QString& track
      );


   protected:


      float mMusicVolume;

      float mSfxVolume;
};

#endif // MIXER_H
