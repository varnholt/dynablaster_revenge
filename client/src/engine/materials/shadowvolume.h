// shadow volumes

#ifndef SHADOWVOLUME_H
#define SHADOWVOLUME_H

#include "material.h"

class ShadowVolume : public Material
{
public:
   ShadowVolume();
   void load(Stream *stream);
   void add(Geometry *geo);
   void update(Node **nodes);
   void renderAmbient();

private:
   int calcEdgeIndices(unsigned int vertexbuffer, Geometry *geo, const Vector& lightdir);

   unsigned int mTransform;
};

#endif
