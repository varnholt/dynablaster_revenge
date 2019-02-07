#pragma once

// engine
#include "math/vector.h"
#include "render/texture.h"

class EarthBackground
{
public:
   struct Vertex
   {
      float x,y,z;
      float tx,ty,tz;
      float u,v;
   };

   EarthBackground();

   void animate(float time);

   //! draw fragments
   void draw(const Vector& camPos);

   void begin();
   void end();

private:
   void drawEarth(const Vector& camPos);


   //! shader
   unsigned int mEarthShader;
   unsigned int mAuraShader;

   // textures
   Texture mEarthTexture;
   Texture mNormalMapTexture;

   // vbo
   unsigned int mVertexBuffer;
   unsigned int mIndexBuffer;
   int          mIndexCount;

   //! light
   int mLightParam;
   int mCameraParam;
   int mCloudMoveParam;

   //!
   int mTextureMapParam;
   int mNormalMapParam;
   int mSpecularMapParam;
   int mCloudMapParam;
};
