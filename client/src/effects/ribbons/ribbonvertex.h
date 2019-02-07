#pragma once

#include "math/vector.h"

class RibbonVertex
{
   public:

      RibbonVertex()
       : mU(0.0f),
         mV(0.0f)
      {
      }

         Vector mPosition;
         Vector mNormal;
         float mU;
         float mV;
};

