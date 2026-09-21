// contains 3d scaling with orientation

#pragma once

#include "quat.h"
#include "vector.h"

class Scale
{
public:
   Scale();
   Scale(const Quat& q, const Vector& v);

   void operator<<(Stream& stream);  // stream operator
   void operator>>(Stream& stream);  // stream operator

   void load(Stream* stream);
   void write(Stream* stream);
   const Vector& value() const;
   const Quat& orientation() const;

private:
   Quat mRot;
   Vector mScale;
};
