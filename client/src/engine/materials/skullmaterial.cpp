#include "stdafx.h"
#include "skullmaterial.h"
#include "textureslot.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "image/image.h"
#include "image/psd.h"
#include "tools/stream.h"
#include "tools/profiling.h"
#include "nodes/camera.h"
#include "image/imagepool.h"
#include "animation/motionmixer.h"

#include <math.h>

/*
 expected block neighbouring information (in mesh::mRenderFlags)

 top left      1
 top center    2   * top
 top right     4
 mid left      8   * left
 mid center    16            <-- this is the mesh itself (ignore)
 mid right     32  * right
 bot left      64
 bot center    128 * down
 bot right     256

 the main directions make up 2^4=16 possible combinations stored as a 4x4 matrix in the texture
*/

SkullMaterial::SkullMaterial(SceneGraph *scene)
: Material(scene, MAP_DIFFUSE | MAP_REFLECT)
, mDiffuseMap(0)
, mColorMap(0)
, mShadowMap(0)
, mSpecularMap(0)
, mShader(0)
, mParamDiffuse(0)
, mParamTexture(0)
, mParamShadow(0)
, mParamSpecular(0)
, mParamCamera(0)
, mParamShadowCamera(0)
, mParamOffset(0)
, mShadowCam(0)
{
}

SkullMaterial::SkullMaterial(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap, const char* shadowMap, Camera* shadowCam)
: Material(scene, MAP_DIFFUSE | MAP_REFLECT)
, mDiffuseMap(0)
, mColorMap(0)
, mShadowMap(0)
, mSpecularMap(0)
, mShader(0)
, mParamDiffuse(0)
, mParamTexture(0)
, mParamShadow(0)
, mParamSpecular(0)
, mParamCamera(0)
, mParamShadowCamera(0)
, mParamOffset(0)
, mShadowCam(shadowCam)
{
   addTexture(mSpecularMap, specmap);
   addTexture(mDiffuseMap, envmap);
   addTexture(mColorMap, colormap);
   addTexture(mShadowMap, shadowMap);
}


SkullMaterial::~SkullMaterial()
{
}

void SkullMaterial::putScanline(unsigned int *dst, unsigned int *src, int width)
{
   // replicate first and last pixel
   dst[0]= src[0];
   for (int x=0; x<width; x++)
   {
      dst[x+1]= src[x];
   }
   dst[width+1]= src[width-1];
}


void SkullMaterial::putImage(Image& target, int posX, int posY, const Image& source)
{
   int width= source.getWidth();
   int height= source.getHeight();

   unsigned int *dst;
   unsigned int *src;

   dst= target.getScanline(posY) + posX;
   src= source.getScanline(0);
   
   putScanline(dst, src, width);

   for (int y=0; y<height; y++)
   {
      dst= target.getScanline(y + posY + 1) + posX;
      src= source.getScanline(y);
      putScanline(dst, src, width);
   }

   dst= target.getScanline(posY + height + 1) + posX;
   src= source.getScanline(height-1);
   putScanline(dst, src, width);
}

void SkullMaterial::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
   addTexture(mSpecularMap, getTextureSlot(1)->name());

   init();
}


void SkullMaterial::init()
{
   mShader= activeDevice->loadShader("skullmaterial-vert.glsl", "skullmaterial-frag.glsl");

   mParamSpecular= activeDevice->getParameterIndex("specularmap");
   mParamDiffuse= activeDevice->getParameterIndex("diffusemap");
   mParamShadow= activeDevice->getParameterIndex("shadowmap");
   mParamTexture= activeDevice->getParameterIndex("texturemap");

   mParamCamera= activeDevice->getParameterIndex("camera");
   mParamShadowCamera= activeDevice->getParameterIndex("shadowCamera");
   mParamOffset= activeDevice->getParameterIndex("uvOffset");

}


void SkullMaterial::addGeometry(Geometry *geo)
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


void SkullMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glBindTexture(GL_TEXTURE_2D, mDiffuseMap);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mShadowMap);

   glActiveTexture(GL_TEXTURE3_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamDiffuse, 0);
   activeDevice->bindSampler(mParamTexture, 1);
   activeDevice->bindSampler(mParamShadow, 2);
   activeDevice->bindSampler(mParamSpecular, 3);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   Matrix cam;
   if (mShadowCam)
   {
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();

      cam= mShadowCam->getTransform().getView();
      float fov= mShadowCam->getFOV();
      fov= tan(fov*0.5)*0.75;
      float zNear= mShadowCam->getNear();
      float zFar= mShadowCam->getFar();

      activeDevice->setCamera(cam, fov, zNear, zFar, mShadowCam->getPerspectiveMode());
      glMatrixMode(GL_PROJECTION);

      glGetFloatv(GL_PROJECTION_MATRIX, cam.data());
      cam.normalizeZ();
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
   }
   activeDevice->setParameter(mParamShadowCamera, cam);

}

void SkullMaterial::end()
{
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

   glActiveTexture(GL_TEXTURE4);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE3);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}


void SkullMaterial::renderDiffuse()
{
   begin();

   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      VertexBuffer *vb= mVB[i].vb;
      Geometry *geo= mVB[i].geo;

      if (geo->isVisible())
      {
         Mesh *mesh= (Mesh*)geo->getParent();

         int top= mesh->getRenderFlags() >> 1 & 1;
         int left= mesh->getRenderFlags() >> 3 & 1;
         int right= mesh->getRenderFlags() >> 5 & 1;
         int bottom= mesh->getRenderFlags() >> 7 & 1;
         int flags= (top)|(left<<1)|(right<<2)|(bottom<<3);
         int x= flags & 3;
         int y= (flags >> 2) & 3;

         Matrix invView= (geo->getTransform() * mCamera).invert();
         Vector osCam= invView.translation();
//         Vector osCam= geo->getParent()->getWorld2Obj() * camPos;
         activeDevice->setParameter(mParamCamera, osCam);
         activeDevice->setParameter(mParamOffset, Vector(x,y));

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

//   printf("blocks [%d]: %f \n", mVB.size(), (t2-t1)/1000000.0);
}

void SkullMaterial::update(float /*frame*/, Node** /* nodelist */, const Matrix& cam )
{
   mCamera= cam;
}

