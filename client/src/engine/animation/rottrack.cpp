#include "stdafx.h"
#include "rottrack.h"

RotTrack::RotTrack()
: Track<RotKey>( Track::idRotation )
{
}

void RotTrack::add(int time, const Quat& rot)
{
   addKey( RotKey(time, rot) );
}

void RotTrack::load(Stream *stream)
{
   Track<RotKey>::load(stream);

   for (int i=1;i<mSize;i++)
   {
      Quat q1= mData[i-1].value();
      Quat q2= mData[i].value();
      Quat q= q1*q2;
      if ((q1 % q) < 0) q=-q;
      mData[i].setValue( q );
   }
}

void RotTrack::write(Stream *stream)
{
   // TODO: delta

/*

   for (int i=1;i<mSize;i++)
   {
      Quat q1= mData[i-1].value();
      Quat q2= mData[i].value();
      Quat q= q1*q2;
      if ((q1 % q) < 0) q=-q;
      mData[i].set( q );
   }
*/

   Track<RotKey>::write(stream);
}

Quat RotTrack::get(float time)
{
   int last= size()-1;

   if (last<0) return Quat(1,0,0,0);

   if (time<=key(0).time()) return key(0).value();
   if (time>=key(last).time()) return key(last).value();

   float f= interpolate(time);

   Quat q1= prevKey().value();
   Quat q2= nextKey().value();

   Quat q;// = q1 + (q2-q1)*f;
   q= Quat::slerp(q1, q2, f);
   return q;
}

