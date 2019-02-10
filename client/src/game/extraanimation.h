#ifndef EXTRAANIMATION_H
#define EXTRAANIMATION_H

// Qt
#include <QColor>
#include <QList>

// engine
#include "math/vector.h"
#include "framework/frametimer.h"

class ExtraAnimation
{
   class ExtraPointSprite
   {
      public:

         ExtraPointSprite()
          : mSize(1.0f),
            mScalar(0.0f),
            mIntensity(1.0f),
            mElapsed(0.0f),
            mStartTime(0.0f)
         {
         }

         float mSize;
         float mScalar;
         float mIntensity;
         float mElapsed;
         float mStartTime;

         Vector mPosition;
         Vector mOrigin;
         Vector mDirection;
   };


   public:

      //! constructor
      ExtraAnimation();

      //! this is only done once
      static void initializeTextures();

      //! initialize texture
      void initialize();

      //! draw the QVector3Ds in one go
      void draw(float dt);

      //! update sprite position
      void updateSpritePosition(
         float dt,
         ExtraPointSprite* sprite
      );

      void setOrigin(const Vector& origin);

      const Vector& getOrigin() const;

      bool isElapsed() const;

      void setExtraDestroyed(bool destroyed);

      bool isExtraDestroyed() const;


    protected:

      void initPointsprites();
      void initSphere();

      void drawPointSprites(float dt);
      void drawHalfSphere(float dt);
      void drawFullSphere(float dt);

      ExtraPointSprite* getSprite(int x, int y);

      //! textures
      static int sPointspriteTexture;
      static int sHalfSphereTexture;
      static int sFullSphereTexture;
      static int sFullSphereTextureRed;

      // pointsprites
      ExtraPointSprite** mSprites;
      QColor mSpriteColor;
      FrameTimer mAnimationTime;

      Vector mOrigin;

      bool mExtraDestroyed;
};

#endif // POINTSPRITE_H


