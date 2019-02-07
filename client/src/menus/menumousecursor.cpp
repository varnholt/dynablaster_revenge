#include "menumousecursor.h"

// base
#include "framework/gldevice.h"
#include "psdlayer.h"

// Qt
#include <QGLContext>
#include <QMouseEvent>

// texture names
#define DEFAULT "default"
#define BUSY    "busy"
#define CLICKED "clicked"

// cmath
#include <math.h>

/*
   TODO: maybe move to /game!
*/

MenuMouseCursor::MenuMouseCursor(RenderDevice *dev, bool visible)
   : QObject(),
     Drawable(dev, visible),
     mDefaultLayer(0),
     mClickedLayer(0),
     mBusyLayer(0),
     mBusyX(0),
     mBusyY(0),
     mX(0),
     mY(0),
     mSizeFactor(1.0f),
     mBusy(false),
     mMousePressed(false),
     mTime(0.0f)
{
   mFilename = "data/cursors/cursor_small.psd";
}

MenuMouseCursor::~MenuMouseCursor()
{
   delete mDefaultLayer;
   mDefaultLayer = 0;

   delete mClickedLayer;
   mClickedLayer = 0;

   delete mBusyLayer;
   mBusyLayer = 0;
}


void MenuMouseCursor::animate(float time)
{
   mTime = time;
}


void MenuMouseCursor::mousePressEvent(int /*x*/, int /*y*/, Qt::MouseButton)
{
   mMousePressed = true;
   mClickTime.restart();
}


void MenuMouseCursor::mouseReleaseEvent(QMouseEvent* /*event*/)
{
   mMousePressed = false;
}


void MenuMouseCursor::mouseMoveEvent(int x, int y)
{
   mX = x;
   mY = y;
}


void MenuMouseCursor::setBusy(bool busy)
{
   mBusy = busy;
}


void MenuMouseCursor::initializeGL()
{
   initializeLayers();
}


void MenuMouseCursor::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items

   PSD::Layer* psdlayer;

   psdlayer= mPsd.getLayer( DEFAULT );
   if (psdlayer)
      mDefaultLayer= new PSDLayer(psdlayer);

   psdlayer= mPsd.getLayer( CLICKED );
   if (psdlayer)
      mClickedLayer= new PSDLayer(psdlayer);

   psdlayer= mPsd.getLayer( BUSY );
   if (psdlayer)
   {
      mBusyX= psdlayer->getLeft();
      mBusyY= psdlayer->getTop();
      psdlayer->setX(0);
      psdlayer->setY(0);
      mBusyLayer= new PSDLayer(psdlayer);
   }
}


void MenuMouseCursor::paintGL()
{
   // init
   initGlParameters();

   paintDefaultCursor();
   paintClickedCursor();
   paintBusyIcon();

   // cleanup
   cleanupGlParameters();
}


void MenuMouseCursor::paintCursor(PSDLayer* layer, float opacity)
{
   layer->render(mX, mY, opacity);
}


void MenuMouseCursor::paintDefaultCursor()
{
   if (!mDefaultLayer)
      return;

   paintCursor(mDefaultLayer);
}


void MenuMouseCursor::paintClickedCursor()
{
   if (!mClickedLayer)
      return;

   float opacity =
         qMax(300 - (int)mClickTime.elapsed(), 0) * 0.00003f
       * mClickedLayer->getOpacity();

   if (opacity > 0.0f)
   {
      paintCursor(mClickedLayer, opacity);
   }
}


void MenuMouseCursor::paintBusyIcon()
{
   if (mBusy)
   {
      glPushMatrix();

      glTranslatef(
         0.5f * mBusyLayer->getWidth(),
         0.5f * mBusyLayer->getHeight(),
         0.0f
      );

      glTranslatef(
         mBusyX,
         mBusyY,
         0.0f
      );

      glScalef(mSizeFactor, mSizeFactor, 1.0f);

      glTranslatef(
         mX,
         mY,
         0.0f
      );

      glRotatef(mTime, 0.0f, 0.0f, 1.0f);

      glTranslatef(
         - 0.5f * mBusyLayer->getWidth() * mSizeFactor,
         - 0.5f * mBusyLayer->getHeight() * mSizeFactor,
         0.0f
      );

      mBusyLayer->render();

      glPopMatrix();
   }
}


void MenuMouseCursor::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


void MenuMouseCursor::initGlParameters()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(
      0.0f,
      1920,
      1080,
      0.0f,
      -1.0f,
      1.0f
   );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


