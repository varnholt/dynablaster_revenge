#pragma once

#include <QList>
#include "image/image.h"

class DeathFlowFieldAnimation;
class Material;
class Vector;
class Matrix;

class PlayerDeathEffect
{
public:
   PlayerDeathEffect();
   ~PlayerDeathEffect();

   void clear();

   void add( Material* playerMaterial );

   void animate(float delta);
   void render();

private:
   QList<DeathFlowFieldAnimation*> mFlowAnimations;

   unsigned int mParticleTextureId;
   unsigned int mFlowFieldTextureId;

   // draws vertices as point sprites
   unsigned int mPointsShader;
   int          mPointsTexture;
   int          mPointsColorMap;
   int          mPointsSize;

   // move vertices along flowfield
   unsigned int mFlowShader;
   int          mFlowVertexPosTexture;
   int          mFlowVertexColTexture;
   int          mFlowFieldTexture;
   int          mFlowCenter;
   int          mFlowFieldScale;
   int          mFlowTimeDelta;

   unsigned int mFlowInitPosShader;
   int          mFlowInitPosDepth;
   int          mFlowInitPosInvProj;

   unsigned int mFlowInitParamShader;
   int          mFlowInitParamDepth;
   int          mFlowInitParamInvProj;
   int          mFlowInitParamCenter;
};

