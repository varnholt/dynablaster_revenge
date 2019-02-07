#pragma once

#include "material.h"
#include "math/matrix.h"
#include "render/uv.h"

class Camera;

class DestructionMaterial : public Material
{
public:
   struct Vertex {
      Vector pos;
      Vector normal;
      UV     uv;
   };

   DestructionMaterial(SceneGraph *scene);
   DestructionMaterial(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap, const char* shadowmap, Camera* shadowCam);
   virtual ~DestructionMaterial();

   void update(float frame, Node **nodelist, const Matrix& cam);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   virtual void renderDiffuse();

private:
   void init();
   void begin();
   void end();

   Texture mColorMap;
   Texture mDiffuseMap;
   Texture mSpecularMap;
   Texture mShadowMap;
   unsigned int mShader;

   int mParamSpecular;
   int mParamDiffuse;
   int mParamTexture;
   int mParamShadow;
   int mParamShadowCamera;

   Camera* mShadowCam;
};
