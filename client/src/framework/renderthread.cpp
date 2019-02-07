#include "renderthread.h"
#include "maindrawable.h"
#include <QTime>
#include <QMutexLocker>

RenderThread::RenderThread(MainDrawable *main)

#ifdef WIN32
   : QThread()
#else
   : QObject()
#endif
, mMain(main)
, mStopped(0)
, mFPS(0.0)
, mFrames(0)
, mFpsTime(0)
{
}

RenderThread::~RenderThread()
{
#ifdef WIN32
   if (isRunning())
   {
      setStopped(1);
      while (stopped() != 2);
   }
#endif
}

float RenderThread::getFPS() const
{
   QMutexLocker lock(&mMutex);
   return mFPS;
}

void RenderThread::setStopped(int s)
{
   QMutexLocker lock(&mMutex);
   mStopped= s;
}

int RenderThread::stopped() const
{
   QMutexLocker lock(&mMutex);
   return mStopped;
}

bool RenderThread::isStopped() const
{
   QMutexLocker lock(&mMutex);
   return (mStopped != 0);
}




void RenderThread::run()
{
#ifdef WIN32

   QTime curTime;
   curTime.start();

   while (!isStopped())
   {
      mMain->glDraw();

      msleep(1);

//      emit update();

      mFrames++;
      int time= curTime.elapsed();
      int delta= time - mFpsTime;
      if (delta >= 1000)
      {
         mFPS= mFrames * 1000.0 / delta;
         mFpsTime= time;
         mFrames= 0;

         emit updateFps(mFPS);
      }
   }
   setStopped(2);

#else

   mMain->glDraw();

#endif
}

