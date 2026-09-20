#pragma once

// GLES3 port of client/src/game/gamelogopointsprite.h. The class name suggests GL_POINT_SPRITE /
// gl_PointSize-driven rendering, but the real draw() (see the .cpp) never used that at all - each
// "sprite" was always a screen-facing quad built from 4 explicit vertices in world space, sized by
// offsetting x/y directly (no billboarding math, no point-sprite GL state). That makes this a
// perfectly ordinary attribute-array quad batch in GLES3, not the point-sprite API port the name
// implies - verified against the real .cpp before assuming otherwise.

#include "tools/array.h"
#include "math/vector.h"
#include "render/texture.h"

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
      static Texture sTexture;

      //! shader + uniform locations
      static unsigned int sShader;
      static int sTextureParam;

      //! dynamic quad-batch vertex buffer (position + texcoord per vertex, 6 verts/sprite -
      //! rebuilt every draw() call since positions/glow values change every frame)
      static unsigned int sVertexBuffer;

      //! vector singleton
      static Array<Vector> mPositions;

      //! glow values
      static Array<float> mGlowValues;
};
