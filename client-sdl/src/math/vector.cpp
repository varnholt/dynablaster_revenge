// vector class implementation

#include "vector.h"
#include <math.h>
#include "tools/stream.h"

// default constructor: uninitialized(!) vector
Vector::Vector()
{
}

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z)
{
}

Vector::Vector(Stream& stream)
{
   load(&stream);
}

void Vector::set(float px, float py, float pz)
{
   x = px;
   y = py;
   z = pz;
}

Vector Vector::operator*(const float scalar) const
{
   Vector result;
   result.x = x * scalar;
   result.y = y * scalar;
   result.z = z * scalar;
   return result;
}

float Vector::operator*(const Vector& other) const
{
   return x * other.x + y * other.y + z * other.z;
}

void Vector::operator*=(const float scalar)
{
   x *= scalar;
   y *= scalar;
   z *= scalar;
}

void Vector::operator/=(const float scalar)
{
   float reciprocal = 1.0f / scalar;
   x *= reciprocal;
   y *= reciprocal;
   z *= reciprocal;
}

Vector Vector::operator+(const Vector& other) const
{
   Vector result;
   result.x = x + other.x;
   result.y = y + other.y;
   result.z = z + other.z;
   return result;
}

void Vector::operator+=(const Vector& other)
{
   x += other.x;
   y += other.y;
   z += other.z;
}

Vector Vector::operator-(const Vector& other) const
{
   Vector result;
   result.x = x - other.x;
   result.y = y - other.y;
   result.z = z - other.z;
   return result;
}

Vector Vector::operator-() const
{
   Vector result;
   result.x = -x;
   result.y = -y;
   result.z = -z;
   return result;
}

void Vector::operator-=(const Vector& other)
{
   x -= other.x;
   y -= other.y;
   z -= other.z;
}

Vector Vector::operator%(const Vector& other) const
{
   Vector result;
   result.x = y * other.z - z * other.y;
   result.y = z * other.x - x * other.z;
   result.z = x * other.y - y * other.x;
   return result;
}

bool Vector::operator==(const Vector& other) const
{
   float delta_x = x - other.x;
   float delta_y = y - other.y;
   float delta_z = z - other.z;
   float distance = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
   if (distance < 0.0001f)
      return true;
   else
      return false;
}

bool Vector::operator!=(const Vector& other) const
{
   float delta_x = x - other.x;
   float delta_y = y - other.y;
   float delta_z = z - other.z;
   float distance = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;  // no sqrt! compare with EPS^2 instead
   if (distance < EPS)
      return false;
   else
      return true;
}

void Vector::operator<<(Stream& stream)
{
   load(&stream);
}

void Vector::operator>>(Stream& stream)
{
   write(&stream);
}

void Vector::normalize(float length)
{
   float scale_factor = length / sqrt(x * x + y * y + z * z);
   x *= scale_factor;
   y *= scale_factor;
   z *= scale_factor;
}

Vector Vector::normalize(const Vector& other)
{
   Vector result;
   float scale_factor = 1.0f / sqrt(other.x * other.x + other.y * other.y + other.z * other.z);
   result.x = other.x * scale_factor;
   result.y = other.y * scale_factor;
   result.z = other.z * scale_factor;
   return result;
}

float Vector::length() const
{
   return sqrt(x * x + y * y + z * z);
}

void Vector::load(Stream* stream)
{
   x = stream->getFloat();
   y = stream->getFloat();
   z = stream->getFloat();
}

void Vector::write(Stream* stream)
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

void Vector::maximum(const Vector& other)
{
   if (other.x > x)
      x = other.x;
   if (other.y > y)
      y = other.y;
   if (other.z > z)
      z = other.z;
}

void Vector::minimum(const Vector& other)
{
   if (other.x < x)
      x = other.x;
   if (other.y < y)
      y = other.y;
   if (other.z < z)
      z = other.z;
}

Vector Vector::abs() const
{
   Vector result;
   result.x = fabs(x);
   result.y = fabs(y);
   result.z = fabs(z);
   return result;
}

int Vector::absMaxIndex() const
{
   float abs_x = fabs(x);
   float abs_y = fabs(y);
   float abs_z = fabs(z);
   if (abs_x >= abs_y && abs_x >= abs_z)
      return 0;
   else if (abs_y >= abs_z && abs_y >= abs_x)
      return 1;
   else
      return 2;
}
