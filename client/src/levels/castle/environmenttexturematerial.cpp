#include "stdafx.h"
#include "environmenttexturematerial.h"
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

EnvironmentTextureMaterial::EnvironmentTextureMaterial(SceneGraph *scene)
: Material(scene, MAP_DIFFUSE | MAP_REFLECT)
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

EnvironmentTextureMaterial::EnvironmentTextureMaterial(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap, const char *aomap)
: Material(scene, MAP_DIFFUSE | MAP_REFLECT)
{
   addTexture(mColorMap, colormap);
   addTexture(mDiffuseMap, envmap);
   addTexture(mSpecularMap, specmap, 1|2|4);
   addTexture(mAmbientMap, aomap);
}

EnvironmentTextureMaterial::~EnvironmentTextureMaterial()
{
}


void EnvironmentTextureMaterial::init()
{
   mShader= activeDevice->loadShader("environmenttexture-vert.glsl", "environmenttexture-frag.glsl");

   mParamSpecular= activeDevice->getParameterIndex("specularmap");
   mParamDiffuse= activeDevice->getParameterIndex("diffusemap");
   mParamTexture= activeDevice->getParameterIndex("texturemap");
   mParamAmbient= activeDevice->getParameterIndex("ambientmap");
   mParamCamera= activeDevice->getParameterIndex("camera");
}

void EnvironmentTextureMaterial::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
   addTexture(mDiffuseMap, "diffuse_level");
   addTexture(mSpecularMap, getTextureSlot(1)->name(), 1|2|4);
}


void EnvironmentTextureMaterial::updateGeometry(VertexBuffer *vb)
{
   Geometry *geo= vb->getGeometry();
   if (!geo)
      return;

   Vector *vtx= geo->getVertices();
   Vector *nrm= geo->getNormals();
   UV *uv= geo->getUV(1);


   Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer(vb->getVertexBuffer());
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
   activeDevice->unlockVertexBuffer(vb->getVertexBuffer());
}

void EnvironmentTextureMaterial::addGeometry(Geometry *geo)
{
   VertexBuffer *vb= mPool->get(geo);
   if (!vb)
   {
      vb= mPool->add(geo);

      bool dynamic= geo->getParent()->hasSkinning();
      activeDevice->allocateVertexBuffer(vb->getVertexBuffer(), sizeof(Vertex)*geo->getVertexCount(), dynamic);
      updateGeometry(vb);

      activeDevice->allocateIndexBuffer( vb->getIndexBuffer(), geo->getIndexCount()*sizeof(unsigned short) );
      unsigned short *src= geo->getIndices();
      volatile unsigned short *dst= (unsigned short*)activeDevice->lockIndexBuffer( vb->getIndexBuffer() );
      for (int i=0;i<geo->getIndexCount();i++)
         dst[i]= src[i];
      activeDevice->unlockIndexBuffer( vb->getIndexBuffer() );

      vb->setIndexCount( geo->getIndexCount() );
   }

   mVB.add(Material::Buffer(geo,vb));
}


void EnvironmentTextureMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mDiffuseMap);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);

   glActiveTexture(GL_TEXTURE3_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mAmbientMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamSpecular, 0);
   activeDevice->bindSampler(mParamDiffuse, 1);
   activeDevice->bindSampler(mParamTexture, 2);
   activeDevice->bindSampler(mParamAmbient, 3);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void EnvironmentTextureMaterial::end()
{
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

   glActiveTexture(GL_TEXTURE3);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}


void EnvironmentTextureMaterial::renderDiffuse()
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

void EnvironmentTextureMaterial::update(float /*frame*/, Node** /* nodelist */, const Matrix& cam )
{
//   Material::update(frame, nodelist, cam);

   mCamera= cam;
/*
   for (int j=0;j<mVB.size();j++)
   {
      const Material::Buffer& buffer= mVB[j];
      VertexBuffer *vb= buffer.vb;
      Geometry *geo= buffer.geo;

      if (geo->getBoneCount())
      {
         Mesh *mesh= (Mesh*)geo->getParent();
         if (mesh)
         {
            MotionMixer *mixer= mesh->getMotionMixer();
            if (mixer)
            {
               double t1= getCpuTick();

               Vector *vtemp= geo->getSkinVertices();
               Vector *ntemp= geo->getSkinNormals();
               UV* uv= geo->getUV(1);
               volatile Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );
               for (int i=0;i<geo->getVertexCount();i++)
               {
                  dst[i].pos.x= vtemp[i].x;
                  dst[i].pos.y= vtemp[i].y;
                  dst[i].pos.z= vtemp[i].z;
                  dst[i].normal.x= ntemp[i].x;
                  dst[i].normal.y= ntemp[i].y;
                  dst[i].normal.z= ntemp[i].z;
                  dst[i].uv.u= uv[i].u;
                  dst[i].uv.v= uv[i].v;
               }
               activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );

               double t2= getCpuTick();
            }
         }
      }
   }
*/
}
