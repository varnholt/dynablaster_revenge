#include "stdafx.h"
#include "environmentmaterial.h"
#include "textureslot.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "image/image.h"
#include "tools/stream.h"
#include "tools/profiling.h"
#include "animation/motionmixer.h"

EnvironmentMaterial::EnvironmentMaterial(SceneGraph *scene)
: Material(scene, MAP_REFLECT)
, mSpecularMap(0)
, mShader(0)
, mParamSpecular(0)
, mParamCamera(0)
{
}

EnvironmentMaterial::EnvironmentMaterial(SceneGraph *scene, const char *specmap)
: Material(scene, MAP_DIFFUSE | MAP_REFLECT)
{
   addTexture(mSpecularMap, specmap, 1|2|4);
}

EnvironmentMaterial::~EnvironmentMaterial()
{
}

void EnvironmentMaterial::init()
{
   mShader= activeDevice->loadShader("environment-vert.glsl", "environment-frag.glsl");
   mParamSpecular= activeDevice->getParameterIndex("specularmap");
   mParamCamera= activeDevice->getParameterIndex("camera");
}

void EnvironmentMaterial::load(Stream *stream)
{
   Material::load(stream);
   addTexture(mSpecularMap, getTextureSlot(0)->name(), 1|2|4);
}


void EnvironmentMaterial::addGeometry(Geometry *geo)
{
   VertexBuffer *vb= mPool->get(geo);
   if (!vb)
   {
      vb= mPool->add(geo);

      Vector *vtx= geo->getVertices();
      Vector *nrm= geo->getNormals();

      activeDevice->allocateVertexBuffer( vb->getVertexBuffer(), sizeof(Vertex)*geo->getVertexCount() );
      volatile Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );
      for (int i=0;i<geo->getVertexCount();i++)
      {
         dst->pos.x= vtx[i].x;
         dst->pos.y= vtx[i].y;
         dst->pos.z= vtx[i].z;
         dst->normal.x= nrm[i].x;
         dst->normal.y= nrm[i].y;
         dst->normal.z= nrm[i].z;
         dst++;
      }
      activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );

      vb->setIndexBuffer( geo->getIndices(), geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}


void EnvironmentMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamSpecular, 0);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
}

void EnvironmentMaterial::end()
{
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_NORMAL_ARRAY);

   activeDevice->setShader(0);
}


void EnvironmentMaterial::renderDiffuse()
{
   begin();

/*
   Matrix projMat;
   glGetFloatv(GL_PROJECTION_MATRIX, projMat.data());
   projMat= projMat.invert();
   Vector camPos= projMat.translation();
*/

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      VertexBuffer *vb= mVB[i].vb;
      Geometry *geo= mVB[i].geo;

      if (geo->isVisible())
      {
         Matrix invView= (geo->getTransform() * mCamera).invert();
         Vector osCam= invView.translation();
//         Vector osCam= geo->getParent()->getWorld2Obj() * camPos;
	      activeDevice->setParameter(mParamCamera, osCam);

//         if (geo->getBoneCount()==0)
            activeDevice->push(geo->getTransform());

         // draw mesh
         glBindBuffer( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
         glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
         glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)sizeof(Vector) );  

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
         glDrawElements( GL_TRIANGLES, vb->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

         activeDevice->pop();
      }
   }

   end();
}

void EnvironmentMaterial::update(float /*frame*/, Node** /* nodelist */, const Matrix& cam )
{
   mCamera= cam;
}
