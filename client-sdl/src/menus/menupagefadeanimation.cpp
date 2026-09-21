#include "menupagefadeanimation.h"

#include <math.h>

namespace
{
// MSVC only defines kPi when _USE_MATH_DEFINES is set before every <math.h>/<cmath> include
// site (fragile project-wide), so this is a self-contained local constant instead.
constexpr float kPi = 3.14159265358979323846f;
}  // namespace

MenuPageFadeAnimation::MenuPageFadeAnimation() : MenuPageAnimation(), mStopped(false), mFadeIn(false), mAlpha(0.0f)
{
}

MenuPageFadeAnimation::~MenuPageFadeAnimation()
{
}

void MenuPageFadeAnimation::initialize()
{
}

float MenuPageFadeAnimation::getAlpha() const
{
   return mAlpha;
}

void MenuPageFadeAnimation::setAlpha(float alpha)
{
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
         if (elapsed <= kPi * 0.5f)
            val = sin(elapsed);
         else
            val = 1.0f;
      }
      else
      {
         if (elapsed <= kPi * 0.5f)
            val = cos(elapsed);
         else
            val = 0.0f;
      }

      setAlpha(val);

      if (elapsed > kPi * 0.5f)
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
