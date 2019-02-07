#pragma once

#include <QMap>
#include "image/image.h"
#include "render/texture.h"

class PlayerInvincibleInstance;
class Material;
class Vector;
class Matrix;
class FrameBuffer;

class PlayerInvincibleEffect
{
public:
   PlayerInvincibleEffect();
   ~PlayerInvincibleEffect();

   void clear();
   void setRadius(float radius);

   void add( Material* playerMaterial );
   void remove( Material* playerMaterial );
   void animate(float delta);
   void render();

private:
   void blurPlayers(FrameBuffer* dst, const Matrix& proj);
   void setMaterialFade(PlayerInvincibleInstance* player, float fade);

   QMap<Material*, PlayerInvincibleInstance*> mPlayers;
   Texture      mDisplacementTexture;
   float        mRadius;
   float        mKernel[32];

   unsigned int mBlurHShader;
   int          mBlurHTexture;
   int          mBlurHTexelOffset;
   int          mBlurHRadius;
   int          mBlurHKernel;

   unsigned int mBlurVShader;
   int          mBlurVTexture;
   int          mBlurVTexelOffset;
   int          mBlurVRadius;
   int          mBlurVKernel;

   unsigned int mDisplaceShader;
   int          mDisplaceTexture1;
   int          mDisplaceTexture2;
   int          mDisplaceTexelOffset;
   int          mDisplaceOffset;
   int          mDisplaceSourceRect;
   int          mDisplaceFade;
   int          mDisplayCenter;
};

