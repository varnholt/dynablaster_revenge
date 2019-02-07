
// header
#include "openalmixerstreamthread.h"

// Qt
#include <QString>


OpenAlMixerStreamThread::OpenAlMixerStreamThread(QObject *parent) :
    QThread(parent),
    mOggLoader(0),
    mStopped(-1)
{   
   mOggLoader = new OpenAlOggLoader();

   connect(
      mOggLoader,
      SIGNAL(playing(QString,QString,QString)),
      this,
      SIGNAL(playing(QString,QString,QString))
   );
}


OpenAlMixerStreamThread::~OpenAlMixerStreamThread()
{
   if (isStopped() >= 0)
   {
      setStopped(1);

      while (isStopped() != 2)
         msleep(10);
   }

   mMutex.lock();
   delete mOggLoader;
   mOggLoader = 0;
   mMutex.unlock();
}


void OpenAlMixerStreamThread::setStopped(int stopped)
{
   mMutex.lock();
   mStopped = stopped;
   mMutex.unlock();
}


int OpenAlMixerStreamThread::isStopped() const
{
   int stopped = -1;

   mMutex.lock();
   stopped = mStopped;
   mMutex.unlock();

   return stopped;
}


void OpenAlMixerStreamThread::playMusic(const QString& filename)
{
   // reset stop flag
   setStopped(0);

   // open and play
   mMutex.lock();

   mOggLoader->open(filename);
   // mOggLoader->display();

   if (mOggLoader->play())
      start();

   mMutex.unlock();
}


void OpenAlMixerStreamThread::pauseMusic()
{

}


void OpenAlMixerStreamThread::stopMusic()
{

}


void OpenAlMixerStreamThread::run()
{
   bool trackFinished = false;
   setStopped(0);

   while (!isStopped() && !trackFinished)
   {
      if (mOggLoader->update())
      {
         if (!mOggLoader->isPlaying())
         {
            if (!mOggLoader->play())
            {
               qWarning("MixerStreamThread::run: ogg abruptly stopped.");
            }
         }
      }
      else
      {
         trackFinished = true;
      }

      msleep(1);
   }

   setStopped(2);

   qDebug("MixerStreamThread::run: done");
}

