#include "stdafx.h"
#include "renderbuffer.h"
#include "renderdevice.h"
#include "nodes/mesh.h"
#include "math/vector.h"
#include "weight.h"
#include "geometry.h"

RenderBuffer::RenderBuffer(Geometry *geo)
 : mNode(geo->getParent()),
   mGeometry(geo),
   mFormat(0),
   mSize(0),
   mVertexCount(0),
   mOrgVerts(NULL),
   mWeights(NULL),
   mVertex(0),
   mIndex(0)
{
/*
   mEdgeSize= geo->getEdgeCount();
   mEdge= activeDevice->createIndexBuffer(mEdgeSize*sizeof(short), false);
   dst= activeDevice->lockIndexBuffer(mEdge);
   memcpy(dst, geo->getEdges(), mEdgeSize*sizeof(short));
   activeDevice->unlockIndexBuffer(mEdge);
*/
}


RenderBuffer::~RenderBuffer()
{
}


unsigned int RenderBuffer::createVertexBuffer(void *data, int size, bool dyn)
{
   unsigned int buf;
   buf= activeDevice->createVertexBuffer(size, dyn);
   void *dst= activeDevice->lockVertexBuffer(buf);
   if (data)
      memcpy(dst, data, size);
   else
      memset(dst, 0, size);
   activeDevice->unlockVertexBuffer(buf);
   return buf;
}


unsigned int RenderBuffer::createIndexBuffer(void *data, int size, bool dyn)
{
   unsigned int buf;
   buf= activeDevice->createIndexBuffer(size, dyn);
   void *dst= activeDevice->lockIndexBuffer(buf);
   if (data)
      memcpy(dst, data, size);
   else
      memset(dst, 0, size);
   activeDevice->unlockIndexBuffer(buf);
   return buf;
}


void RenderBuffer::deleteBuffer(unsigned int buffer)
{
   activeDevice->deleteBuffer(buffer);
}


void RenderBuffer::update(Node**)
{
   if (mWeights)
   {
      // Node *mesh= mGeometry->getParent();
      // Vector *dst= mGeometry->getVertices();
      // Vector *src= mOrgVerts;

/*
      for (int i=0;i<mVertexCount;i++)
      {
         int   num= mWeights[i].size();

         if (num>0)
         {
            const Weight& weight= mWeights[i];

            Vector v(0,0,0);
            for (int j=0;j<num;j++)
            {
               int id= weight.id(j);
               float w= weight.weight(j);

               Node *node= nodelist[id];
               Matrix mat= node->getBoneTransform();
               v+= (mat * src[i]) * w;
            }
            dst[i]= mesh->getInitTransform() * v;
         }
      }
*/

      Vector *vram= (Vector*)activeDevice->lockVertexBuffer(mVertex);
      memcpy(vram, mGeometry->getVertices(), mGeometry->getVertexCount()*sizeof(Vector));
      activeDevice->unlockVertexBuffer(mVertex);
/*
      Matrix mat;
      mat.identity();
      mGeometry->getNode()->setTransform(mat);
*/
   }
}


const Matrix& RenderBuffer::getTransform() const
{
   return mNode->getTransform();
}

int RenderBuffer::getSize() const
{
   return mSize;
}

void RenderBuffer::setSize(int num)
{ 
   mSize= num; 
}

unsigned int RenderBuffer::getVertexBuffer() const
{
   return mVertex;
}

unsigned int RenderBuffer::getIndexBuffer() const
{
   return mIndex;
}

Geometry* RenderBuffer::getGeometry() const
{
   return mGeometry;
}

bool RenderBuffer::getVisible() const
{
   return mGeometry->isVisible();
}

void RenderBuffer::clear()
{

}
