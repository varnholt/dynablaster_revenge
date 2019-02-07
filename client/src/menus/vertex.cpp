#include "vertex.h"

Vertex::Vertex()
: x(0.0f)
, y(0.0f)
, u(0.0f)
, v(0.0f)
{
}

Vertex::Vertex(float x, float y, float u, float v)
: x(x)
, y(y)
, u(u)
, v(v)
{
}

Vertex::~Vertex()
{
}

Vertex Vertex::operator + (const Vertex& vtx) const
{
   return 
      Vertex(
         x+vtx.x, 
         y+vtx.y, 
         u+vtx.u,
         v+vtx.v
      );
}

Vertex Vertex::operator - (const Vertex& vtx) const
{
   return 
      Vertex(
         x-vtx.x, 
         y-vtx.y, 
         u-vtx.u,
         v-vtx.v
      );
}

Vertex Vertex::operator * (const float f) const
{
   return 
      Vertex(
         x*f,
         y*f,
         u*f,
         v*f
      );
}
