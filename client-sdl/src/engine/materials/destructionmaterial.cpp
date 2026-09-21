#include "destructionmaterial.h"
#include "animation/motionmixer.h"
#include "framework/gldevice.h"
#include "image/image.h"
#include "nodes/camera.h"
#include "nodes/mesh.h"
#include "render/renderbuffer.h"
#include "render/texturepool.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "textureslot.h"
#include "tools/profiling.h"
#include "tools/stream.h"

#include <math.h>

DestructionMaterial::DestructionMaterial(SceneGraph* scene)
    : Material(scene, MAP_DIFFUSE | MAP_REFLECT),
      mColorMap(0),
      mDiffuseMap(0),
      mSpecularMap(0),
      mShader(0),
      mParamSpecular(0),
      mParamDiffuse(0),
      mParamTexture(0),
      mParamShadow(0),
      mShadowCam(0)
{
}

DestructionMaterial::DestructionMaterial(
   SceneGraph* scene,
   const char* colormap,
   const char* envmap,
   const char* specmap,
   const char* shadowmap,
   Camera* shadowCam
)
    : Material(scene, MAP_DIFFUSE | MAP_REFLECT),
      mColorMap(0),
      mDiffuseMap(0),
      mSpecularMap(0),
      mShader(0),
      mParamSpecular(0),
      mParamDiffuse(0),
      mParamTexture(0),
      mParamShadow(0),
      mShadowCam(shadowCam)
{
   addTexture(mColorMap, colormap);
   addTexture(mDiffuseMap, envmap);
   addTexture(mSpecularMap, specmap);
   addTexture(mShadowMap, shadowmap);
}

DestructionMaterial::~DestructionMaterial()
{
}

void DestructionMaterial::init()
{
   mShader = activeDevice->loadShader("destruction-vert.glsl", "destruction-frag.glsl");

   mParamSpecular = activeDevice->getParameterIndex("specularmap");
   mParamDiffuse = activeDevice->getParameterIndex("diffusemap");
   mParamTexture = activeDevice->getParameterIndex("texturemap");
   mParamShadow = activeDevice->getParameterIndex("shadowmap");
   mParamShadowCamera = activeDevice->getParameterIndex("shadowCamera");
}

void DestructionMaterial::load(Stream* stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
   addTexture(mDiffuseMap, "diffuse_level");
   addTexture(mSpecularMap, getTextureSlot(1)->name());
}

void DestructionMaterial::addGeometry(Geometry* geo)
{
   VertexBuffer* vb = mPool->get(geo);
   if (!vb)
   {
      vb = mPool->add(geo);

      Vector* vtx = geo->getVertices();
      Vector* nrm = geo->getNormals();
      UV* uv = geo->getUV(1);

      activeDevice->allocateVertexBuffer(vb->getVertexBuffer(), sizeof(Vertex) * geo->getVertexCount());
      volatile Vertex* dst = (Vertex*)activeDevice->lockVertexBuffer(vb->getVertexBuffer());
      for (int i = 0; i < geo->getVertexCount(); i++)
      {
         dst[i].pos.x = vtx[i].x;
         dst[i].pos.y = vtx[i].y;
         dst[i].pos.z = vtx[i].z;

         dst[i].normal.x = nrm[i].x;
         dst[i].normal.y = nrm[i].y;
         dst[i].normal.z = nrm[i].z;

         dst[i].uv.u = uv[i].u;
         dst[i].uv.v = uv[i].v;
      }
      activeDevice->unlockVertexBuffer(vb->getVertexBuffer());

      vb->setIndexBuffer(geo->getIndices(), geo->getIndexCount());
   }

   mVB.add(Material::Buffer(geo, vb));
}

void DestructionMaterial::begin()
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
   glBindTexture(GL_TEXTURE_2D, mShadowMap);

   activeDevice->setShader(mShader);
   activeDevice->bindSampler(mParamSpecular, 0);
   activeDevice->bindSampler(mParamDiffuse, 1);
   activeDevice->bindSampler(mParamTexture, 2);
   activeDevice->bindSampler(mParamShadow, 3);

   // enable required vertex arrays
   glEnableVertexAttribArray(0);  // vertex data
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);

   Matrix cam;
   if (mShadowCam)
   {
      GLDevice* device = static_cast<GLDevice*>(activeDevice);
      device->pushProjection();

      cam = mShadowCam->getTransform().getView();
      float fov = mShadowCam->getFOV();
      fov = tan(fov * 0.5) * 0.75;
      float zNear = mShadowCam->getNear();
      float zFar = mShadowCam->getFar();

      activeDevice->setCamera(cam, fov, zNear, zFar, mShadowCam->getPerspectiveMode());

      cam = device->getProjectionMatrix();
      cam.normalizeZ();
      device->popProjection();
   }
   activeDevice->setParameter(mParamShadowCamera, cam);
}

void DestructionMaterial::end()
{
   glDisableVertexAttribArray(0);  // vertex data
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(1);

   glActiveTexture(GL_TEXTURE3_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}

void DestructionMaterial::renderDiffuse()
{
   begin();

   /*
      Matrix projMat;
      glGetFloatv(GL_PROJECTION_MATRIX, projMat.data());
      projMat= projMat.invert();
      Vector camPos= projMat.translation();
   */

   for (int i = 0; i < mVB.size(); i++)
   {
      // get vertex buffer
      VertexBuffer* vb = mVB[i].vb;
      Geometry* geo = mVB[i].geo;

      if (geo->isVisible())
      {
         //         Matrix invView= (geo->getTransform() * mCamera).invert();
         //         Vector osCam;//= invView.translation();

         //         Vector osCam= geo->getParent()->getWorld2Obj() * camPos;

         activeDevice->push(geo->getTransform());

         // draw mesh
         glBindBuffer(GL_ARRAY_BUFFER, vb->getVertexBuffer());
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(Vector));
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vector) * 2));

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb->getIndexBuffer());
         glDrawElements(GL_TRIANGLES, vb->getIndexCount(), GL_UNSIGNED_SHORT, 0);  // render

         activeDevice->pop();
      }
   }

   end();
}

void DestructionMaterial::update(float /*frame*/, Node** /* nodelist */, const Matrix&)
{
}
