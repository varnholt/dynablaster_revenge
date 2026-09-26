#include "vertexbufferpool.h"
#include "geometry.h"
#include "renderdevice.h"
#include "vertexbuffer.h"

VertexBufferPool::VertexBufferPool()
{
}

VertexBufferPool::~VertexBufferPool()
{
}

bool VertexBufferPool::contains(Geometry* geo) const
{
   return mPool.contains(geo->getID());
}

VertexBuffer* VertexBufferPool::get(Geometry* geo)
{
   auto it = mPool.find(geo->getID());
   if (it != mPool.end())
      return it->second;
   else
      return 0;
}

VertexBuffer* VertexBufferPool::add(Geometry* geo)
{
   VertexBuffer* buffer = new VertexBuffer(geo);
   mPool[geo->getID()] = buffer;
   return buffer;
}
