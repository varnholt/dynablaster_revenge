//
// C++ Implementation: playerrotation
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

// header
#include "playerrotation.h"

// math
#include <math.h>

// init static variables
float PlayerRotation::mAngleIncrement = 0.2f; // 0.075f;
QVector2D PlayerRotation::mDown = QVector2D(0.0f, -1.0f);


//-----------------------------------------------------------------------------
/*!
  constructor
*/
PlayerRotation::PlayerRotation()
   : mAngle(0.0f)
   , mPreviousAngle(0.0f)
   , mTargetAngle(0.0f)
   , mDelta(0.0f)
{
}


//-----------------------------------------------------------------------------
/*!
   \param vector target direction
*/
void PlayerRotation::setAngleIncrement(float angle)
{
   mAngleIncrement = angle;
}


//-----------------------------------------------------------------------------
/*!
   \param vector target direction
*/
void PlayerRotation::setTargetVector(const QVector2D& target)
{
   mTargetVector = target;
   /*

      the player may only move in 4 directions:
      - right
      - down
      - left
      - up

      => the player rotation is interpolated between these values.


                        |
                        | 0,1
                        |
                    _.-"+"-._
                  .'    |    `.
                 /      |      \
                |       |       |
         -------+-------+-------+-------
         -1,0   |       |       |   1,0
                 \      |      /
                  `._   |   _.'
                     `-.+.-'
                        |
                        | 0,-1
                        |

   */

   // initialize target angle from direction vector

   // map the atan2 circle to 0..2*PI
   mTargetAngle = atan2(-target.y(), -target.x()) + M_PI;

//      qDebug(
//         "PlayerRotation::SetTargetVector: target angle is %f [deg]",
//         mTargetAngle / M_PI * 180.0f
//      );
}


//-----------------------------------------------------------------------------
/*!
   \return target vector
*/
const QVector2D& PlayerRotation::getTargetVector()
{
   return mTargetVector;
}


//-----------------------------------------------------------------------------
/*!
*/
void PlayerRotation::updateAngle()
{
   /*
         |--------+<<<<<<<<<<<<<<<<<<<<<<+------------|
         0        |                      |           360
                  |                      |
                  to                    from

         aaaaaaaaa|bbbbbbbbbbbbbbbbbbbbbb|aaaaaaaaaaaaa


         case 1: from > to

            a = 360 - from + to
            b = from - to

         |--------+>>>>>>>>>>>>>>>>>>>>>>+------------|
         0        |                      |           360
                  |                      |
                 from                    to

         aaaaaaaaa|bbbbbbbbbbbbbbbbbbbbbb|aaaaaaaaaaaaa

         case 2:  to > from

            a = from + 360 - to
            b = to - from

         increase = (a > b)
   */

   float a = 0.0f;
   float b = 0.0f;

   float from = mAngle;
   float to = mTargetAngle;

   bool increase = false;

   if (from >= to)
   {
      a = from - to;
      b = to + (2.0f * M_PI) - from;
   }
   else
   {
      a = from + (2.0f * M_PI) - to;
      b = to - from;
   }

   increase = (a > b);

   mPreviousAngle = mAngle;

   if (increase)
   {
      float delta = mAngleIncrement;

      if (delta > b)
         delta = b;

      mDelta = delta;
      mAngle += delta;

      if (mAngle >= (float)M_PI * 2.0f)
         mAngle -= (float)M_PI * 2.0f;
   }
   else
   {
      float delta = mAngleIncrement;

      if (delta > a)
         delta = a;

      mDelta = -delta;
      mAngle -= delta;

      if (mAngle < 0.0f)
         mAngle += (float)M_PI * 2.0f;

      if (mAngle < mTargetAngle && mPreviousAngle >= mTargetAngle)
         mAngle = mTargetAngle;
   }

//   qDebug(
//      "PlayerRotation::UpdateAngle: "
//      "from: %f [deg], to: %f [deg] new angle to: %f [deg]",
//      from / M_PI * 180.0f,
//      to / M_PI * 180.0f,
//      mAngle / M_PI * 180.0f
//   );
}


//-----------------------------------------------------------------------------
/*!
   \param vector target direction
*/
float PlayerRotation::getAngle()
{
   return mAngle;
}



//-----------------------------------------------------------------------------
/*!
   \param vector target direction
*/
float PlayerRotation::getPreviousAngle()
{
   return mPreviousAngle;
}


//-----------------------------------------------------------------------------
/*!
   \param vector target direction
*/
float PlayerRotation::getAngleDelta() const
{
   return mDelta;
}


//-----------------------------------------------------------------------------
/*!
*/
void PlayerRotation::reset()
{
   setTargetVector(mDown);
}



