// vector class implementation

#include "vector4.h"
#include <math.h>
#include <cstdint>
#include "tools/stream.h"

// default constructor: uninitialized(!) vector
Vector4::Vector4()
{
}

Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{
}

Vector4::Vector4(Stream& stream)
{
   load(&stream);
}

void Vector4::set(float px, float py, float pz, float pw)
{
   x = px;
   y = py;
   z = pz;
   w = pw;
}

Vector4 Vector4::operator*(const float scalar) const
{
   Vector4 result;
   result.x = x * scalar;
   result.y = y * scalar;
   result.z = z * scalar;
   result.w = w * scalar;
   return result;
}

void Vector4::operator*=(const float scalar)
{
   x *= scalar;
   y *= scalar;
   z *= scalar;
   w *= scalar;
}

float Vector4::operator*(const Vector4& other) const
{
   return x * other.x + y * other.y + z * other.z + w * other.w;
}

Vector4 Vector4::operator+(const Vector4& other) const
{
   Vector4 result;
   result.x = x + other.x;
   result.y = y + other.y;
   result.z = z + other.z;
   result.w = w + other.w;
   return result;
}

void Vector4::operator+=(const Vector4& other)
{
   x += other.x;
   y += other.y;
   z += other.z;
   w += other.w;
}

Vector4 Vector4::operator-(const Vector4& other) const
{
   Vector4 result;
   result.x = x - other.x;
   result.y = y - other.y;
   result.z = z - other.z;
   result.w = w - other.w;
   return result;
}

Vector4 Vector4::operator-() const
{
   Vector4 result;
   result.x = -x;
   result.y = -y;
   result.z = -z;
   result.w = -w;
   return result;
}

void Vector4::operator-=(const Vector4& other)
{
   x -= other.x;
   y -= other.y;
   z -= other.z;
   w -= other.w;
}

void Vector4::operator<<(Stream& stream)
{
   load(&stream);
}

void Vector4::operator>>(Stream& stream)
{
   write(&stream);
}

void Vector4::load(Stream* stream)
{
   x = stream->getFloat();
   y = stream->getFloat();
   z = stream->getFloat();
   w = stream->getFloat();
}

void Vector4::write(Stream* stream)
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
   int r = static_cast<int32_t>(x * 255.0);
   int g = static_cast<int32_t>(y * 255.0);
   int b = static_cast<int32_t>(z * 255.0);
   int a = static_cast<int32_t>(w * 255.0);

   if (r < 0)
      r = 0;
   if (r > 255)
      r = 255;

   if (g < 0)
      g = 0;
   if (g > 255)
      g = 255;

   if (b < 0)
      b = 0;
   if (b > 255)
      b = 255;

   if (a < 0)
      a = 0;
   if (a > 255)
      a = 255;

   return (a << 24) | (b << 16) | (g << 8) | r;
}

Vector4 Vector4::linear(const Vector4& other, float interpolation_factor) const
{
   return *this + (other - *this) * interpolation_factor;
}

Vector Vector4::xyz() const
{
   return Vector(x, y, z);
}
