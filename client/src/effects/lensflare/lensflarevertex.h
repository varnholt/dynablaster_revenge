#pragma once

#include "math/vector2.h"
#include "math/vector.h"


class LensFlareVertex
{
   public:

      LensFlareVertex()
       : mRayPosition(0.0f),
         mRotationSpeed(0.0f)
      {
      }

      Vector2 mPosition;
      float mRayPosition;
      float mRotationSpeed;
      Vector2 mOffset;
      Vector2 mUv;
      Vector mColor;
};

