#include "detonation.h"

Detonation::Detonation(int cx, int cy, int left, int right, int top, int bottom)
: mCenterX(cx)
, mCenterY(cy)
, mLeft(left)
, mRight(right)
, mTop(top)
, mBottom(bottom)
, mStartTime(0.0f)
{
}

int Detonation::getX() const
{
   return mCenterX;
}

int Detonation::getY() const
{
   return mCenterY;
}

int Detonation::getUp() const
{
   return mTop;
}

int Detonation::getDown() const
{
   return mBottom;
}

int Detonation::getLeft() const
{
   return mLeft;
}

int Detonation::getRight() const
{
   return mRight; 
}

void Detonation::setStartTime(float time)
{
   mStartTime= time;
}

float Detonation::elapsed(float time) const
{
   return time - mStartTime;
}
