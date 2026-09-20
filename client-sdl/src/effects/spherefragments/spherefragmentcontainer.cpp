#include "spherefragmentcontainer.h"

// spherefragments
#include "spherefragment.h"

// engine
#include "gldevice.h"
#include "nodes/mesh.h"
#include "nodes/node.h"
#include "render/texturepool.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "image/image.h"

// cmath
#include <math.h>


SphereFragmentContainer::SphereFragmentContainer(Node* root)
  : mEarthTexture(0),
    mNormalMapTexture(0),
    mLavaMapTexture(0),
    mShader(0),
    mSizeParam(-1),
    mProjectMatrixParam(-1),
    mModelMatrixParam(-1),
    mFresnelParam(-1),
    mLightParam(-1),
    mCameraParam(-1),
    mTextureMapParam(-1),
    mNormalMapParam(-1),
    mSpecularMapParam(-1),
    mLavaMapParam(-1)
{
   // init texture
   TexturePool* pool= TexturePool::Instance();

   Image* image= new Image("earth_normalmap");
   image->buildNormalMap( 256 );  // strength of normal map

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
               mFragments.add(fragment);
               meshList.clear();
            }
         }
      }
   }

   if (meshList.size()>0)
   {
      SphereFragment* fragment = new SphereFragment(meshList, order);
      mFragments.add(fragment);
   }

   delete order;
}


SphereFragmentContainer::~SphereFragmentContainer()
{
   for (int i=0; i<mFragments.size(); i++)
      delete mFragments[i];
}


void SphereFragmentContainer::begin()
{
   // activate shader
   activeDevice->setShader(mShader);

   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, mEarthTexture.getTexture());
   activeDevice->bindSampler(mTextureMapParam, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glBindTexture(GL_TEXTURE_2D, mNormalMapTexture.getTexture());
   activeDevice->bindSampler(mNormalMapParam, 1);

   glActiveTexture(GL_TEXTURE2_ARB);
   glBindTexture(GL_TEXTURE_2D, mLavaMapTexture.getTexture());
   activeDevice->bindSampler(mLavaMapParam, 2);
}


void SphereFragmentContainer::animate(float time)
{
   Matrix rotation;
   rotation = Matrix::rotateY(time * 0.1745f);   // rotate the earth around its own y axis
   rotation = rotation * Matrix::rotateX(-1.0f); // rotate the north pole towards the viewer
   rotation = rotation * Matrix::rotateY(1.0f);  // make earth rotate / instead of |

   time*=0.5f;

   for (int i=0; i<mFragments.size(); i++)
      mFragments[i]->animate(time, rotation);
}


void SphereFragmentContainer::drawFragments(const Vector& camPos)
{
   begin();

   Vector lightPosition = Vector(10, -10, -30);

   Matrix proj = static_cast<GLDevice*>(activeDevice)->getProjectionMatrix();

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

   for (int i=0; i<mFragments.size(); i++)
   {
      SphereFragment* fragment= mFragments[i];
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

   glActiveTexture(GL_TEXTURE0_ARB);
}
