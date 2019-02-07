#ifndef EXTRAMAPPING_H
#define EXTRAMAPPING_H

#include "material.h"
#include "math/matrix.h"
#include "../render/renderbuffer.h"
#include "../render/uv.h"

class ExtraMapping : public Material
{
public:
   struct Vertex
   {
      Vector pos;
      UV uv;
   };

   ExtraMapping(SceneGraph *scene);
   ExtraMapping(SceneGraph *scene, const char *map);
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

   Matrix mCamera;
};

#endif
