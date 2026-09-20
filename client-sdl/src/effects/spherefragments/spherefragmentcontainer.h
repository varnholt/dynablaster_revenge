#pragma once

// engine
#include "math/vector.h"
#include "render/texture.h"
#include "tools/array.h"

// forwards
class SphereFragment;
class Node;

class SphereFragmentContainer
{
public:

   SphereFragmentContainer(Node* scene);
   virtual ~SphereFragmentContainer();

   void animate(float time);

   //! draw fragments
   void drawFragments(const Vector& camPos);

   void begin();
   void end();


protected:

   Array<SphereFragment*> mFragments;

   // textures

   Texture mEarthTexture;
   Texture mNormalMapTexture;
   Texture mLavaMapTexture;

   // shader

   //! shader
   unsigned int mShader;

   //! intensity uniform
   int mSizeParam;

   int mProjectMatrixParam;
   int mModelMatrixParam;
   int mFresnelParam;

   //! light
   int mLightParam;
   int mCameraParam;

   //! mapping parameters
   int mTextureMapParam;
   int mNormalMapParam;
   int mSpecularMapParam;
   int mLavaMapParam;

};
