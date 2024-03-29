#pragma once

#include "material.h"
#include "math/matrix.h"
#include "render/uv.h"

class EnvironmentTextureMaterial : public Material
{
public:
   struct Vertex {
      Vector pos;
      Vector normal;
      UV     uv;
   };

   EnvironmentTextureMaterial(SceneGraph *scene);
   EnvironmentTextureMaterial(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap);
   virtual ~EnvironmentTextureMaterial();

   void updateGeometry(VertexBuffer *vb);
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
   unsigned int mShader;

   int mParamSpecular;
   int mParamDiffuse;
   int mParamTexture;
   int mParamCamera;

   Matrix mCamera;
};
