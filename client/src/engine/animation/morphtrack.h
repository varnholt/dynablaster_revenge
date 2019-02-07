// morph track
// animates a set of vertices

#pragma once

#include "morphkey.h"
#include "track.h"

class MorphTrack : public Track<MorphKey>
{
public:
   MorphTrack();

   void load(Stream *stream);
   const List<Vector>& get(float time);

   void add(int time, const List<Vector>& list);

private:
   List<Vector> mVertices;
};
