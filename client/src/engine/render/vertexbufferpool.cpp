#include "vertexbufferpool.h"
#include "vertexbuffer.h"
#include "renderdevice.h"
#include "geometry.h"


VertexBufferPool::VertexBufferPool()
{
}

VertexBufferPool::~VertexBufferPool()
{
}

bool VertexBufferPool::contains(Geometry *geo) const
{
   return mPool.contains(geo->getID());
}

VertexBuffer* VertexBufferPool::get(Geometry *geo)
{
   QMap<int,VertexBuffer*>::ConstIterator it= mPool.constFind(geo->getID());
   if (it != mPool.constEnd())
      return it.value();
   else
      return 0;
}

VertexBuffer* VertexBufferPool::add(Geometry *geo)
{
   VertexBuffer *buffer= new VertexBuffer(geo);
   mPool.insert(geo->getID(), buffer);
   return buffer;
}

