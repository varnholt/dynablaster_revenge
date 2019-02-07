// header
#include "loadingdrawable.h"

// framework
#include "globaltime.h"
#include "tools/filestream.h"
#include "psdlayer.h"
#include "renderdevice.h"

// Qt
#include <QGLContext>
#include <QImage>

// cmath
#include <math.h>

#define OFFSET_X -15
#define OFFSET_Y -15




LoadingDrawable::LoadingDrawable(RenderDevice* dev, bool visible)
 : QObject(),
   Drawable(dev, visible),
   mLoading(false)
{
}


LoadingDrawable::~LoadingDrawable()
{
   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();
}


void LoadingDrawable::setVisible(bool /*visible*/)
{
   // we generously ignore the visibility
}


bool LoadingDrawable::isVisible() const
{
   return mLoading;
}


void LoadingDrawable::paintGL()
{
   initGlParameters();

   float t = GlobalTime::Instance()->getTime();
   float alpha = fmod(t, 1.0f);

   for (int layerIndex = 0; layerIndex < mPsd.getLayerCount(); layerIndex++)
   {
      mPsdLayers[layerIndex]->render(
         1920 - mPsd.getWidth() + OFFSET_X,
         1080 - mPsd.getHeight() + OFFSET_Y,
         alpha
      );
   }

   cleanupGlParameters();
}


void LoadingDrawable::setLoading(bool enabled)
{
   mLoading = enabled;
}


void LoadingDrawable::loadingStarted()
{
   setLoading(true);
}


void LoadingDrawable::loadingStopped()
{
   setLoading(false);
}


void LoadingDrawable::initializeGL()
{
   initializeLayers();
}


void LoadingDrawable::initializeLayers()
{
   mPsd.load("data/game/loading.psd");

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer = mPsd.getLayer(l);
      mPsdLayers << new PSDLayer(layer);
   }
}



void LoadingDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(
      0.0f,
      1920,
      1080,
      0.0f,
      1.0f,
      -1.0f
   );

   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


void LoadingDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

