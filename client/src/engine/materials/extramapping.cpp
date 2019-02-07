#include "stdafx.h"
#include "extramapping.h"
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

ExtraMapping::ExtraMapping(SceneGraph *scene)
: Material(scene, -3),
  mColorMap(0)
{
}

ExtraMapping::ExtraMapping(SceneGraph *scene, const char *map)
: Material(scene, -3)
{
   addTexture(mColorMap, map);
}

void ExtraMapping::init()
{
   mShader= activeDevice->loadShader("texturemapping-vert.glsl", "texturemapping-frag.glsl");

   mParamTexture= activeDevice->getParameterIndex("texturemap");
}

void ExtraMapping::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
}


void ExtraMapping::addGeometry(Geometry *geo)
{
   VertexBuffer *vb= mPool->get(geo);
   if (!vb)
   {
      vb= mPool->add(geo);

      Vector *vtx= geo->getVertices();
      UV *uv= geo->getUV(1);

      activeDevice->allocateVertexBuffer( vb->getVertexBuffer(), sizeof(Vertex)*geo->getVertexCount() );
      volatile Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );
      for (int i=0;i<geo->getVertexCount();i++)
      {
         dst[i].pos.x= vtx[i].x;
         dst[i].pos.y= vtx[i].y;
         dst[i].pos.z= vtx[i].z;
         dst[i].uv.u= uv[i].u;
         dst[i].uv.v= uv[i].v;
      }
      activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );

      vb->setIndexBuffer( geo->getIndices(), geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}

void ExtraMapping::update(float, Node**, const Matrix& cam)
{
   mCamera= cam;
}

void ExtraMapping::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

   glBindTexture(GL_TEXTURE_2D, mColorMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamTexture, 0);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ExtraMapping::end()
{
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glDisable(GL_BLEND);

   activeDevice->setShader(0);
}

void ExtraMapping::renderDiffuse()
{
   int count= 0;

   begin();

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      VertexBuffer *vb= mVB[i].vb;
      Geometry *geo= mVB[i].geo;

      if (geo->isVisible())
      {
//         Vector osCam= geo->getParent()->getCamera2Obj().translation();
//         activeDevice->setParameter(mParamCamera, osCam);

         activeDevice->push(geo->getTransform());

         // draw mesh
         glBindBuffer( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
         glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
         glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)sizeof(Vector) );  

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
         glDrawElements( GL_TRIANGLES, vb->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

         activeDevice->pop();
         count++;
      }
   }

   end();
}

