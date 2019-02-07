// quaternion class

#pragma once

class Vector;
class Matrix;
class Stream;

class Quat
{
public:
   float x,y,z,w;

   Quat();
   Quat(const Matrix& mat);
   Quat(float x, float y, float z, float w);

   Quat  operator - () const;
   Quat  operator + (const Quat& q) const;
   Quat  operator - (const Quat& q) const;
   Quat  operator * (const float f) const;
   Quat  operator * (const Quat& q) const;
   float operator % (const Quat& q) const;

   Quat conjugate() const;
   Quat snuggle(Vector& k);

   void   operator << (Stream& stream);         // stream operator
   void   operator >> (Stream& stream);         // stream operator

   void load(Stream *stream);
   void write(Stream *stream);

   static Quat slerp(const Quat& q1, const Quat& q2, float t);
};
