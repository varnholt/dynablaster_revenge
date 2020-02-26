#include "gui.h"

// engine
#include "framework/gldevice.h"
#include "render/texturepool.h"
#include "tools/filestream.h"
#include "nodes/scenegraph.h"

// Qt
#include <QMouseEvent>
#include <QTimer>

#include <iostream>

// cmath
#include <math.h>


Gui::Gui(const QGLFormat& format)
 : QGLWidget(format)
{
   // enable mouse tracking
   setMouseTracking(true);
   setFocusPolicy(Qt::StrongFocus);

   FileStream::addPath("../client/data/level-castle");
   FileStream::addPath("../client/data/shaders");
}


void Gui::initializeGL()
{
   mDevice = new GLDevice();
   mDevice->init();

   mScene= new SceneGraph();
   mScene->load("level.hjb");

   std::cout << "scene loaded" << std::endl;
}


float Gui::getTime() const
{
    return mTime;
}


void Gui::paintGL()
{
   mTime = static_cast<float>(mTimer.elapsed() / 1000.0);

   TexturePool::Instance()->update();

   // clear screen
   mDevice->clear();

   auto time = 0.0f;
   mScene->render(time);

   /*
      // activeDevice->setViewPort(0,0,1024,1024);
      unsigned int *temp= new unsigned int[1024*1024];
      glReadPixels(0,0, 1024,1024, GL_BGRA, GL_UNSIGNED_BYTE, temp);
      savetga("test.tga", temp, 1024, 1024);
      delete[] temp;
   */

   // figure fps
   mFrames++;

   if (mTime - mFrameTime >= 1.0f)
   {
      auto fps = mFrames / (mTime - mFrameTime);
      mFrames = 0;
      mFrameTime = mTime;

      emit updateFps( fps );
   }
}


void Gui::resizeGL(int width, int height)
{
   mDevice->resize(width, height);
}

