#pragma once

#include <QMap>
#include "image/image.h"
#include "math/matrix.h"

class Material;
class Vector;
class InfectedFlowFieldAnimation;

class PlayerInfectedEffect
{
public:
   PlayerInfectedEffect();
   ~PlayerInfectedEffect();

   void clear();

   void add( Material* playerMaterial );
   void remove( Material* playerMaterial );

   void animate(float delta);
   void render();

private:
   void update(InfectedFlowFieldAnimation* animation, Material* material, float delta);

   QMap<InfectedFlowFieldAnimation*, Material*> mFlowAnimations;

   Matrix       mProj;
   Matrix       mInvProj;

   unsigned int mParticleTextureId;
   unsigned int mFlowFieldTextureId;

   // draws vertices as point sprites
   unsigned int mPointsShader;
   int          mPointsTexture;
   int          mPointsSize;

   // move vertices along flowfield
   unsigned int mFlowUpdatePosShader;
   int          mFlowDepthTexture;
   int          mFlowVertexPosTexture;
   int          mFlowVertexParamTexture;
   int          mFlowFieldTexture;
   int          mFlowCenter;
   int          mFlowFieldScale;
   int          mFlowTimeDelta;
   int          mFlowSrcRect;
   int          mFlowInvProj;
   int          mFlowStop;

   unsigned int mFlowInitPosShader;
   int          mFlowInitPosDepth;
   int          mFlowInitPosInvProj;
   int          mFlowInitSrcRect;

   unsigned int mFlowInitParamShader;
   int          mFlowInitParamDepth;
   int          mFlowInitParamInvProj;
   int          mFlowInitParamCenter;

   unsigned int mFlowUpdateColShader;
   int          mFlowUpdateColColorMap;
   int          mFlowUpdateColPositionMap;
   int          mFlowUpdateColProj;
   int          mFlowUpdateColStop;

   float        mDelta;
};

