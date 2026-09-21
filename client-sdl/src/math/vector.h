// simple 3d vector class

#pragma once

// do not derive from Streamable here!
// The pointer to the virtual function table will increase the size of "Vector" and we want it be exactly float[3]

class Stream;

class Vector
{
#define EPS 0.0001f

public:
   float x, y, z;

   Vector();
   Vector(float x, float y = 0.0f, float z = 0.0f);
   Vector(Stream& stream);

   static Vector normalize(const Vector& other);

   operator const float*() const
   {
      return &x;
   }  // cast to float*
   bool operator==(const Vector& other) const;   // equal
   bool operator!=(const Vector& other) const;   // not equal
   Vector operator+(const Vector& other) const;  // add two vectors
   void operator+=(const Vector& other);         // add another vector
   Vector operator-(const Vector& other) const;  // subtract two vectors
   Vector operator-() const;                     // negate
   void operator-=(const Vector& other);         // subtract another vector
   Vector operator*(const float scalar) const;   // multiply by scalar
   float operator*(const Vector& other) const;   // dot product
   void operator*=(const float scalar);          // multiply by scalar
   void operator/=(const float scalar);          // divsion by scalar
   Vector operator%(const Vector& other) const;  // cross product
   Vector abs() const;                           // return absilute values of vector

   void set(float x = 0, float y = 0, float z = 0);  // set components
   void normalize(float length = 1.0f);              // scale to length
   float length() const;                             // get length
   float* data() const;                              // get float[3] pointer to components
   int maxIndex() const;                             // get index of maximum component (0=x, 1=y, 2=z)
   int absMaxIndex() const;                          // get index of absolute maximum component
   void minimum(const Vector& other);                // component-wise minimum (1,2,3,4).minimum( (3,2,1,0) ) = (1,2,1,0)
   void maximum(const Vector& other);                // component-wise maximum

   void operator<<(Stream& stream);  // stream operator
   void operator>>(Stream& stream);  // stream operator

   void load(Stream* stream);   // load components from stream
   void write(Stream* stream);  // write components to stream
};
