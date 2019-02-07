#ifndef VERTEX_H
#define VERTEX_H

class Vertex
{
public:
   Vertex();
   Vertex(float x, float y, float u, float v);
   ~Vertex();

   Vertex operator + (const Vertex& v) const;
   Vertex operator - (const Vertex& v) const;
   Vertex operator * (const float f) const;

   float x,y;
   float u,v;
};

#endif
