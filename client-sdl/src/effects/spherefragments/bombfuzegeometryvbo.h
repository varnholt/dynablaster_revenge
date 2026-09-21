#pragma once

// base
#include "geometryvbo.h"

// engine
#include "render/texture.h"

class Geometry;
class Vector4;

class BombFuzeGeometryVbo : public GeometryVbo
{
public:
   BombFuzeGeometryVbo(Geometry* geo);

   virtual void initialize();

   void draw(const Vector4& color);

   void initGlParameters();

   void cleanupGlParameter();

protected:
   //! sphere texture
   Texture mTexture;

   //! shader
   unsigned int mShader;
   int mFresnel;
   int mColorParam;
};
