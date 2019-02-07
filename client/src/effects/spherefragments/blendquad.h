#pragma once

#include "math/vector.h"
#include "math/vector4.h"

class BlendQuad
{

public:

   BlendQuad();

   void process(
      unsigned int texture,
      const Vector4& color = Vector4(1,1,1,1),
      float scale = 1.0f,
      const Vector& offset = Vector(0,0,0)
   );

private:

   unsigned int mShader;
   int mColorParam;
   int mScaleParam;
   int mOffsetParam;
};
