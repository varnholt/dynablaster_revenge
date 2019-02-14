// header
#include "levelloadingthread.h"

// Qt
#include <QGLContext>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#endif

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
 : QThread()
 , mPath(path)
 , mLevel(nullptr)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
 , mSurface(nullptr)
#endif
{
   abortAll();
   add(this);

   mLevel = LevelFactory::getFactoryInstance()->getLevelInstance(path);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   setupSurface();
#endif
}


//-----------------------------------------------------------------------------
/*!
*/
LevelLoadingThread::~LevelLoadingThread()
{
   remove(this);
}


#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void LevelLoadingThread::setupSurface()
{
    connect(
       this,
       SIGNAL(createSurfaceSignal()),
       this,
       SLOT(createSurface()),
       Qt::QueuedConnection
    );

    QGLWidget* shared= MainDrawable::getInstance()->getSharedContext();
    const QGLContext* current = shared->context();

    QSurfaceFormat fmt;
    fmt.setMajorVersion(2);
    fmt.setMinorVersion(0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    mGlContext = new QOpenGLContext();
    mGlContext->setFormat(fmt);
    mGlContext->setShareContext(current->contextHandle());
    mGlContext->create();
    mGlContext->moveToThread(this);
}


void LevelLoadingThread::createSurface()
{
   QMutexLocker locker(&mSurfaceMutex);
   mSurface = new QOffscreenSurface();
   mSurface->setFormat(mGlContext->format());
   mSurface->create();

   if (mGlContext->create())
   {
      mSurface->moveToThread(this);
      mSurfaceWaitCondition.wakeOne();
   }
}
#endif


// returns level (never 0 as it was instanced in the constructor)
// if loading was aborted not all level members are valid
Level* LevelLoadingThread::getLevel()
{
   return mLevel;
}


// thread main loop loads the level
void LevelLoadingThread::run()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   QMutexLocker locker(&mSurfaceMutex);
#endif

   bool success= false;

   // wait until all other loader threads have been terminated
   while (threadCount() > 1)
   {
      msleep(100);
   }

   emit started(mPath);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   if (!mSurface)
   {
      emit createSurfaceSignal();
      success = mSurfaceWaitCondition.wait(&mSurfaceMutex, 5000);

      if (success)
      {
         success = mGlContext->makeCurrent(mSurface);
      }
   }
#else
   QGLWidget* shared= MainDrawable::getInstance()->getSharedContext();
   const QGLContext* context = shared->context();

   QGLContext glContext(context->format(), context->device());
   success = glContext.create(context);

   if (success)
   {
      glContext.makeCurrent();
   }
#endif

   if (!success)
   {
      abort();
   }

   mLevel->load();

   if (!mLevel->isAborted())
   {
      mLevel->initialize();
   }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   mGlContext->doneCurrent();
   delete mGlContext;
   mGlContext = nullptr;
   mSurface->moveToThread(QGuiApplication::instance()->thread());
   mSurface->deleteLater();
   mSurface = nullptr;
#else
   glContext.doneCurrent();
#endif

   if (!mLevel->isAborted())
   {
      emit finished(mPath);
   }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   exit();
   moveToThread(QGuiApplication::instance()->thread());
#endif
}


// abort all running loader threads
void LevelLoadingThread::abortAll()
{
   QWriteLocker locker(&sLock);

   foreach (LevelLoadingThread* thread, sQueue)
   {
      Level* level= thread->getLevel();

      if (level)
      {
         level->abort();
      }
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


