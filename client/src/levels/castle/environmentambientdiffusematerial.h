#pragma once

#include "materials/material.h"
#include "math/matrix.h"
#include "render/uv.h"

class EnvironmentAmbientDiffuseMaterial : public Material
{
public:
   struct Vertex {
      Vector pos;
      Vector normal;
      UV     uv;
   };

   EnvironmentAmbientDiffuseMaterial(SceneGraph *scene);
   EnvironmentAmbientDiffuseMaterial(SceneGraph *scene, const char *ambientmap, const char* diffusemap, const char *specmap);
   ~EnvironmentAmbientDiffuseMaterial();

   void update(float frame, Node **nodelist, const Matrix& cam);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   virtual void renderDiffuse();

private:
   void init();
   void begin();
   void end();

   Texture mSpecularMap;
   Texture mAmbientMap;
   Texture mDiffuseMap;
   unsigned int mShader;

   int mParamSpecular;
   int mParamAmbient;
   int mParamDiffuse;
   int mParamCamera;

   Matrix mCamera;
};
