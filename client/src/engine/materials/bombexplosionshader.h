#ifndef BOMBEXPLOSIONSHADER_H
#define BOMBEXPLOSIONSHADER_H

#include "material.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "math/matrix.h"

class BombExplosionShader : public Material
{
public:
   struct Vertex
   {
      Vector pos;
      Vector normal;
      UV     uv;
   };

   BombExplosionShader(SceneGraph *scene);
   BombExplosionShader(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap);
   virtual ~BombExplosionShader();

   void update(float, Node **nodelist, const Matrix& cam);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   virtual void renderDiffuse();

private:
   void begin();
   void end();
   void init();

   Texture mColorMap;
   Texture mDiffuseMap;
   Texture mSpecularMap;
   unsigned int mShader;

   int mParamSpecular;
   int mParamDiffuse;
   int mParamTexture;
   int mParamCamera;

   Matrix mCamera;
};

#endif
