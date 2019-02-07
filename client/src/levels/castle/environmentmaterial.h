#pragma once

#include "materials/material.h"
#include "render/uv.h"
#include "math/matrix.h"

class EnvironmentMaterial : public Material
{
public:
   struct Vertex
   {
      Vector pos;
      Vector normal;
   };

   EnvironmentMaterial(SceneGraph *scene);
   EnvironmentMaterial(SceneGraph *scene, const char *specmap);
   ~EnvironmentMaterial();

   void init();
   void update(float frame, Node **nodelist, const Matrix& cam);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   void begin();
   void end();
   virtual void renderDiffuse();

private:
   Texture mSpecularMap;
   unsigned int mShader;

   int mParamSpecular;
   int mParamCamera;

   Matrix mCamera;
};
