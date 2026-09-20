#pragma once

#include "playermaterialbase.h"
#include "math/matrix.h"
#include "render/uv.h"

class InvisibilityMaterial : public PlayerMaterialBase
{
public:
   InvisibilityMaterial(SceneGraph *scene);
   virtual ~InvisibilityMaterial();

   void load(Stream *stream);
   virtual void renderDiffuse();

   void setTexture(unsigned int texture);

private:
   void init();
   void begin();
   void end();

   unsigned int mShader;

   unsigned int mTextureMap;
   Texture mGradientMap;

   int mParamTexture;
   int mParamGradient;
   int mParamFadeThreshold;
   int mParamCamera;
   int mParamBones;
   int mParamFlash;
};

