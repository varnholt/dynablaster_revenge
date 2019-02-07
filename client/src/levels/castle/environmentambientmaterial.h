#pragma once

#include "materials/material.h"
#include "math/matrix.h"
#include "render/uv.h"

class EnvironmentAmbientMaterial : public Material
{
public:
   struct Vertex {
      Vector pos;
      Vector normal;
      UV     uv;
   };

   EnvironmentAmbientMaterial(SceneGraph *scene);
   EnvironmentAmbientMaterial(SceneGraph *scene, const char *ambientmap, const char *specmap);
   virtual ~EnvironmentAmbientMaterial();

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
   unsigned int mShader;

   int mParamSpecular;
   int mParamAmbient;
   int mParamCamera;

   Matrix mCamera;
};
