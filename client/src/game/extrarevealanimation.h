/***************************************************************************
             extrarevealanimation
 ---------------------------------------------------------------------------
    project              : dynablaster revenge client
    begin                : january 2013
    copyright            : (C) 2013 by mueslee
    author               : mueslee
    email                : nope
***************************************************************************/


#ifndef EXTRAREVEALANIMATION_H
#define EXTRAREVEALANIMATION_H

// engine
#include "math/vector.h"

class ExtraRevealAnimation
{
   public:

      //! constructor
      ExtraRevealAnimation();

      //! this is only done once
      static void initializeTextures();

      //! initialize texture
      static void initialize();

      //! draw the QVector3Ds in one go
      void draw(float dt);

      bool isElapsed() const;

      void setPos(int x, int y);

    protected:

      void drawFrustum(float dt);

      //! textures
      static int sFrustumTexture;

      int mX;
      int mY;

      float mElapsed;

      //! shader
      static unsigned int mShader;
      static int sBlendColorParameter;
      static int sTextureScrollParameter;
};

#endif // EXTRAREVEALANIMATION_H


