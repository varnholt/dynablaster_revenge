#ifndef SHADOWBILLBOARD_H
#define SHADOWBILLBOARD_H

#include "material.h"
#include "math/vector2.h"
#include "../render/renderbuffer.h"
#include "../render/uv.h"

#include <QMap>

class ShadowBillboard : public Material
{
public:
   struct Vertex
   {
      Vector pos;
      UV uv;
   };

   struct Bounding
   {
      Vector min;
      Vector max;
   };

   ShadowBillboard(SceneGraph *scene);
   ShadowBillboard(SceneGraph *scene, const char *map);
   void load(Stream *stream);
   void addGeometry(Geometry *geo);
   virtual void renderDiffuse();
   void removeMesh(Mesh *mesh);
   void setOffset(float x, float y);

private:
   void init();
   void begin();
   void end();

   Texture mColorMap;
   unsigned int mShader;
   unsigned int mVertices;
   unsigned int mTexcoords;
   unsigned int mIndices;

   int mParamTexture;
   QMap<Geometry*,Bounding> mInstances;
   Vector2 mOffset;
};

#endif
