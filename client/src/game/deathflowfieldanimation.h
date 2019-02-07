#pragma once

#include "tools/array.h"
#include "math/vector.h"
#include "math/vector4.h"

class Matrix;
class FrameBuffer;

class DeathFlowFieldAnimation
{
public:
   struct VertexParam
   {
      VertexParam() : fade(0.0f), offsetU(0.0f), offsetV(0.0f), unused(0.0f) {};
      float fade;
      float offsetU;
      float offsetV;
      float unused;
   };

   DeathFlowFieldAnimation();
   ~DeathFlowFieldAnimation();

   // initialize flowfield from 2d rendering
   void initialize( FrameBuffer* src, const Vector& min, const Vector& max );
   void initializePositions(unsigned int depthMap, const Vector& min, const Vector& max);
   void initializeParams(unsigned int depthMap, const Vector& min, const Vector& max);

   // center reference position (world space 3d)
   const Vector& getCenter() const;
   void setCenter(const Vector& center);

   // flowfield scale
   float getScale() const;
   void setScale(float scale);

   //! return size of particles in unprojected units
   float getPointSize() const;

   // update vertex position from flowfield directions
   void update(float deltaTime);

   // draw particles
   void draw();

   unsigned int getColorMap() const;

   bool isElapsed() const;


private:
   int                     mWidth;
   int                     mHeight;
   unsigned int            mVertexPosBuffer;
   unsigned int            mVertexUVBuffer;
   unsigned int            mVertexParams;
   unsigned int            mPositions[2];
   unsigned int            mTarget[2];
   unsigned int            mTexture;
   int                     mPage;

   Vector                  mCenter;
   float                   mFlowScale;
   float                   mParticleSize;

   float                   mElapsed;
};
