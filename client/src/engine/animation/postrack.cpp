#include "stdafx.h"
#include "postrack.h"

PosTrack::PosTrack()
: Track<PosKey>( Track::idPosition )
{
}

void PosTrack::add(int time, const Vector& pos)
{
   addKey( PosKey(time, pos) );
}

Vector PosTrack::get(float time)
{
   int last= size()-1;

   if (last<0) return Vector(0,0,0);

   if (time<=key(0).time()) return key(0).value();
   if (time>=key(last).time()) return key(last).value();

   float f= interpolate(time);

   Vector v1= prevKey().value();
   Vector v2= nextKey().value();

   Vector v= v1 + (v2-v1)*f;

   return v;
}

