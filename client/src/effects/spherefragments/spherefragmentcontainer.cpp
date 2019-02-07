#include "spherefragmentcontainer.h"

// spherefragments
#include "spherefragment.h"

// Qt
#include <QGLBuffer>
#include <QGLShaderProgram>

// engine
#include "framework/globaltime.h"
#include "gldevice.h"
#include "nodes/mesh.h"
#include "renderdevice.h"
#include "render/texturepool.h"
#include "nodes/scenegraph.h"
#include "tools/filestream.h"
#include "math/vector.h"
#include "image/image.h"

// cmath
#include <math.h>

// postproduction
#include "postproduction/blurfilter.h"


SphereFragmentContainer::SphereFragmentContainer(Node* root)
  : mEarthTexture(0),
    mNormalMapTexture(0),
    mLavaMapTexture(0),
    mShader(0),
    mSizeParam(0),
    mProjectMatrixParam(0),
    mModelMatrixParam(0),
    mFresnelParam(0),
    mLightParam(0),
    mCameraParam(0),
    mTextureMapParam(0),
    mNormalMapParam(0),
    mSpecularMapParam(0),
    mLavaMapParam(0)
{
   // init texture
   TexturePool* pool= TexturePool::Instance();

   Image* image= new Image("earth_normalmap");
   image->buildNormalMap( 256 );  // strength of normal map
//   image->save("test.tga");

   mEarthTexture       = pool->getTexture("earth");
   mNormalMapTexture   = pool->getTexture(image);
   mLavaMapTexture     = pool->getTexture("bomb");

   delete image;

   // init shader
   mShader = activeDevice->loadShader(
      "spherefragments-vert.glsl",
      "spherefragments-frag.glsl"
   );

   mLightParam = activeDevice->getParameterIndex("lightPosition");
   mCameraParam = activeDevice->getParameterIndex("cameraPosition");
   mProjectMatrixParam = activeDevice->getParameterIndex("projection");

   mModelMatrixParam = activeDevice->getParameterIndex("transformations");
   mFresnelParam = activeDevice->getParameterIndex("fresnelFactors");

   mTextureMapParam = activeDevice->getParameterIndex("texturemap");
   mNormalMapParam = activeDevice->getParameterIndex("normalmap");
   mSpecularMapParam = activeDevice->getParameterIndex("specularmap");
   mLavaMapParam = activeDevice->getParameterIndex("lavamap");

   Image *order= new Image("order");

   Array<Mesh*> meshList;

   for (int i = 0; i < root->getChildCount(); i++)
   {
      Node* node = root->getChild(i);
      if (node->visible() && node->id() == Node::idMesh)
      {
         Mesh* mesh= (Mesh*)node;

         if (mesh->getPart(0)->getVertexCount() < 200)
         {
            meshList.add(mesh);

            if (meshList.size()>=16)
            {
               SphereFragment* fragment = new SphereFragment(meshList, order);
               mFragments << fragment;
               meshList.clear();
            }
         }
      }
   }

   if (meshList.size()>0)
   {
      SphereFragment* fragment = new SphereFragment(meshList, order);
      mFragments << fragment;
   }

   delete order;
}


SphereFragmentContainer::~SphereFragmentContainer()
{
   qDeleteAll(mFragments);
}


void SphereFragmentContainer::begin()
{
   // activate shader
   activeDevice->setShader(mShader);

   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, mEarthTexture.getTexture());
   activeDevice->bindSampler(mTextureMapParam, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mNormalMapTexture.getTexture());
   activeDevice->bindSampler(mNormalMapParam, 1);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mLavaMapTexture.getTexture());
   activeDevice->bindSampler(mLavaMapParam, 2);
}


void SphereFragmentContainer::animate(float time)
{
   // Matrix rotation = Matrix::rotateX(-0.41f) * Matrix::rotateY(time * 0.1745f);

   Matrix rotation;
   rotation = Matrix::rotateY(time * 0.1745f);   // rotate the earth around its own y axis
   rotation = rotation * Matrix::rotateX(-1.0f); // rotate the north pole towards the viewer
   rotation = rotation * Matrix::rotateY(1.0f);  // make earth rotate / instead of |

   time*=0.5f;

   foreach (SphereFragment* fragment, mFragments)
   {
      fragment->animate(time, rotation);
   }
}


void SphereFragmentContainer::drawFragments(const Vector& camPos)
{
   begin();

//   float t= GlobalTime::Instance()->getTime();
//   Vector lightPosition = Vector(cos(t*0.1) * 30, -10, sin(t*0.1) * 30);
//   Vector lightPosition = Vector(0, 0, -50);
   Vector lightPosition = Vector(10, -10, -30);

   Matrix proj;
   glGetFloatv(GL_PROJECTION_MATRIX, proj.data());

   activeDevice->setParameter(
      mLightParam,
      lightPosition
   );

   activeDevice->setParameter(
      mCameraParam,
      camPos
   );

   activeDevice->setParameter(
      mProjectMatrixParam,
      proj
   );

   foreach (SphereFragment* fragment, mFragments)
   {
      const int count= fragment->getPartCount();

      activeDevice->setParameter(
         mModelMatrixParam,
         fragment->getMatrices(),
         count
      );

      activeDevice->setParameter(
         mFresnelParam,
         fragment->getFresnelFactors(),
         count
      );

      fragment->draw();
   }

   end();
}


void SphereFragmentContainer::end()
{
   activeDevice->setShader(0);

   glActiveTexture(GL_TEXTURE3_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE2_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0_ARB);
}

