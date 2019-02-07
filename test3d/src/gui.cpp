#include "gui.h"

// engine
#include "framework/gldevice.h"
#include "render/texturepool.h"
#include "tools/filestream.h"
#include "nodes/scenegraph.h"

// Qt
#include <QMouseEvent>
#include <QTimer>

// cmath
#include <math.h>

Gui::Gui(const QGLFormat& format)
 : QGLWidget(format),
   mDevice(0),
   mScene(0),
   mFrames(0),
   mFrameTime(0.0f),
   mTime(0.0f)
{
   // enable mouse tracking
   setMouseTracking(true);
   setFocusPolicy(Qt::StrongFocus);

   FileStream::addPath( "../client/data/level-castle" );
   FileStream::addPath( "../client/data/shaders" );
}


Gui::~Gui()
{
}


void Gui::initializeGL()
{
   mDevice = new GLDevice();
   mDevice->init();

   mScene= new SceneGraph();
   mScene->load("level.hjb");

}

float Gui::getTime() const
{
    return mTime;
}

void Gui::paintGL()
{
   mTime= mTimer.elapsed() / 1000.0;

   TexturePool::Instance()->update();

   // clear screen
   mDevice->clear();

   float time= 0.0f;
   mScene->render(time);


   /*
//      activeDevice->setViewPort(0,0,1024,1024);
      unsigned int *temp= new unsigned int[1024*1024];
      glReadPixels(0,0, 1024,1024, GL_BGRA, GL_UNSIGNED_BYTE, temp);
      savetga("test.tga", temp, 1024, 1024);
      delete[] temp;
   */


   // figure fps
   mFrames++;
   if (mTime - mFrameTime >= 1.0f)
   {
      float fps= mFrames / (mTime - mFrameTime);
      mFrames= 0;
      mFrameTime= mTime;

      emit updateFps( fps );
   }
}

void Gui::resizeGL(int width, int height)
{
   mDevice->resize(width, height);
}

