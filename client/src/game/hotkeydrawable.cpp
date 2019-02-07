// header
#include "hotkeydrawable.h"

// framework
#include "fontpool.h"
#include "gldevice.h"
#include "tools/filestream.h"
#include "psdlayer.h"
#include "framework/timerhandler.h"

// Qt
#include <QGLContext>
#include <QImage>

// cmath
#include <math.h>

// defines
#define FADE_TIME 500.0f


//-----------------------------------------------------------------------------
/*!
   \param dev gl device
*/
HotkeyDrawable::HotkeyDrawable(RenderDevice* dev)
 : QObject(),
   Drawable(dev),
   mFont(0)
{
   setVisible(false);
}


//-----------------------------------------------------------------------------
/*!
*/
HotkeyDrawable::~HotkeyDrawable()
{
   qDeleteAll(mPsdLayers);
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::initializeGL()
{
   // init layers
   initializeLayers();

   // init font
   mFont = FontPool::Instance()->get("default");
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::initializeLayers()
{
   mPsd.load("data/game/help.psd");

   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer = mPsd.getLayer(l);
      mPsdLayers << new PSDLayer(layer);
   }
}



//-----------------------------------------------------------------------------
/*!
   \return alpha value
*/
float HotkeyDrawable::computeAlpha()
{
   float alpha = qMin( (mAnimationTimer.elapsed() / FADE_TIME), 1.0f);

   return alpha;
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::paintGL()
{
   initGlParameters();

   for (int layerIndex = 0; layerIndex < mPsd.getLayerCount(); layerIndex++)
   {
      PSD::Layer* psdLayer = mPsd.getLayer(layerIndex);

      float alpha = computeAlpha();

      if (!mShow)
      {
         alpha = 1.0f - alpha;

         if (alpha == 0.0f)
            Drawable::setVisible(false);
      }

      if (psdLayer->isVisible())
         mPsdLayers[layerIndex]->render(0.0f, 0.0f, alpha);
   }

   cleanupGlParameters();
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::animate(float /*time*/)
{
}


//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void HotkeyDrawable::setVisible(bool visible)
{
   mAnimationTimer.restart();
   mShow = visible;

   if (visible)
   {
      Drawable::setVisible(visible);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if shown
*/
bool HotkeyDrawable::isShown() const
{
   return mShow;
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::showHotkeys()
{
   if (!isVisible())
   {
      setVisible(true);
   }
   else if (isShown())
   {
      setVisible(false);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(
      0.0f,
      mPsd.getWidth(),
      mPsd.getHeight(),
      0.0f,
      1.0f,
      -1.0f
   );

   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);    
   glLoadIdentity();     

   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);    
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


//-----------------------------------------------------------------------------
/*!
*/
void HotkeyDrawable::cleanupGlParameters()
{
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);    
   glDepthMask(GL_TRUE);
}



