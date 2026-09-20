// vector class implementation

#include "vector2.h"
#include "tools/stream.h"
#include <math.h>

// default constructor: uninitialized(!) vector
Vector2::Vector2()
{
}


Vector2::Vector2(float x, float y)
   : x(x)
   , y(y)
{
}

Vector2::Vector2(Stream& stream)
{
   load(&stream);
}

void Vector2::set(float px, float py)
{
   x= px;
   y= py;
}


Vector2 Vector2::operator * (const float f) const
{
   Vector2 n;
   n.x= x*f;
   n.y= y*f;
   return n;
}

void Vector2::operator *=(const float f)
{
   x*=f;
   y*=f;
}

float Vector2::operator * (const Vector2& v) const
{
   return x*v.x + y*v.y;
}

Vector2 Vector2::operator + (const Vector2& v) const
{
   Vector2 n;
   n.x= x + v.x;
   n.y= y + v.y;
   return n;
}


void Vector2::operator += (const Vector2& v)
{
   x+=v.x;
   y+=v.y;
}


Vector2 Vector2::operator - (const Vector2& v) const
{
   Vector2 n;
   n.x= x - v.x;
   n.y= y - v.y;
   return n;
}

Vector2 Vector2::operator - () const
{
   Vector2 n;
   n.x= -x;
   n.y= -y;
   return n;
}


void Vector2::operator -= (const Vector2& v)
{
   x-=v.x;
   y-=v.y;
}


void Vector2::operator << (Stream& stream)
{
   load(&stream);
}

void Vector2::operator >> (Stream& stream)
{
   write(&stream);
}

void Vector2::load(Stream *stream)
{
   x= stream->getFloat();
   y= stream->getFloat();
}


void Vector2::write(Stream *stream)
{
   stream->writeFloat(x);
   stream->writeFloat(y);
}


float* Vector2::data() const
{
   return (float*)&x;
}


Vector2 Vector2::linear(const Vector2& v, float t) const
{
   return *this + (v - *this)*t;
}
