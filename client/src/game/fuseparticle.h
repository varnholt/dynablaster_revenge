#ifndef FUSEPARTICLE_H
#define FUSEPARTICLE_H

// engine
#include "tools/array.h"
#include "math/vector.h"

class FuseParticle
{
   public:

      //! constructor
      FuseParticle();

      //! destructor
      ~FuseParticle();

      //! reset particle
      void reset(const Vector &origin);

      //! animate each particle
      static void animate(float dt);

      //! we want to draw *all* particles here
      static void draw();

      //! initialize particles
      static void initialize();

      //! getter for bomb offset
      static const Vector& getBombOffset();

      //! clear fuse particles
      static void clear();

      //! setter for deleted flag
      void setDeleted(bool deleted);

      //! getter for deleted flag
      bool isDeleted() const;

      //! setter for origin
      void setOrigin(const Vector &origin);

      //! getter for origin
      const Vector& getOrigin() const;

      //! setter for next origin
      void setNextOrigin(const Vector &origin);

      //! getter for next origin
      const Vector& getNextOrigin() const;


   protected:

      //! pointsprite texture
      static int sTexture;

      //! origin
      Vector mOrigin;

      //! origin after next respawn
      Vector mNextOrigin;

      //! particle position
      Vector mPosition;

      //! particle direction
      Vector mDirection;

      //! point size
      float mPointSize;

      //! particle scalar
      float mScalar;

      //! deleted flag
      bool mDeleted;

      //! elapsed time
      float mElapsed;

      //! random start time
      float mRandomStartTime;

      //! started flag
      bool mStarted;

      //! static list of particles
      static Array<FuseParticle*> sParticles;

      //! static list of deleted particles
      static Array<FuseParticle*> sDeletedParticles;

      //! offset to bomb
      static Vector sBombOffset;
};

#endif // FUSEPARTICLE_H
