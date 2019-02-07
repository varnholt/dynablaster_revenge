// visibility track

#pragma once

#include "key.h"
#include "track.h"

class VisTrack : public Track<KeyBase>
{
public:
   VisTrack();

   int get(int time);
};
