// header
#include "fuseparticleemitter.h"

// game
#include "fuseparticle.h"


//-----------------------------------------------------------------------------
/*!
*/
FuseParticleEmitter::FuseParticleEmitter(const Vector &startPosition)
 : mPosition(startPosition)
{
   FuseParticle* particle = 0;
   for (int i = 0; i < PARTICLE_COUNT; i++)
   {
      particle = new FuseParticle();
      particle->reset(mPosition);

      mFuseParticles.add(particle);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
FuseParticleEmitter::~FuseParticleEmitter()
{
   // particles will die at the end of their lifecycle :~(
   for (int i = 0; i < mFuseParticles.size(); i++)
      mFuseParticles[i]->setDeleted(true);
}


//-----------------------------------------------------------------------------
/*!
   \param position fuse emitter position
*/
void FuseParticleEmitter::setPosition(const Vector &position)
{
   mPrevPosition = mPosition;
   mPosition = position;

   // only update if position actually changed
   if (mPrevPosition != mPosition)
   {
      FuseParticle* particle = 0;
      for (int i = 0; i < mFuseParticles.size(); i++)
      {
         particle = mFuseParticles[i];

         // set origin for next respawn
         particle->setNextOrigin(position);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \return fuse emitter position
*/
const Vector &FuseParticleEmitter::getPosition() const
{
   return mPosition;
}


