#pragma once

#include "material.h"
#include "math/matrix.h"
#include "render/uv.h"
#include "image/psd.h"

class Camera;

class SkullMaterial : public Material
{
public:
   struct Vertex {
      Vector pos;
      Vector normal;
      UV     uv;
   };

   SkullMaterial(SceneGraph *scene);
   SkullMaterial(SceneGraph *scene, const char *colormap, const char* diffusemap, const char* specmap, const char* shadowMap, Camera* shadowCam);
   ~SkullMaterial();

   void update(float frame, Node **nodelist, const Matrix& cam);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   virtual void renderDiffuse();

private:
   void putImage(Image& target, int posX, int posY, const Image& source);
   void putScanline(unsigned int *dst, unsigned int *src, int width);

   void init();
   void begin();
   void end();

   Texture mDiffuseMap;
   Texture mColorMap;
   Texture mShadowMap;
   Texture mSpecularMap;
   unsigned int mShader;

   int mParamDiffuse;
   int mParamTexture;
   int mParamAmbient;
   int mParamShadow;
   int mParamSpecular;
   int mParamCamera;
   int mParamShadowCamera;
   int mParamOffset;

   Camera* mShadowCam;
   Matrix mCamera;
};
