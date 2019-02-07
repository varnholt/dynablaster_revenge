// header
#include "countdowndrawable.h"

// engine
#include "gldevice.h"
#include "renderdevice.h"
#include "psdlayer.h"

// math
#include "math.h"

#define ANIMATION_LENGHT 4000
#define FADE_FACTOR 0.009f

CountdownDrawable::CountdownDrawable(RenderDevice* dev)
   : QObject(),
     Drawable(dev),
     mTimeLeft(0),
     mAnimationStartTime(-1.0f),
     mAnimationActive(false),
     mDeltaTime(0.0f),
     mTime(0.0f),
     mDeltaTimeInitialized(false)
{
   mFilename = "data/menus/countdown.psd";
}


CountdownDrawable::~CountdownDrawable()
{
   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();
}


void CountdownDrawable::initializeGL()
{
   initializeLayers();
}


void CountdownDrawable::paintGL()
{
   initGlParameters();
   drawCountdown();
   cleanupGlParameters();
}


void CountdownDrawable::animate(float time)
{
   mDeltaTime = time - mTime;
   mTime = time;

   if (!mDeltaTimeInitialized)
   {
      mDeltaTime = 0.0f;
      mDeltaTimeInitialized = true;
   }

   // decrease alpha
   bool done = true;
   float val = 0.0f;
   for (int i = 0; i < mLayerAlphas.size(); i++)
   {
      val = mLayerAlphas[i];

      if (val > 0.0f)
      {
         mLayerAlphas[i] = val - (FADE_FACTOR * mDeltaTime);
         done = false;
      }
   }

   // check if we' done
   if (done)
   {
      setVisible(false);
      mDeltaTimeInitialized = false;
   }
}



void CountdownDrawable::drawCountdown()
{
   for (int layerIndex = 0; layerIndex < mPsdLayers.size(); layerIndex++)
   {
      if (mLayerAlphas[layerIndex] > 0.0)
      {
         PSDLayer* layer= mPsdLayers[layerIndex];
         layer->render(0,0, mLayerAlphas[layerIndex]);
      }
   }
}


void CountdownDrawable::countdown(int left)
{
   qDebug("CountdownDrawable::countdown: left: %d", left);

   setVisible(true);
   mTimeLeft = left;
   mLayerAlphas[mLayerAlphas.size() - 1 - left] = 1.0f;
}


void CountdownDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(
      0.0f,
      mPsd.getWidth(), //(GLint)mDevice->getWidth(),
      mPsd.getHeight(),//(GLint)mDevice->getHeight(),
      0.0f,
      -1.0f,
      1.0f
   );
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf( ortho );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


void CountdownDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


void CountdownDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSDLayer *layer= new PSDLayer( mPsd.getLayer(l) );

      mPsdLayers << layer;
      mLayerAlphas << 0.0f;
   }
}

