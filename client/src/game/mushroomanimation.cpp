// header
#include "mushroomanimation.h"

// framework
#include "globaltime.h"

#define TIME_FADE   1.0f

MushroomAnimation::MushroomAnimation()
 : mTime(0.0f),
   mStartTime(0.0f),
   mActive(false),
   mIntensity(0.0f),
   mAborted(false)
{
}


void MushroomAnimation::start()
{
   if (!isActive())
   {
      setIntensity(0.0f);
      setActive(true);
   }

   float time = GlobalTime::Instance()->getTime();
   setTime(time);
   setStartTime(time);
   setAborted(false);
}


void MushroomAnimation::abort()
{
   mAborted = true;
}


bool MushroomAnimation::isAborted() const
{
   return mAborted;
}


void MushroomAnimation::setAborted(bool aborted)
{
   mAborted = aborted;
}


float MushroomAnimation::getNormalizedTime() const
{
   return getTime() - getStartTime();
}


void MushroomAnimation::update()
{
   float time = GlobalTime::Instance()->getTime();
   float dt = time - getTime();

   setTime(time);

   float normalizedTime = getNormalizedTime();
   float intensity = getIntensity();

   if (isAborted())
   {
      intensity -= dt;
   }
   else
   {
      if (normalizedTime < TIME_FADE)
      {
         intensity += dt;
      }
      else if (normalizedTime > TIME_FADE)
      {
         intensity = 1.0f;
      }
   }

   if (intensity <= 0.0)
   {
      setActive(false);
   }
   else if (intensity > 1.0f)
   {
      intensity = 1.0f;
   }

   setIntensity(intensity);

   /*
            _____________________
           /|                   |\
          / |                   | \
         /  |                   |  \
     +---|--|-------------------|--|------+
         |  |                   |  |
         fade                   fade
         |                         |

         <-    d u r a t i o n    ->

   */
}


void MushroomAnimation::setTime(float time)
{
   mTime = time;
}


float MushroomAnimation::getTime() const
{
   return mTime;
}


void MushroomAnimation::setStartTime(float time)
{
   mStartTime = time;
}


float MushroomAnimation::getStartTime() const
{
   return mStartTime;
}


void MushroomAnimation::setActive(bool active)
{
   mActive = active;
}


bool MushroomAnimation::isActive() const
{
   return mActive;
}


void MushroomAnimation::setIntensity(float intensity)
{
   mIntensity = intensity;
}


float MushroomAnimation::getIntensity() const
{
   return mIntensity;
}

