#include "menupagefadeanimation.h"

#include <QGLShaderProgram>

#include <math.h>

MenuPageFadeAnimation::MenuPageFadeAnimation()
   : MenuPageAnimation(),
     mAlphaLocation(0),
     mStopped(false),
     mFadeIn(false),
     mAlpha(0.0f)
{
}


MenuPageFadeAnimation::~MenuPageFadeAnimation()
{
}


void MenuPageFadeAnimation::initialize()
{
   // mAlphaLocation = mProgram->uniformLocation("alpha");
}


float MenuPageFadeAnimation::getAlpha() const
{
   return mAlpha;
}


int MenuPageFadeAnimation::getAlphaLocation()
{
   return mAlphaLocation;
}


void MenuPageFadeAnimation::setAlpha(float alpha)
{
   // mProgram->setUniformValue(mAlphaLocation, alpha);
   mAlpha = alpha;
}


void MenuPageFadeAnimation::setFadeIn(bool fadeIn)
{
   mFadeIn = fadeIn;
}


void MenuPageFadeAnimation::start()
{
   if (mFadeIn)
      mAlpha = 0.0f;
   else
      mAlpha = 1.0f;

   mStopped = false;
   mElapsed.restart();
}


void MenuPageFadeAnimation::animate()
{
   if (!mStopped)
   {
      float elapsed = mElapsed.elapsed() * 0.002f;

      float val = 0.0f;

      if (mFadeIn)
      {
         if (elapsed <= M_PI * 0.5f)
            val = sin(elapsed);
         else
            val = 1.0f;
      }
      else
      {
         if (elapsed <= M_PI * 0.5f)
            val = cos(elapsed);
         else
            val = 0.0f;
      }

      setAlpha(val);

      // qDebug("fade: %d, val: %f", mFadeIn, val);

      if (elapsed > M_PI * 0.5f)
      {
         mStopped = true;
         emit stopped();
      }
   }
}


bool MenuPageFadeAnimation::isStopped() const
{
   return mStopped;
}


void MenuPageFadeAnimation::setStopped(bool value)
{
   mStopped = value;
}


