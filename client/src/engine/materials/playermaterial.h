#pragma once

#include "playermaterialbase.h"
#include "math/matrix.h"
#include "render/uv.h"

class PlayerMaterial : public PlayerMaterialBase
{
public:
   PlayerMaterial(SceneGraph *scene);
   PlayerMaterial(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap, const char *aomap=0);
   virtual ~PlayerMaterial();

   void exportOBJ(Stream* stream, int& indexOffset);

   void load(Stream *stream);
   virtual void renderDiffuse();

   //! colormap setter
   void setColorMap(const Texture& colorMap);

private:
   void init();
   void begin();
   void end();

   Texture mColorMap;
   Texture mDiffuseMap;
   Texture mSpecularMap;
   Texture mAmbientMap;
   unsigned int mShader;

   int mParamSpecular;
   int mParamDiffuse;
   int mParamTexture;
   int mParamAmbient;
   int mParamCamera;
   int mParamBones;
   int mParamFlash;
   int mParamFade;
};

