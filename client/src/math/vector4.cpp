// vector class implementation

#include "vector4.h"
#include "tools/stream.h"
#include <math.h>

// default constructor: uninitialized(!) vector
Vector4::Vector4()
{
}


Vector4::Vector4(float x, float y, float z, float w)
   : x(x)
   , y(y)
   , z(z)
   , w(w)
{
}

Vector4::Vector4(Stream& stream)
{
   load(&stream);
}

void Vector4::set(float px, float py, float pz, float pw)
{
   x= px;
   y= py;
   z= pz;
   w= pw;
}


Vector4 Vector4::operator * (const float f) const
{
   Vector4 n;
   n.x= x*f;
   n.y= y*f;
   n.z= z*f;
   n.w= w*f;
   return n;
}

void Vector4::operator *=(const float f)
{
   x*=f;
   y*=f;
   z*=f;
   w*=f;
}

float Vector4::operator * (const Vector4& v) const
{
   return x*v.x + y*v.y + z*v.z + w*v.w;
}

Vector4 Vector4::operator + (const Vector4& v) const
{
   Vector4 n;
   n.x= x + v.x;
   n.y= y + v.y;
   n.z= z + v.z;
   n.w= w + v.w;
   return n;
}


void Vector4::operator += (const Vector4& v)
{
   x+=v.x;
   y+=v.y;
   z+=v.z;
   w+=v.w;
}


Vector4 Vector4::operator - (const Vector4& v) const
{
   Vector4 n;
   n.x= x - v.x;
   n.y= y - v.y;
   n.z= z - v.z;
   n.w= w - v.w;
   return n;
}

Vector4 Vector4::operator - () const
{
   Vector4 n;
   n.x= -x;
   n.y= -y;
   n.z= -z;
   n.w= -w;
   return n;
}


void Vector4::operator -= (const Vector4& v)
{
   x-=v.x;
   y-=v.y;
   z-=v.z;
   w-=v.w;
}


void Vector4::operator << (Stream& stream)
{
   load(&stream);
}

void Vector4::operator >> (Stream& stream)
{
   write(&stream);
}

void Vector4::load(Stream *stream)
{
   x= stream->getFloat();
   y= stream->getFloat();
   z= stream->getFloat();
   w= stream->getFloat();
}


void Vector4::write(Stream *stream)
{
   stream->writeFloat(x);
   stream->writeFloat(y);
   stream->writeFloat(z);
   stream->writeFloat(w);
}


float* Vector4::data() const
{
   return (float*)&x;
}


unsigned int Vector4::rgba() const
{
   int r= (int) (x * 255.0);
   int g= (int) (y * 255.0);
   int b= (int) (z * 255.0);
   int a= (int) (w * 255.0);
   if (r<0) r=0; if (r>255) r= 255;
   if (g<0) g=0; if (g>255) g= 255;
   if (b<0) b=0; if (b>255) b= 255;
   if (a<0) a=0; if (a>255) a= 255;
   return (a<<24)|(b<<16)|(g<<8)|r;
}

Vector4 Vector4::linear(const Vector4& v, float t) const
{
   return *this + (v - *this)*t;
}

Vector Vector4::xyz() const
{
   return Vector(x,y,z);
}
