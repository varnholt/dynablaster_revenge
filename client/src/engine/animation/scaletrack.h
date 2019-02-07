// scale track

#pragma once

#include "scalekey.h"
#include "track.h"
#include "math/matrix.h"

class ScaleTrack : public Track<ScaleKey>
{
public:
   ScaleTrack();

   Matrix get(float time);

   void add(int time, const Scale& scale);

   virtual void load(Stream *stream);
   virtual void write(Stream *stream);
};
