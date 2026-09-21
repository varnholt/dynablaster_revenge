#include "scale.h"

Scale::Scale()
{
}

Scale::Scale(const Quat& rotation, const Vector& scale) : _rotation(rotation), _scale(scale)
{
}

void Scale::load(Stream* stream)
{
   _rotation.load(stream);
   _scale.load(stream);
}

void Scale::write(Stream* stream)
{
   _rotation.write(stream);
   _scale.write(stream);
}

void Scale::operator<<(Stream& stream)
{
   load(&stream);
}

void Scale::operator>>(Stream& stream)
{
   write(&stream);
}

const Vector& Scale::value() const
{
   return _scale;
}

const Quat& Scale::orientation() const
{
   return _rotation;
}
