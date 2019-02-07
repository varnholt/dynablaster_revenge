#ifndef FUSEPARTICLEEMITTER_H
#define FUSEPARTICLEEMITTER_H

// engine
#include "tools/array.h"
#include "math/vector.h"

// defines
#define PARTICLE_COUNT 100

// forward declarations
class FuseParticle;

class FuseParticleEmitter
{
   public:

      //! constructor
      FuseParticleEmitter(const Vector& startPosition);

      //! destructor
      ~FuseParticleEmitter();

      //! setter for bomb/emitter position
      void setPosition(const Vector& position);

      //! getter for bomb/emitter position
      const Vector& getPosition() const;


   protected:

      //! bomb/emitter position
      Vector mPosition;

      //! previous position
      Vector mPrevPosition;

      //! particles related to this fuse
      Array<FuseParticle*> mFuseParticles;

};

#endif // FUSEPARTICLEEMITTER_H
