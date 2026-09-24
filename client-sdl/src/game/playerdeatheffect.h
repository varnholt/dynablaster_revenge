#pragma once

/// \brief GLES3 port of client/src/game/playerdeatheffect.cpp - see project memory for the full
/// GPGPU-particle-simulation background. Interface unchanged from the original.

#include "image/image.h"

#include <QList>

class DeathFlowFieldAnimation;
class Material;
class Vector;
class Matrix;
class FrameBuffer;

class PlayerDeathEffect
{
public:
   PlayerDeathEffect();
   ~PlayerDeathEffect();

   void clear();

   void add(Material* playerMaterial);


   void animate(float delta);
   void render();

private:
   QList<DeathFlowFieldAnimation*> mFlowAnimations;

   unsigned int mParticleTextureId;
   unsigned int mFlowFieldTextureId;

   // draws vertices as point sprites
   unsigned int mPointsShader;
   int mPointsTexture;
   int mPointsColorMap;
   int mPointsSize;
   int mPointsProjection;

   // move vertices along flowfield
   unsigned int mFlowShader;
   int mFlowVertexPosTexture;
   int mFlowVertexColTexture;
   int mFlowFieldTexture;
   int mFlowCenter;
   int mFlowFieldScale;
   int mFlowTimeDelta;

   unsigned int mFlowInitPosShader;
   int mFlowInitPosDepth;
   int mFlowInitPosInvProj;

   unsigned int mFlowInitParamShader;
   int mFlowInitParamDepth;
   int mFlowInitParamInvProj;
   int mFlowInitParamCenter;

   // owns its own offscreen render target for capturing a dying player's color+depth (see
   // DeathFlowFieldAnimation) - this port has no MainDrawable to borrow a shared render-buffer
   // from, matching MenuDrawable's own established "own your FBO" pattern.
   FrameBuffer* mDeferredBuffer;
};
