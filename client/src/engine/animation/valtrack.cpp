#include "stdafx.h"
#include "valtrack.h"

ValTrack::ValTrack()
: Track<ValKey>( Track::idValue )
{
}

void ValTrack::add(int time, float val)
{
   ValKey key(time, val);
   addKey(key);
}

float ValTrack::get(float time)
{
   int last= size()-1;
   if (last<0) return 0;
   if (time<=key(0).time()) return key(0).value();
   if (time>=key(last).time()) return key(last).value();

   float f= interpolate(time);

   float a= prevKey().value();
   float b= nextKey().value();
   return a + (b-a)*f;
}

