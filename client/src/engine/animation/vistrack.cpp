#include "stdafx.h"
#include "vistrack.h"

VisTrack::VisTrack()
: Track<KeyBase>( Track::idVisibility )
{
}

int VisTrack::get(int time)
{
   int last= size()-1;

   // no keys: always visible
   if (last<0) return 1;

   // before first key: visible
   if (time<=key(0).time()) return 1;

   // after last key:
   if (time>=key(last).time()) return last & 1;

   int num=0;
   while (num<=last && time>=key(num).time()) num++;

   return 1-(num & 1);
}

