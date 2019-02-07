#include "animatedmesh.h"
#include "render/geometry.h"

AnimatedMesh::AnimatedMesh(Node *parent)
: Mesh(parent)
{
   setUserTransformable(true);
}

AnimatedMesh::~AnimatedMesh()
{
}

int AnimatedMesh::getFrameCount() const
{
   return mGeometry.size();
}

Geometry* AnimatedMesh::getFrame(int frame)
{
   return mGeometry[frame];
}

