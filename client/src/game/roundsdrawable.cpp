// header
#include "roundsdrawable.h"

// engine
#include "gldevice.h"
#include "renderdevice.h"
#include "psdlayer.h"

// filter
#include "postproduction/motionblurfilter.h"

// game
#include "bombermanclient.h"
#include "constants.h"

// math
#include <math.h>

#define ANIMATION_DURATION 7.0f * 62.5f
#define FADE_OUT_DURATION 1.0f * 62.5f
#define BLUR_FACTOR 0.15f

RoundsDrawable::RoundsDrawable(RenderDevice* dev)
 : QObject(),
   Drawable(dev),
   mLayerRound(0),
   mLayerRoundFinal(0),
   mLayerRound1(0),
   mLayerRound2(0),
   mLayerRound3(0),
   mLayerRound4(0),
   mLayerRound5(0),
   mTime(0.0f),
   mStartTime(0.0f),
   mMotionBlurFilter(0),
   mMaxLayerWidth(0.0f)
{
   mFilename = "data/game/rounds.psd";

   mMotionBlurFilter = new MotionBlurFilter();
}


RoundsDrawable::~RoundsDrawable()
{
   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();

   delete mMotionBlurFilter;
}


void RoundsDrawable::initializeGL()
{
   initializeLayers();

   mMotionBlurFilter->init();
}


void RoundsDrawable::paintGL()
{
   initGlParameters();

   mMotionBlurFilter->setIntensity(1.0f);

   float x = 0.0f;
   float y = 0.0f;
   float alpha = 1.0f;

   float scale = 0.0f;
   Vector dir;

   float speed = 0.0f;

   float t = getRelativeTime() * 0.016f; // / 62.5

   float t1 = 2.0f;
   float t2 = 4.0f;
   float t3 = 6.0f;

   if (t < t1)
      speed = 0.5f * (cos(t * (1.0f / t1) * M_PI) + 1.0f); // 1..0
   else if (t < t2)
      speed = 0.0f;
   else if (t < t3)
      speed = -(1.0f - (0.5f * (cos((t2 - t) * (1.0f / (t3 - t2)) * M_PI) + 1.0f))); // 0..-1
   else
      speed = -1.0f;

   speed *= fabs(speed);
   x = 0.75f * -speed * mPsd.getWidth();

   /*

      envelope: (envelope... yeah, right)

      y
      ^
      |
      |^^^^^^^^~.                     .~^^^^^^^^^
      |          \                   /
      |           \                 /
      |            ^._____________.^
      |----------------------------------------> t
      |      |      |      |      |      |     |
      |      1      2      3      4      5     6
      |             t1            t2           t3
   */


   GameInformation* info = BombermanClient::getInstance()->getCurrentGameInformation();

   if (info && info->getRoundCount() > 1)
   {
      if (info->getCurrentRound() == info->getRoundCount()-1)
      {
         scale = 1.0f / (mLayerRoundFinal->getWidth()/mMaxLayerWidth);
         dir = Vector(speed * scale * BLUR_FACTOR, 0.0f, 0.0f);
         mMotionBlurFilter->setMotionDir(dir);
         mMotionBlurFilter->setTextureParam(mLayerRoundFinal->getTexture());
         mMotionBlurFilter->process(0,0,0);
         mLayerRoundFinal->render(x, y, alpha);
      }
      else
      {
         PSDLayer** crLayer = 0;

         switch (info->getCurrentRound())
         {
            case 0:
               crLayer = &mLayerRound1;
               break;
            case 1:
               crLayer = &mLayerRound2;
               break;
            case 2:
               crLayer = &mLayerRound3;
               break;
            case 3:
               crLayer = &mLayerRound4;
               break;
            case 4:
               crLayer = &mLayerRound5;
               break;
            default:
               break;
         }

         scale = 1.0f / (mLayerRound->getWidth()/mMaxLayerWidth);
         dir = Vector(speed * scale * BLUR_FACTOR, 0.0f, 0.0f);
         mMotionBlurFilter->setMotionDir(dir);
         mMotionBlurFilter->setTextureParam(mLayerRound->getTexture());
         mMotionBlurFilter->process(0,0,0);
         mLayerRound->render(x, y, alpha);

         if (!crLayer)
            crLayer = &mLayerRound1;

         if (*crLayer)
         {
            scale = 1.0f / ((*crLayer)->getWidth()/mMaxLayerWidth);
            dir = Vector(speed * scale * BLUR_FACTOR, 0.0f, 0.0f);
            mMotionBlurFilter->setMotionDir(dir);
            mMotionBlurFilter->setTextureParam((*crLayer)->getTexture());
            mMotionBlurFilter->process(0,0,0);
            (*crLayer)->render(x, y, alpha);
         }
      }
   }

   activeDevice->setShader(0);

   cleanupGlParameters();
}


void RoundsDrawable::animate(float time)
{
   mTime = time;

   if (mInitializeTime)
   {
      mStartTime = time;
      mInitializeTime = false;
   }

   if (getRelativeTime() > ANIMATION_DURATION + FADE_OUT_DURATION)
      setVisible(false);

}


void RoundsDrawable::showGame()
{
   setVisible(true);
   mInitializeTime = true;
}


void RoundsDrawable::initGlParameters()
{
   glMatrixMode(GL_PROJECTION);
   Matrix ortho= Matrix::ortho(
      0.0f,
      mPsd.getWidth(),
      mPsd.getHeight(),
      0.0f,
      -1.0f,
      1.0f
   );
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


void RoundsDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


float RoundsDrawable::getRelativeTime() const
{
   return mTime - mStartTime;
}


void RoundsDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* psd  = mPsd.getLayer(l);
      PSDLayer *layer= new PSDLayer(psd);

      mMaxLayerWidth = qMax(mMaxLayerWidth, (float)layer->getWidth());

      if (psd->getName() == QString("round_label"))
         mLayerRound = layer;
      else if (psd->getName() == QString("finalround_label"))
         mLayerRoundFinal = layer;
      else if (psd->getName() == QString("r1"))
         mLayerRound1 = layer;
      else if (psd->getName() == QString("r2"))
         mLayerRound2 = layer;
      else if (psd->getName() == QString("r3"))
         mLayerRound3 = layer;
      else if (psd->getName() == QString("r4"))
         mLayerRound4 = layer;
      else if (psd->getName() == QString("r5"))
         mLayerRound5 = layer;

      mPsdLayers << layer;
   }
}

