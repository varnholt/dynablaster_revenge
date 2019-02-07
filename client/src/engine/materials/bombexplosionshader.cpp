#include "stdafx.h"
#include "bombexplosionshader.h"
#include "tools/stream.h"
#include "tools/profiling.h"
#include "image/image.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"

BombExplosionShader::BombExplosionShader(SceneGraph *scene)
: Material(scene, -2)
, mColorMap(0)
, mDiffuseMap(0)
, mSpecularMap(0)
, mShader(0)
, mParamSpecular(0)
, mParamDiffuse(0)
, mParamTexture(0)
, mParamCamera(0)
{
}

BombExplosionShader::BombExplosionShader(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap)
: Material(scene, -2)
{
   addTexture(mColorMap, colormap);
   addTexture(mDiffuseMap, envmap);
   addTexture(mSpecularMap, specmap);
}


BombExplosionShader::~BombExplosionShader()
{
}


void BombExplosionShader::init()
{
   mShader= activeDevice->loadShader("environmentmapping-vert.glsl", "environmentmapping-frag.glsl");

   mParamSpecular= activeDevice->getParameterIndex("specularmap");
   mParamDiffuse= activeDevice->getParameterIndex("diffusemap");
   mParamTexture= activeDevice->getParameterIndex("texturemap");
   mParamCamera= activeDevice->getParameterIndex("camera");
}


void BombExplosionShader::load(Stream *stream)
{
   Material::load(stream);
}


void BombExplosionShader::addGeometry(Geometry *geo)
{
   VertexBuffer *vb= mPool->get(geo);
   if (!vb)
   {
      vb= mPool->add(geo);

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

      vb->setIndexBuffer( geo->getIndices(), geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}


void BombExplosionShader::begin()
{
   Material::begin();

   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mDiffuseMap);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamSpecular, 0);
   activeDevice->bindSampler(mParamDiffuse, 1);
   activeDevice->bindSampler(mParamTexture, 2);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}


void BombExplosionShader::end()
{
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}


void BombExplosionShader::renderDiffuse()
{
   int count=0;

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
         Matrix invView= (geo->getTransform() * mCamera).invert();
         Vector osCam= invView.translation();
         activeDevice->setParameter(mParamCamera, osCam);

         activeDevice->push(geo->getTransform());

         // draw mesh
         glBindBuffer( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
         glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
         glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)sizeof(Vector) );  
         glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector)*2) );  

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
         glDrawElements( GL_TRIANGLES, vb->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

         activeDevice->pop();

         count++;
      }
   }

   end();
}

void BombExplosionShader::update(float, Node**, const Matrix& cam)
{
   mCamera= cam;
}
