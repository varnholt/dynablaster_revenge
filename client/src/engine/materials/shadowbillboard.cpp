#include "stdafx.h"
#include "shadowbillboard.h"
#include "tools/stream.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/geometry.h"
#include "render/uv.h"
#include "tools/profiling.h"
#include "image/image.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "textureslot.h"

ShadowBillboard::ShadowBillboard(SceneGraph *scene)
: Material(scene, -1),
  mColorMap(0),
  mShader(0),
  mVertices(0),
  mTexcoords(0),
  mIndices(0),
  mOffset(0.0f, 0.0f)
{
}

ShadowBillboard::ShadowBillboard(SceneGraph *scene, const char *map)
: Material(scene, -1),
  mColorMap(0),
  mShader(0),
  mVertices(0),
  mTexcoords(0),
  mIndices(0),
  mOffset(0.0f, 0.0f)
{
   addTexture(mColorMap, map, 1|2|4);
}

void ShadowBillboard::removeMesh(Mesh *mesh)
{
   for (int i=0; i<mesh->getPartCount(); i++)
   {
      mInstances.remove( mesh->getPart(i) );
   }
}

void ShadowBillboard::init()
{
   mShader= activeDevice->loadShader("shadowbillboard-vert.glsl", "shadowbillboard-frag.glsl");
   mParamTexture= activeDevice->getParameterIndex("texturemap");

   mVertices= activeDevice->createVertexBuffer( sizeof(Vector) * 4 * 1000, true );
   mTexcoords= activeDevice->createVertexBuffer( sizeof(float) * 2 * 4 * 1000 );
   mIndices= activeDevice->createIndexBuffer( sizeof(unsigned short) * 6 * 1000 );

   volatile float *dst= (float*)activeDevice->lockVertexBuffer( mTexcoords );
   float minUV= 0.01f;
   float maxUV= 0.99f;

   for (int i=0;i<1000;i++)
   {
      *dst++= minUV;
      *dst++= maxUV;

      *dst++= maxUV;
      *dst++= maxUV;

      *dst++= maxUV;
      *dst++= minUV;

      *dst++= minUV;
      *dst++= minUV;
   }
   activeDevice->unlockVertexBuffer( mTexcoords );

   volatile unsigned short *idx= (unsigned short*)activeDevice->lockIndexBuffer( mIndices );
   for (int i=0;i<1000;i++)
   {
      *idx++= i*4+0;
      *idx++= i*4+1;
      *idx++= i*4+2;
      *idx++= i*4+0;
      *idx++= i*4+2;
      *idx++= i*4+3;
   }
   activeDevice->unlockIndexBuffer( mIndices );
}

void ShadowBillboard::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
}

void ShadowBillboard::setOffset(float x, float y)
{
   mOffset.set(x,y);
}

void ShadowBillboard::addGeometry(Geometry *geo)
{
   Bounding bound;
   Matrix mat;//= geo->getTransform();
//   mat.translate( Vector(0.0f) );
//   mat= mat.invert();
   bound.min= Vector(-1.5f-mOffset.x, -1.5f-mOffset.y);
   bound.max= Vector( 1.5f-mOffset.x,  1.5f-mOffset.y);
   mInstances.insert(geo, bound);

/*
   if (!vb)
   {
      vb= mPool->add(geo);

      {
         Vector bmin(0.0f, 0.0f, 0.0f);
         Vector bmax(0.0f, 0.0f, 0.0f);
         geo->calcBoundingBox(bmin, bmax);

         Vector t= bmin;
         bmin.minimum( -bmax );
         bmax.maximum( -t );

         activeDevice->allocateVertexBuffer( vb->getVertexBuffer(), sizeof(Vertex)*geo->getVertexCount() );
         volatile Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );

         float z= 0.2f;

         float dx= bmax.x - bmin.x;
         float dy= bmax.y - bmin.y;

         bmin -= Vector(dx,dy,0.0f);
         bmax += Vector(dx,dy,0.0f);

         bmin.y -= 0.1f;
         bmax.y -= 0.1f;


         float minUV= 0.01f;
         float maxUV= 0.99f;
         dst[0].pos.x= bmin.x;
         dst[0].pos.y= bmin.y;
         dst[0].pos.z= z;
         dst[0].uv.u= minUV;
         dst[0].uv.v= maxUV;

         dst[1].pos.x= bmax.x;
         dst[1].pos.y= bmin.y;
         dst[1].pos.z= z;
         dst[1].uv.u= maxUV;
         dst[1].uv.v= maxUV;

         dst[2].pos.x= bmax.x;
         dst[2].pos.y= bmax.y;
         dst[2].pos.z= z;
         dst[2].uv.u= maxUV;
         dst[2].uv.v= minUV;

         dst[3].pos.x= bmin.x;
         dst[3].pos.y= bmax.y;
         dst[3].pos.z= z;
         dst[3].uv.u= minUV;
         dst[3].uv.v= minUV;

         activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );
      }

      activeDevice->allocateIndexBuffer( vb->getIndexBuffer(), 6*sizeof(unsigned short) );
      volatile unsigned short *idx= (unsigned short*)activeDevice->lockIndexBuffer( vb->getIndexBuffer() );
      *idx++= 0; *idx++= 1; *idx++= 2;
      *idx++= 0; *idx++= 2; *idx++= 3;
      activeDevice->unlockIndexBuffer( vb->getIndexBuffer() );
      vb->setIndexCount( 6 );


   }

   mVB.add(Material::Buffer(geo,vb));
*/
}


void ShadowBillboard::begin()
{
   Material::begin();

   glDepthMask(GL_FALSE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ZERO, GL_SRC_COLOR);
   glEnable(GL_ALPHA_TEST);

   glActiveTexture(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamTexture, 0);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}


void ShadowBillboard::end()
{

   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   glDisable(GL_ALPHA_TEST);

   activeDevice->setShader(0);
}


void ShadowBillboard::renderDiffuse()
{
   begin();

   float z= 0.1f;
   int count= 0;
   Vector* dst= (Vector*)activeDevice->lockVertexBuffer( mVertices );
   QMap<Geometry*,Bounding>::ConstIterator it;
   for (it= mInstances.constBegin(); it!= mInstances.constEnd(); it++)
   {
      // get vertex buffer
      Geometry *geo= it.key();
      const Bounding& bound= it.value();

      if (geo->isVisible())
      {
         //         Vector osCam= geo->getParent()->getCamera2Obj().translation();
         //         activeDevice->setParameter(mParamCamera, osCam);

         Matrix obj= geo->getTransform().normalized();

         Vector bmin= bound.min + obj.translation();
         Vector bmax= bound.max + obj.translation();

         dst->x= bmin.x;
         dst->y= bmin.y;
         dst->z= z;
         dst++;

         dst->x= bmax.x;
         dst->y= bmin.y;
         dst->z= z;
         dst++;

         dst->x= bmax.x;
         dst->y= bmax.y;
         dst->z= z;
         dst++;

         dst->x= bmin.x;
         dst->y= bmax.y;
         dst->z= z;
         dst++;

         count++;
      }
   }
   activeDevice->unlockVertexBuffer( mVertices );

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertices );
   glVertexPointer( 3, GL_FLOAT, sizeof(Vector), (GLvoid*)0 );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, mTexcoords );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(float)*2, (GLvoid*)0 );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndices );

   glDrawElements( GL_TRIANGLES, count*6, GL_UNSIGNED_SHORT, (void*)0 );

   end();
//   printf("shadows [%d]: %f \n", mVB.size(), (t2-t1)/1000000.0);
}
