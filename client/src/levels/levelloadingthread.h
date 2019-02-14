#pragma once

// Qt
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QMutex>
#include <QOpenGLContext>
#include <QWaitCondition>
#endif

#include <QQueue>
#include <QReadWriteLock>
#include <QThread>

// tools
#include "tools/array.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
class QOffscreenSurface;
#endif

class MainDrawable;
class Material;
class SceneGraph;
class Node;
class SharedGLWidget;
class Level;

class LevelLoadingThread : public QThread
{
   Q_OBJECT

public:

   LevelLoadingThread(const QString& path);
   ~LevelLoadingThread();

   void run();

   Level* getLevel();

   static void abortAll();
   static void add(LevelLoadingThread*);
   static void remove(LevelLoadingThread*);
   static int threadCount();

signals:

   void started(const QString& path);
   void finished(const QString& path);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   void createSurfaceSignal();
#endif

private slots:

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   void createSurface();
   void setupSurface();
#endif

private:
   QString mPath;
   Level* mLevel;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
   QOpenGLContext* mGlContext;
   QOffscreenSurface* mSurface;
   QMutex mSurfaceMutex;
   QWaitCondition mSurfaceWaitCondition;
#endif

   static QReadWriteLock sLock;
   static QQueue<LevelLoadingThread*> sQueue;
};
