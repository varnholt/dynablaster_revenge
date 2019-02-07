#pragma once

// Qt
#include <QQueue>
#include <QReadWriteLock>
#include <QThread>

// tools
#include "tools/array.h"

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

private:
   QString     mPath;
   Level*      mLevel;

   static QReadWriteLock sLock;
   static QQueue<LevelLoadingThread*> sQueue;
};
