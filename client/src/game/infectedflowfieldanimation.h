#pragma once

#include "tools/array.h"
#include "math/vector.h"
#include "math/vector4.h"

class Matrix;
class FrameBuffer;

class InfectedFlowFieldAnimation
{
public:
   InfectedFlowFieldAnimation();
   ~InfectedFlowFieldAnimation();

   // initialize flowfield from 2d rendering
   void initialize();
   void initializePositions(const Vector& min, const Vector& max);
   void initializeParams(const Vector& min, const Vector& max);

   bool initialized() const;
   void setInitialized(bool init);

   // center reference position (world space 3d)
   const Vector& getCenter() const;
   void setCenter(const Vector& center);

   // flowfield scale
   float getScale() const;

   //! return size of particles in unprojected units
   float getPointSize() const;

   // update vertex position from flowfield directions
   void updatePositions(float deltaTime);

   // update colors of newly created particles
   void updateColors();

   // draw particles
   void draw();

   // all particles have faded away
   bool isElapsed() const;

   // stop respawning of particles
   void stop();
   bool stopped() const;

private:
   bool                    mInitialized;
   int                     mWidth;
   int                     mHeight;
   unsigned int            mVertexPosBuffer;
   unsigned int            mVertexColorBuffer;
   unsigned int            mVertexParamTexture;
   unsigned int            mParamTarget;
   unsigned int            mVertexColorTexture;
   unsigned int            mColorTarget;
   unsigned int            mPositions[2];
   unsigned int            mPosTarget[2];
   int                     mPage;

   Vector                  mCenter;
   float                   mFlowScale;
   float                   mParticleSize;

   float                   mElapsed;
   bool                    mStop;
};
