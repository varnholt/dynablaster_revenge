// vector class implementation

#include "vector2.h"
#include <math.h>
#include "tools/stream.h"

// default constructor: uninitialized(!) vector
Vector2::Vector2()
{
}

Vector2::Vector2(float x, float y) : x(x), y(y)
{
}

Vector2::Vector2(Stream& stream)
{
   load(&stream);
}

void Vector2::set(float px, float py)
{
   x = px;
   y = py;
}

Vector2 Vector2::operator*(const float scalar) const
{
   Vector2 result;
   result.x = x * scalar;
   result.y = y * scalar;
   return result;
}

void Vector2::operator*=(const float scalar)
{
   x *= scalar;
   y *= scalar;
}

float Vector2::operator*(const Vector2& other) const
{
   return x * other.x + y * other.y;
}

Vector2 Vector2::operator+(const Vector2& other) const
{
   Vector2 result;
   result.x = x + other.x;
   result.y = y + other.y;
   return result;
}

void Vector2::operator+=(const Vector2& other)
{
   x += other.x;
   y += other.y;
}

Vector2 Vector2::operator-(const Vector2& other) const
{
   Vector2 result;
   result.x = x - other.x;
   result.y = y - other.y;
   return result;
}

Vector2 Vector2::operator-() const
{
   Vector2 result;
   result.x = -x;
   result.y = -y;
   return result;
}

void Vector2::operator-=(const Vector2& other)
{
   x -= other.x;
   y -= other.y;
}

void Vector2::operator<<(Stream& stream)
{
   load(&stream);
}

void Vector2::operator>>(Stream& stream)
{
   write(&stream);
}

void Vector2::load(Stream* stream)
{
   x = stream->getFloat();
   y = stream->getFloat();
}

void Vector2::write(Stream* stream)
{
   stream->writeFloat(x);
   stream->writeFloat(y);
}

float* Vector2::data() const
{
   return (float*)&x;
}

Vector2 Vector2::linear(const Vector2& other, float interpolation_factor) const
{
   return *this + (other - *this) * interpolation_factor;
}
