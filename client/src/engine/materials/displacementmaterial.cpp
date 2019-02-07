#include "stdafx.h"
#include "displacementmaterial.h"
#include "textureslot.h"
#include "tools/stream.h"
#include "tools/profiling.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/geometry.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "image/image.h"
#include "framework/globaltime.h"

DisplacementMaterial::DisplacementMaterial(SceneGraph *scene)
: Material(scene, MAP_DIFFUSE | MAP_DISPLACE),
  mColorMap(0),
  mDiffuseMap(0),
  mShader(0)
{
}

DisplacementMaterial::DisplacementMaterial(SceneGraph *scene, const char* map, const char* diffusemap)
: Material(scene, MAP_DIFFUSE | MAP_DISPLACE),
  mShader(0)
{
   addTexture(mColorMap, map);
   addTexture(mDiffuseMap, diffusemap);
}


void DisplacementMaterial::init()
{
   mShader= activeDevice->loadShader("displacementmaterial-vert.glsl", "displacementmaterial-frag.glsl");
   mParamTexture= activeDevice->getParameterIndex("texturemap");
   mParamDiffuse= activeDevice->getParameterIndex("diffusemap");
   mParamTime= activeDevice->getParameterIndex("time");
}

void DisplacementMaterial::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
   addTexture(mDiffuseMap, "diffuse_level");
}


void DisplacementMaterial::addGeometry(Geometry *geo)
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
         dst->pos.x= vtx[i].x;
         dst->pos.y= vtx[i].y;
         dst->pos.z= vtx[i].z;
         dst->uv.u= uv[i].u;
         dst->uv.v= uv[i].v;
         dst++;
      }
      activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );

      vb->setIndexBuffer( geo->getIndices(), geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}


void DisplacementMaterial::update(float, Node**, const Matrix& cam)
{
   mCamera= cam;
}


void DisplacementMaterial::begin()
{
   Material::begin();

   activeDevice->setCulling(false);
   glDisable(GL_BLEND);

   activeDevice->setShader( mShader );

   glBindTexture(GL_TEXTURE_2D, mColorMap);
   activeDevice->bindSampler(mParamTexture, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mDiffuseMap);
   activeDevice->bindSampler(mParamDiffuse, 1);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   float time= GlobalTime::Instance()->getTime();
   activeDevice->setParameter(mParamTime, time);
}

void DisplacementMaterial::end()
{
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setCulling(true);
   activeDevice->setShader(0);
}

void DisplacementMaterial::renderDiffuse()
{
   int count= 0;

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);

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

