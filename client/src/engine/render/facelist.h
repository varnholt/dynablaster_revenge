// facelist class
// holds an array of vertex-indices (3 per polygon)
// actually this is just an array<int>, but since integers can't load themselfes,
// we use this simple helper class here

#pragma once

#include "tools/array.h"
#include "tools/streamable.h"

class FaceList : public Array<unsigned short>, public Streamable
{
public:   
   void load(Stream *stream);
   void write(Stream *stream);
};
