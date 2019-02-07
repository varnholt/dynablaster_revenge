#ifndef ANIMATEDMESH_H
#define ANIMATEDMESH_H

#include "mesh.h"

class AnimatedMesh : public Mesh
{
public:
   AnimatedMesh(Node *parent = 0);
   ~AnimatedMesh();

   int getFrameCount() const;
   Geometry* getFrame(int frame);

private:
};

#endif
