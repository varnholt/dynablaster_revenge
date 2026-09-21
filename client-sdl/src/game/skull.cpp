// header
#include "skull.h"

// framework
#include "globaltime.h"

Skull::Skull(Mesh *reference, float x, float y)
 : Mesh(*reference),
   mReference(reference)
{
   setUserTransformable(true);

   mTranslation.identity();
   mTranslation.translate( Vector(x + 0.5f, -y - 0.5f, 0.5f) );

   // animation starts at 0
   setAnimationFrame(0.0f);

   // animation start time
   mStartTime = GlobalTime::Instance()->getTime();
}


Skull::~Skull()
{
}


Mesh *Skull::getReference() const
{
   return mReference;
}


Matrix Skull::getTranslation()  const
{
   return mTranslation;
}


float Skull::getStartTime() const
{
   return mStartTime;
}
