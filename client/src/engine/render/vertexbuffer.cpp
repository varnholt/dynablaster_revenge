#include "stdafx.h"
#include "vertexbuffer.h"
#include "renderdevice.h"
#include "geometry.h"
#include "../nodes/node.h"

VertexBuffer::VertexBuffer(Geometry *geo)
: mGeometry(geo)
, mVertex(0)
, mIndex(0)
, mSize(0)
{
   mVertex= activeDevice->createBuffer();
   mIndex= activeDevice->createBuffer();
   mSize= 0;
}



VertexBuffer::~VertexBuffer()
{
   activeDevice->deleteBuffer(mVertex);
   activeDevice->deleteBuffer(mIndex);
}

Geometry* VertexBuffer::getGeometry() const
{
   return mGeometry;
}


void VertexBuffer::setIndexBuffer( unsigned short* indices, int count )
{
   activeDevice->allocateIndexBuffer( mIndex, count*sizeof(unsigned short) );
   unsigned short *dst= (unsigned short*)activeDevice->lockIndexBuffer( mIndex );
   for (int i=0; i<count; i++)
      dst[i]= indices[i];
   activeDevice->unlockIndexBuffer( mIndex );
   mSize= count;
}


void VertexBuffer::update(Node ** /*nodelist*/)
{
/*
   if (mWeights)
   {
      Vector *dst= (Vector*)activeDevice->lockVertexBuffer(mVertex);
      Vector *src= mOrgVerts;

      for (int i=0;i<mVertexCount;i++)
      {
         int   num= mWeights[i].size();

         if (num>0)
         {
            int   *id= mWeights[i].mID;
            float *w=  mWeights[i].mWeight;

            Vector n(0,0,0);
            for (int j=0;j<num;j++)
            {
               Node *node= nodelist[id[j]];
               Matrix mat= node->getBoneTransform();
               n+= (mat * (src[i]) ) * w[j];
            }
            dst[i]= n;
         }
      }

      Matrix m;
      m.identity();
      mNode->setTransform(m);

      activeDevice->unlockVertexBuffer(mVertex);
   }
*/
}

unsigned int VertexBuffer::getVertexBuffer() const
{
   return mVertex;
}

unsigned int VertexBuffer::getIndexBuffer() const
{
   return mIndex;
}

int VertexBuffer::getIndexCount() const
{
   return mSize;
}

void VertexBuffer::setIndexCount(int count)
{
   mSize= count;
}

