#include "stdafx.h"
#include "environmentambientdiffusematerial.h"
#include "materials/textureslot.h"
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

EnvironmentAmbientDiffuseMaterial::EnvironmentAmbientDiffuseMaterial(SceneGraph *scene)
: Material(scene, MAP_AMBIENT | MAP_DIFFUSE | MAP_REFLECT)
, mSpecularMap()
, mAmbientMap()
, mDiffuseMap()
, mShader(0)
, mParamSpecular(0)
, mParamAmbient(0)
, mParamDiffuse(0)
, mParamCamera(0)
{
}

EnvironmentAmbientDiffuseMaterial::EnvironmentAmbientDiffuseMaterial(SceneGraph *scene, const char *ambientmap, const char* diffusemap, const char *specmap)
: Material(scene, MAP_AMBIENT | MAP_DIFFUSE | MAP_REFLECT)
, mShader(0)
, mParamSpecular(0)
, mParamAmbient(0)
, mParamDiffuse(0)
, mParamCamera(0)
{
   addTexture(mAmbientMap, ambientmap);
   addTexture(mDiffuseMap, diffusemap);
   addTexture(mSpecularMap, specmap);
}

EnvironmentAmbientDiffuseMaterial::~EnvironmentAmbientDiffuseMaterial()
{
}


void EnvironmentAmbientDiffuseMaterial::init()
{
   mShader= activeDevice->loadShader("environmentambientdiffuse-vert.glsl", "environmentambientdiffuse-frag.glsl");

   mParamSpecular= activeDevice->getParameterIndex("specularmap");
   mParamAmbient= activeDevice->getParameterIndex("ambientmap");
   mParamDiffuse= activeDevice->getParameterIndex("diffusemap");
   mParamCamera= activeDevice->getParameterIndex("camera");
}

void EnvironmentAmbientDiffuseMaterial::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mAmbientMap, getTextureSlot(0)->name());
   addTexture(mDiffuseMap, getTextureSlot(1)->name());
   addTexture(mSpecularMap, getTextureSlot(2)->name());
}


void EnvironmentAmbientDiffuseMaterial::addGeometry(Geometry *geo)
{
   VertexBuffer *vb= mPool->get(geo);
   if (!vb)
   {
      vb= mPool->add(geo);

      {
         Vector *vtx= geo->getVertices();
         Vector *nrm= geo->getNormals();
         UV *uv= geo->getUV(1);

         activeDevice->allocateVertexBuffer( vb->getVertexBuffer(), sizeof(Vertex)*geo->getVertexCount() );
         volatile Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );
         for (int i=0;i<geo->getVertexCount();i++)
         {
            dst[i].pos.x= vtx[i].x;
            dst[i].pos.y= vtx[i].y;
            dst[i].pos.z= vtx[i].z;

            dst[i].normal.x= nrm[i].x;
            dst[i].normal.y= nrm[i].y;
            dst[i].normal.z= nrm[i].z;

            dst[i].uv.u= uv[i].u;
            dst[i].uv.v= uv[i].v;
         }
         activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );
      }

      vb->setIndexBuffer( geo->getIndices(), geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}


void EnvironmentAmbientDiffuseMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glBindTexture(GL_TEXTURE_2D, mAmbientMap);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mDiffuseMap);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamAmbient, 0);
   activeDevice->bindSampler(mParamDiffuse, 1);
   activeDevice->bindSampler(mParamSpecular, 2);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void EnvironmentAmbientDiffuseMaterial::end()
{
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}


void EnvironmentAmbientDiffuseMaterial::renderDiffuse()
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
         glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector)*2) );

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
         glDrawElements( GL_TRIANGLES, vb->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

         activeDevice->pop();
      }
   }

   end();
}

void EnvironmentAmbientDiffuseMaterial::update(float /*frame*/, Node** /* nodelist */, const Matrix& cam )
{
   mCamera= cam;
}

