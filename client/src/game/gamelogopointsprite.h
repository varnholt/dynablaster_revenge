/***************************************************************************
                                 pointsprite
 ---------------------------------------------------------------------------
    project              : fireflies
    begin                : june 2011
    copyright            : (C) 2011 by mueslee
    author               : mueslee
    email                : nope
***************************************************************************/


#ifndef POINTSPRITE_H
#define POINTSPRITE_H

#include "tools/array.h"
#include "math/vector.h"


class GameLogoPointSprite
{
   public:

      //! constructor
      GameLogoPointSprite();

      //! initialize texture
      static void initialize();

      //! setter for point sprite vectors
      static void setPointSprites(
         const Array<Vector>& v,
         const Array<float>& glowValues
      );

      //! draw the vectors in one go
      static void draw();


   protected:

      //! pointsprite texture
      static int sTexture;

      //! vector singleton
      static Array<Vector> mPositions;

      //! z positions multiplied by the rotation matrix
      // static Array<Vector> mRotatedPositions;

      //! glow values
      static Array<float> mGlowValues;
};

#endif // POINTSPRITE_H


