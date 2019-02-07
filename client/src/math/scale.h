// contains 3d scaling with orientation

#pragma once

#include "vector.h"
#include "quat.h"

class Scale
{
public:
   Scale();
   Scale(const Quat& q, const Vector& v);

   void   operator << (Stream& stream);         // stream operator
   void   operator >> (Stream& stream);         // stream operator

   void          load(Stream *stream);
   void          write(Stream *stream);
   const Vector& value() const;
   const Quat&   orientation() const;

private:
   Quat   mRot;
   Vector mScale;
};
