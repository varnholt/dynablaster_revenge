#include "environmentambientmaterial.h"
#include "animation/motionmixer.h"
#include "gldevice.h"
#include "image/image.h"
#include "nodes/mesh.h"
#include "render/renderbuffer.h"
#include "render/texturepool.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "textureslot.h"
#include "tools/profiling.h"
#include "tools/stream.h"

EnvironmentAmbientMaterial::EnvironmentAmbientMaterial(SceneGraph* scene)
    : Material(scene, MAP_AMBIENT | MAP_REFLECT), mSpecularMap(0), mShader(0), mParamSpecular(0), mParamAmbient(0), mParamCamera(0)
{
}

EnvironmentAmbientMaterial::EnvironmentAmbientMaterial(SceneGraph* scene, const char* ambientmap, const char* specmap)
    : Material(scene, MAP_AMBIENT | MAP_REFLECT)
{
   addTexture(mAmbientMap, ambientmap);
   addTexture(mSpecularMap, specmap);
}

EnvironmentAmbientMaterial::~EnvironmentAmbientMaterial()
{
}

void EnvironmentAmbientMaterial::init()
{
   mShader = activeDevice->loadShader("environmentambient-vert.glsl", "environmentambient-frag.glsl");

   mParamSpecular = activeDevice->getParameterIndex("specularmap");
   mParamAmbient = activeDevice->getParameterIndex("ambientmap");
   mParamCamera = activeDevice->getParameterIndex("camera");
}

void EnvironmentAmbientMaterial::load(Stream* stream)
{
   Material::load(stream);

   addTexture(mAmbientMap, getTextureSlot(0)->name());
   addTexture(mSpecularMap, getTextureSlot(1)->name());
}

void EnvironmentAmbientMaterial::addGeometry(Geometry* geo)
{
   VertexBuffer* vb = mPool->get(geo);
   if (!vb)
   {
      vb = mPool->add(geo);

      {
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
      }

      vb->setIndexBuffer(geo->getIndices(), geo->getIndexCount());
   }

   mVB.add(Material::Buffer(geo, vb));
}

void EnvironmentAmbientMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glBindTexture(GL_TEXTURE_2D, mAmbientMap);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   activeDevice->setShader(mShader);
   activeDevice->bindSampler(mParamAmbient, 0);
   activeDevice->bindSampler(mParamSpecular, 1);

   // enable required vertex arrays
   glEnableVertexAttribArray(0);  // vertex data
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);
}

void EnvironmentAmbientMaterial::end()
{
   glDisableVertexAttribArray(2);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}

void EnvironmentAmbientMaterial::renderDiffuse()
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
         Matrix invView = (geo->getTransform() * mCamera).invert();
         Vector osCam = invView.translation();
         //         Vector osCam= geo->getParent()->getWorld2Obj() * camPos;
         activeDevice->setParameter(mParamCamera, osCam);

         //         if (geo->getBoneCount()==0)
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

void EnvironmentAmbientMaterial::update(float /*frame*/, Node** /* nodelist */, const Matrix& cam)
{
   mCamera = cam;
   /*
      for (int j=0;j<mVB.size();j++)
      {
         const Material::Buffer& buffer= mVB[j];
         VertexBuffer *vb= buffer.vb;
         Geometry *geo= buffer.geo;

         if (geo->getBoneCount())
         {
            double t1= getCpuTick();

            Mesh *mesh= (Mesh*)geo->getParent();
            if (mesh)
            {
               MotionMixer *mixer= mesh->getMotionMixer();
               if (mixer)
               {
                  Vector *vtemp= geo->getSkinVertices();
                  Vector *ntemp= geo->getSkinNormals();
                  UV* uv= geo->getUV(1);
                  Vertex *dst= (Vertex*)activeDevice->lockVertexBuffer( vb->getVertexBuffer() );
                  for (int i=0;i<geo->getVertexCount();i++)
                  {
                     dst[i].pos= vtemp[i];
                     dst[i].normal= ntemp[i];
                     dst[i].uv= uv[i];
                  }
                  activeDevice->unlockVertexBuffer( vb->getVertexBuffer() );
               }
            }
            double t2= getCpuTick();
         }
      }
   */
}
