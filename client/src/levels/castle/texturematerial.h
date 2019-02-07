#pragma once

#include "materials/material.h"
#include "render/renderbuffer.h"
#include "render/uv.h"

class TextureMaterial : public Material
{
public:
   struct Vertex
   {
      Vector pos;
      Vector nrm;
      UV uv;
   };

   TextureMaterial(SceneGraph *scene);
   TextureMaterial(SceneGraph *scene, const char* texmap);

   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   void update(float frame, Node **nodelist, const Matrix& cam);

   virtual void renderDiffuse();

private:
   void init();
   void begin();
   void end();

   Texture mColorMap;
   unsigned int mShader;

   int mParamTexture;
};
