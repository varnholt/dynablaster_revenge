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
   void get(List<Vector>& vtx, List<Vector>& nrm, float time);

   void calculateNormals(const Array<unsigned short>& indices);

private:
};
