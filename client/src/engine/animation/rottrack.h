// rotation track

#pragma once

#include "rotkey.h"
#include "track.h"

class RotTrack : public Track<RotKey>
{
public:
   RotTrack();

   Quat get(float time);

   void add(int time, const Quat& scale);

   virtual void load(Stream *stream);
   virtual void write(Stream *stream);
};

