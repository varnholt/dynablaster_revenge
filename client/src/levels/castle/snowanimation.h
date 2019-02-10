#ifndef SNOWANIMATION_H
#define SNOWANIMATION_H

// Qt
#include <QColor>
#include <QList>
#include <QTime>

// engine
#include "math/matrix.h"
#include "math/vector.h"

#include "render/texture.h"

#define SNOW_PARTICLES_COUNT 2000

class SnowParticle;

class SnowAnimation
{
   public:

      //! constructor
      SnowAnimation();

      //! this is only done once
      void initializeTextures();

      //! initialize texture
      void initialize();

      //! draw the QVector3Ds in one go
      void draw();

      //! update sprite position
      void updateSpritePosition(
         int animTimeElapsed,
         float dt,
         SnowParticle* sprite
      );

      //! animate
      void animate(float dt);

      //! restart elapsed time
      void reset();


protected:

      //! create particles
      void createPointsprites();

      //! init particles
      void initPointsprites();

      //! init a single particle
      void initParticle(
         SnowParticle* sprite,
         float y
      );

      //! draw point sprites
      void drawPointSprites();

      //! update wind direction
      void updateWind();

      //! textures
      Texture mPointspriteTexture;

      //! shader
      unsigned int mShader;
      int mParamTexture;
      int mParamSizeFactor;

      //! vertex/index buffer objects
      unsigned int mVertices;

      //! bunch of pointsprites
      SnowParticle* mSnowParticles[SNOW_PARTICLES_COUNT];

      //! animation time
      QTime mAnimationTime;

      //! particle upper limit
      float mLimitUp;

      //! particle lower limit
      float mLimitDown;

      //! direction of the wind
      Vector mWindDirection;

      //! wind scalar
      float mWindScalar;
};

#endif // SNOWANIMATION_H


