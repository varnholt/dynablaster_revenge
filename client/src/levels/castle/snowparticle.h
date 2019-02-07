#ifndef SNOWPARTICLE_H
#define SNOWPARTICLE_H

#include "math/vector.h"

class SnowParticle
{

public:

   SnowParticle()
    : mPosition(0.0f),
      mSize(1.0f),
      mSpeed(1.0f),
      mVisible(false)
   {
   }

   Vector mPosition;
   float mSize;
   float mSpeed;
   bool mVisible;
};

#endif // SNOWPARTICLE_H
