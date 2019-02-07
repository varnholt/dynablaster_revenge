#ifndef VERTEXBUFFERPOOL_H
#define VERTEXBUFFERPOOL_H

#include <QMap>

class Geometry;
class VertexBuffer;

class VertexBufferPool
{
public:
   VertexBufferPool();
   ~VertexBufferPool();

   bool contains(Geometry *geo) const;
   VertexBuffer* add(Geometry *geo);
   VertexBuffer* get(Geometry *geo);

private:
   QMap<int,VertexBuffer*> mPool;
};

#endif
