// header
#include "levelloadingthread.h"

// Qt
#include <QGLContext>

// framework
#include "maindrawable.h"

#include "levels/levelfactory.h"
#include "levels/level.h"

// cmath
#include "math.h"

// static
QReadWriteLock LevelLoadingThread::sLock;
QQueue<LevelLoadingThread*> LevelLoadingThread::sQueue;


//-----------------------------------------------------------------------------
/*!
   \param path level to load
*/
LevelLoadingThread::LevelLoadingThread(const QString& path)
 : QThread(),
   mPath(path),
   mLevel(0)
{
   abortAll();
   add(this);

   mLevel = LevelFactory::getFactoryInstance()->getLevelInstance(path);
}


//-----------------------------------------------------------------------------
/*!
*/
LevelLoadingThread::~LevelLoadingThread()
{
   remove(this);
}


// returns level (never 0 as it was instanced in the constructor)
// if loading was aborted not all level members are valid
Level* LevelLoadingThread::getLevel()
{
   return mLevel;
}


// thread main loop loads the level
void LevelLoadingThread::run()
{
   // wait until all other loader threads have been terminated
   while (threadCount() > 1)
   {
      msleep(100);
   }

   emit started(mPath);

/*
   QGLWidget* shared= MainDrawable::getInstance()->getSharedContext();
   shared->makeCurrent();
*/

   QGLWidget* shared= MainDrawable::getInstance()->getSharedContext();
   QGLContext glContext(shared->context()->format(), shared->context()->device());
   if (! glContext.create(shared->context())) {
       qDebug("Fuck off");
       abort();
   }

   glContext.makeCurrent();

   mLevel->load();

   if (!mLevel->isAborted())
      mLevel->initialize();

   glContext.doneCurrent();

   if (!mLevel->isAborted())
      emit finished(mPath);
}




// abort all running loader threads
void LevelLoadingThread::abortAll()
{
   QWriteLocker locker(&sLock);

   foreach (LevelLoadingThread* thread, sQueue)
   {
      Level* level= thread->getLevel();
      if (level)
         level->abort();
   }
}


// add loader thread
void LevelLoadingThread::add(LevelLoadingThread * thread)
{
   QWriteLocker locker(&sLock);
   sQueue.push_back(thread);
}


// remove loader thread
void LevelLoadingThread::remove(LevelLoadingThread * thread)
{
   QWriteLocker locker(&sLock);
   sQueue.removeOne(thread);
}


// get number of running threads
int LevelLoadingThread::threadCount()
{
   QReadLocker locker(&sLock);
   return sQueue.size();
}


