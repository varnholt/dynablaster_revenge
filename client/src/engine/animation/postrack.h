// position track

#pragma once

#include "poskey.h"
#include "track.h"

class PosTrack : public Track<PosKey>
{
public:
   PosTrack();

   void add(int time, const Vector& pos);

   Vector get(float time);
};

