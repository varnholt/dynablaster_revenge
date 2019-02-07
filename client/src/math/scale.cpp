#include "scale.h"

Scale::Scale()
{
}

Scale::Scale(const Quat& q, const Vector& v)
: mRot(q)
, mScale(v)
{
}

void Scale::load(Stream *stream)
{
   mRot.load(stream);
   mScale.load(stream);
}

void Scale::write(Stream *stream)
{
   mRot.write(stream);
   mScale.write(stream);
}

void Scale::operator << (Stream& stream)
{
   load(&stream);
}

void Scale::operator >> (Stream& stream)
{
   write(&stream);
}

const Vector& Scale::value() const
{
   return mScale;
}

const Quat& Scale::orientation() const
{
   return mRot;
}
