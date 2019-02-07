// vector class implementation

#include "stdafx.h"
#include "vector.h"
#include "tools/stream.h"
#include <math.h>

// default constructor: uninitialized(!) vector
Vector::Vector()
{
}


Vector::Vector(float x, float y, float z)
: x(x)
, y(y)
, z(z)
{
}

Vector::Vector(Stream& stream)
{
   load(&stream);
}

void Vector::set(float px, float py, float pz)
{
   x= px;
   y= py;
   z= pz;
}


Vector Vector::operator * (const float f) const
{
   Vector n;
   n.x= x*f;
   n.y= y*f;
   n.z= z*f;
   return n;
}


float Vector::operator * (const Vector& v) const
{
   return x*v.x + y*v.y + z*v.z;
}

void Vector::operator *= (const float f)
{
   x*= f;
   y*= f;
   z*= f;
}

void Vector::operator /= (const float f)
{
   float t= 1.0f / f;
   x*= t;
   y*= t;
   z*= t;
}

Vector Vector::operator + (const Vector& v) const
{
   Vector n;
   n.x= x + v.x;
   n.y= y + v.y;
   n.z= z + v.z;
   return n;
}


void Vector::operator += (const Vector& v)
{
   x+=v.x;
   y+=v.y;
   z+=v.z;
}


Vector Vector::operator - (const Vector& v) const
{
   Vector n;
   n.x= x - v.x;
   n.y= y - v.y;
   n.z= z - v.z;
   return n;
}

Vector Vector::operator - () const
{
   Vector n;
   n.x= -x;
   n.y= -y;
   n.z= -z;
   return n;
}


void Vector::operator -= (const Vector& v)
{
   x-=v.x;
   y-=v.y;
   z-=v.z;
}


Vector Vector::operator % (const Vector& v) const
{
   Vector n;
   n.x = y*v.z - z*v.y;
   n.y = z*v.x - x*v.z;
   n.z = x*v.y - y*v.x;
   return n;
}


bool Vector::operator == (const Vector& v1) const
{
   float x1= x - v1.x;
   float y1= y - v1.y;
   float z1= z - v1.z;
   float dist= x1*x1 + y1*y1 + z1*z1;
   if (dist<0.0001f) return true; else return false;
}


bool Vector::operator != (const Vector& v1) const
{
	float x1= x - v1.x;
	float y1= y - v1.y;
	float z1= z - v1.z;
	float dist= x1*x1 + y1*y1 + z1*z1; // no sqrt! compare with EPS^2 instead
	if (dist<EPS) return false; else return true;
}


void Vector::operator << (Stream& stream)
{
   load(&stream);
}

void Vector::operator >> (Stream& stream)
{
   write(&stream);
}

void Vector::normalize(float length)
{
   float t= length / sqrt(x*x + y*y + z*z);
   x*=t;
   y*=t;
   z*=t;
}

Vector Vector::normalize(const Vector& v)
{
   Vector n;
   float t= 1.0f / sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
   n.x= v.x * t;
   n.y= v.y * t;
   n.z= v.z * t;
   return n;
}


float Vector::length() const
{
	return sqrt(x*x + y*y + z*z);
}


void Vector::load(Stream *stream)
{
  x= stream->getFloat();
  y= stream->getFloat();
  z= stream->getFloat();
}


void Vector::write(Stream *stream)
{
  stream->writeFloat(x);
  stream->writeFloat(y);
  stream->writeFloat(z);
}


float* Vector::data() const
{
	return (float*)&x;
}

int Vector::maxIndex() const
{
   if (x >= y && x >= z) 
      return 0;
   if (y >= z)
      return 1;
   else
      return 2;
}

void Vector::maximum(const Vector& v)
{
   if (v.x > x) x= v.x;
   if (v.y > y) y= v.y;
   if (v.z > z) z= v.z;
}

void Vector::minimum(const Vector& v)
{
   if (v.x < x) x= v.x;
   if (v.y < y) y= v.y;
   if (v.z < z) z= v.z;
}

Vector Vector::abs() const
{
   Vector n;
   n.x = fabs(x);
   n.y = fabs(y);
   n.z = fabs(z);
   return n;
}

int Vector::absMaxIndex() const
{
   float ax= fabs(x);
   float ay= fabs(y);
   float az= fabs(z);
   if (ax >= ay && ax >= az)
      return 0;
   else if (ay >= az && ay >= ax)
      return 1;
   else
      return 2;
}
