// 4d vector class

#pragma once

#include "vector.h"

// do not derive from Streamable here!
// The pointer to the virtual function table will increase the size of "Vector" and we want it be exactly float[4]

class Stream;

class Vector4
{
#define EPS 0.0001f

public:
   float  x,y,z,w;

   Vector4();
   Vector4(float x, float y = 0.0f, float z = 0.0f, float w = 1.0f);
   Vector4(Stream& stream);

   operator const float* () const { return &x; }// cast to float*
   Vector4 operator + (const Vector4& v) const;   // add two vectors
   void    operator +=(const Vector4& v);         // add another vector
   Vector4 operator - (const Vector4& v) const;   // subtract two vectors
   Vector4 operator - () const;                   // negate
   void    operator -=(const Vector4& v);         // subtract another vector
   Vector4 operator * (const float f) const;      // multiply by scalar
   float   operator * (const Vector4& v) const;   // dot product
   void    operator *=(const float f);            // multiply by scalar

   void   set(float x=0.0f, float y=0.0f, float z=0.0f, float w=0.0f); // set components
   float* data() const;                         // get float[4] pointer to components
   unsigned int rgba() const;
   Vector4 linear(const Vector4& v, float t) const;

   Vector xyz() const;

   void   operator << (Stream& stream);         // stream operator
   void   operator >> (Stream& stream);         // stream operator

   void   load(Stream *stream);                 // load components from stream
   void   write(Stream *stream);                // write components to stream
};
