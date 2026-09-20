#pragma once

#include "math/vector.h"

class Vertex3D
{
   public:

      Vertex3D()
        : mU(0.0f),
          mV(0.0f),
          mIndex(0.0f),
          mBlend(0.0f)
      {
      }

      Vector mPosition;
      Vector mNormal;
      float mU;
      float mV;
      float mIndex;
      float mBlend;
      Vector mTangent;
};
