#include "rect.h"


Rect::Rect()
   : mMin(0.0f, 0.0f)
   , mMax(0.0f, 0.0f)
   , mValid(false)
{
}


Rect::Rect(float x1, float y1, float x2, float y2)
 : mMin(x1,y1),
   mMax(x2,y2),
   mValid(true)
{
}

Rect::Rect(const Vector2& min, const Vector2& max)
 : mMin(min),
   mMax(max),
   mValid(true)
{
}

float Rect::left() const
{
   return mMin.x;
}

float Rect::right() const
{
   return mMax.x;
}

float Rect::top() const
{
   return mMin.y;
}

float Rect::bottom() const
{
   return mMax.y;
}

float Rect::getWidth() const
{
   return getMax().x - getMin().x;
}


float Rect::getHeight() const
{
   return getMax().y - getMin().y;
}


void Rect::setMin(const Vector2& min)
{
   mMin = min;
   mValid= true;
}


void Rect::setMax(const Vector2& max)
{
   mMax = max;
   mValid= true;
}


const Vector2& Rect::getMin() const
{
   return mMin;
}


const Vector2& Rect::getMax() const
{
   return mMax;
}

bool Rect::valid() const
{
   return mValid;
}
