#ifndef VERTEXBUFFERPOOL_H
#define VERTEXBUFFERPOOL_H

#include <unordered_map>

class Geometry;
class VertexBuffer;

class VertexBufferPool
{
public:
   VertexBufferPool();
   ~VertexBufferPool();

   bool contains(Geometry* geo) const;
   VertexBuffer* add(Geometry* geo);
   VertexBuffer* get(Geometry* geo);

private:
   std::unordered_map<int, VertexBuffer*> mPool;
};

#endif
