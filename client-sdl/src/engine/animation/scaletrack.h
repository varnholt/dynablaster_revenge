// scale track

#pragma once

#include "math/matrix.h"
#include "scalekey.h"
#include "track.h"

class ScaleTrack : public Track<ScaleKey>
{
public:
   ScaleTrack();

   Matrix get(float time);

   void add(int time, const Scale& scale);

   virtual void load(Stream* stream);
   virtual void write(Stream* stream);
};
