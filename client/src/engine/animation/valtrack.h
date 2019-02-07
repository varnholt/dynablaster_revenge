// value track
// animates a single value (eg. fov)

#pragma once

#include "valkey.h"
#include "track.h"

class ValTrack : public Track<ValKey>
{
public:
   ValTrack();
   float get(float time);

   void add(int time, float val);
};
