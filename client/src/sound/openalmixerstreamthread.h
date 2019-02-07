#ifndef MIXERSTREAMTHREAD_H
#define MIXERSTREAMTHREAD_H


#include "openaloggloader.h"

#include <QEvent>
#include <QMutex>
#include <QQueue>
#include <QThread>


class OpenAlMixerStreamThread : public QThread
{
    Q_OBJECT

   public:

      //! constructor
      OpenAlMixerStreamThread(QObject *parent = 0);

      //! destructor
      virtual ~OpenAlMixerStreamThread();

      //! play a track
      void playMusic(const QString& filename);

      //! pause music
      void pauseMusic();

      //! stop music
      void stopMusic();

      void setStopped(int stopped);

      int isStopped() const;


   signals:

      void musicFinished();

      void playing(
         const QString& artist,
         const QString& album,
         const QString& track
      );


   protected:

      //! overwrite qthread's run
      void run();


   private:

      //! serialize access to the thread
      mutable QMutex mMutex;

      //! openal loader
      OpenAlOggLoader* mOggLoader;

      //! stopped flag
      int mStopped;
};

#endif // MIXERSTREAMTHREAD_H
