#include "stdafx.h"
#include "texturematerial.h"
#include "materials/textureslot.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/geometry.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "image/image.h"
#include "tools/stream.h"
#include "tools/profiling.h"
#include "materials/textureslot.h"

TextureMaterial::TextureMaterial(SceneGraph *scene)
: Material(scene, MAP_DIFFUSE),
  mColorMap(0)
{
}

TextureMaterial::TextureMaterial(SceneGraph *scene, const char* texmap)
   : Material(scene, MAP_DIFFUSE),
   mColorMap(0)
{
   addTexture(mColorMap, texmap);
}

void TextureMaterial::load(Stream *stream)
{
   Material::load(stream);

   if (getTextureSlot(0))
      addTexture(mColorMap, getTextureSlot(0)->name());
}

void TextureMaterial::init()
{
   mShader= activeDevice->loadShader("texturemapping-vert.glsl", "texturemapping-frag.glsl");
   mParamTexture= activeDevice->getParameterIndex("texturemap");
}


void TextureMaterial::addGeometry(Geometry *geo)
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
         dst[i].nrm.x= nrm[i].x;
         dst[i].nrm.y= nrm[i].y;
         dst[i].nrm.z= nrm[i].z;
         if (uv)
         {
            dst[i].uv.u= uv[i].u;
            dst[i].uv.v= uv[i].v;
         }
      }
      activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );

      vb->setIndexBuffer( geo->getIndices(), geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}


void TextureMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   activeDevice->setShader( mShader );

   glBindTexture(GL_TEXTURE_2D, mColorMap);
   activeDevice->bindSampler(mParamTexture, 0);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}


void TextureMaterial::end()
{
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data

   activeDevice->setShader(0);
}

void TextureMaterial::renderDiffuse()
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

         if (!geo->getBoneCount())
         {
            const Matrix& tm= geo->getTransform();
            activeDevice->push(tm);
         }

         // draw mesh
         glBindBuffer( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
         glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
         glNormalPointer( GL_FLOAT, sizeof(Vertex), (GLvoid*)sizeof(Vector) );  
         glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector)*2) );  

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );
         glDrawElements( GL_TRIANGLES, vb->getIndexCount(), GL_UNSIGNED_SHORT, 0 ); // render

         if (!geo->getBoneCount())
            activeDevice->pop();

         count++;
      }
   }

   end();
}

void TextureMaterial::update(float /*frame*/, Node ** /*nodelist*/, const Matrix&)
{
}
