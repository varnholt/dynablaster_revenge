#include "mapitemanimation.h"


// cmath
#include <math.h>

// defines
#define TICK_UPPER_LIMIT 0.1
#define TICK_LOWER_LIMIT 0.02


MapItemAnimation::MapItemAnimation(
   Constants::Direction dir,
   float speed
)
 : QObject(),
   mDirection(dir),
   mSpeed(speed),
   mX(0.0f),
   mY(0.0f),
   mZ(0.0f),
   mZPrev(0.0f),
   mNominalX(-1),
   mNominalY(-1),
   mTime(0.0f),
   mTick(false),
   mFactor(1.0f)
{
}


void MapItemAnimation::animate(float dt)
{
   mZPrev = mZ;

   // let the bomb bounce
   mTime += dt * 0.1f;
   mZ = fabs(sin(0.7f * pow(mTime, 1.5f)) / (1.0f + mTime)) * 1.5f;
   mZ *= mFactor;

   // check for floor tick sound
   if (
         mZ     < TICK_UPPER_LIMIT
      && mZPrev > mZ
   )
   {
      if (!mTick)
      {
         /*
         qDebug(
            "MapItemAnimation::animate: bounce, bounce, bounce: %f => %f",
            mZPrev,
            mZ
         );
         */

         emit bounce();
         mTick = true;
      }
   }
   else
   {
      if (mZ > TICK_LOWER_LIMIT)
         mTick = false;
   }
}


void MapItemAnimation::reset()
{
   mTime = 0.0f;
   mFactor = 1.0f;
}


