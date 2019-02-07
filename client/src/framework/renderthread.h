#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QMutex>
#include <QTime>
#include <QTimer>
#include <QThread>

class MainDrawable;

#ifdef WIN32
class RenderThread : public QThread
#else
class RenderThread : public QObject
#endif
{
   Q_OBJECT

public:
   RenderThread(MainDrawable *main);
   ~RenderThread();

   float getFPS() const;

   void setStopped(int s = 1);
   int  stopped() const;
   bool isStopped() const;

#ifndef WIN32
public slots:
   void start();
#endif

   void run();

signals:
   void updateFps(float fps);
   void update();

private:
   MainDrawable    *mMain;
   mutable QMutex  mMutex;
   int             mStopped;
   float           mFPS;
   int             mFrames;
   int             mFpsTime;

   QTime           mElapsed;
   QTimer          mTimer;
};

#endif
