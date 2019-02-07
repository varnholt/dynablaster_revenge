// 2d vector class

#pragma once

#include "vector.h"

// do not derive from Streamable here!
// The pointer to the virtual function table will increase the size of "Vector" and we want it be exactly float[4]

class Stream;

class Vector2
{
#define EPS 0.0001f

public:
   float  x,y;

   Vector2();
   Vector2(float x, float y = 0.0f);
   Vector2(Stream& stream);

   operator const float* () const { return &x; }  // cast to float*
   Vector2 operator + (const Vector2& v) const;   // add two vectors
   void    operator +=(const Vector2& v);         // add another vector
   Vector2 operator - (const Vector2& v) const;   // subtract two vectors
   Vector2 operator - () const;                   // negate
   void    operator -=(const Vector2& v);         // subtract another vector
   Vector2 operator * (const float f) const;      // multiply by scalar
   float   operator * (const Vector2& v) const;   // dot product
   void    operator *=(const float f);            // multiply by scalar

   void   set(float x=0.0f, float y=0.0f);        // set components
   float* data() const;                           // get float[2] pointer to components
   Vector2 linear(const Vector2& v, float t) const;

   void   operator << (Stream& stream);         // stream operator
   void   operator >> (Stream& stream);         // stream operator

   void   load(Stream *stream);                 // load components from stream
   void   write(Stream *stream);                // write components to stream
};
