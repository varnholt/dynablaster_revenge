#ifndef DISPLACEMENTMATERIAL_H
#define DISPLACEMENTMATERIAL_H

#include "material.h"
#include "math/matrix.h"
#include "../render/renderbuffer.h"
#include "../render/uv.h"

class DisplacementMaterial : public Material
{
public:
   struct Vertex
   {
      Vector pos;
      UV uv;
   };

   DisplacementMaterial(SceneGraph *scene);
   DisplacementMaterial(SceneGraph *scene, const char *map, const char* diffusemap);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   virtual void renderDiffuse();
   void update(float frame, Node **nodelist, const Matrix& cam);

private:
   void begin();
   void end();
   void init();

   Texture mColorMap;
   Texture mDiffuseMap;
   unsigned int mShader;

   int mParamTexture;
   int mParamDiffuse;
   int mParamTime;

   Matrix mCamera;
};

#endif
