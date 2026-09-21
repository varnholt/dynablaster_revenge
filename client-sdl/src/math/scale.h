// contains 3d scaling with orientation

#pragma once

#include "quat.h"
#include "vector.h"

class Scale
{
public:
   Scale();
   Scale(const Quat& rotation, const Vector& scale);

   void operator<<(Stream& stream);  // stream operator
   void operator>>(Stream& stream);  // stream operator

   void load(Stream* stream);
   void write(Stream* stream);
   const Vector& value() const;
   const Quat& orientation() const;

private:
   Quat _rotation;
   Vector _scale;
};
