#pragma once

#include "math/vector.h"

class StarTalersVertex
{
   public:

      StarTalersVertex()
       : mU(0.0f),
         mV(0.0f),
         mSpeed(0.0f),
         mIndex(0.0f)
      {
      }

      Vector mPosition;
      Vector mNormal;

      float mU;
      float mV;

      Vector mDirection;

      float mSpeed;
      float mIndex;
};

