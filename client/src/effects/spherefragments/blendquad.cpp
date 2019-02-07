#include "blendquad.h"
#include "framework/gldevice.h"

BlendQuad::BlendQuad()
 : mShader(0),
   mColorParam(0),
   mScaleParam(0),
   mOffsetParam(0)
{
   mShader = activeDevice->loadShader(
      "blendquad-vert.glsl",
      "blendquad-frag.glsl"
   );

   mColorParam = activeDevice->getParameterIndex("color");
   mScaleParam = activeDevice->getParameterIndex("scale");
   mOffsetParam = activeDevice->getParameterIndex("offset");
}



void BlendQuad::process(
   unsigned int texture,
   const Vector4& color,
   float scale,
   const Vector& offset
)
{
   activeDevice->setShader(mShader);
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   activeDevice->setParameter(mColorParam, color);
   activeDevice->setParameter(mScaleParam, scale);
   activeDevice->setParameter(mOffsetParam, offset);
   glBindTexture(GL_TEXTURE_2D, texture);

   glBegin(GL_QUADS);
   glVertex3f(-1,-1,-1);
   glVertex3f( 1,-1,-1);
   glVertex3f( 1, 1,-1);
   glVertex3f(-1, 1,-1);
   glEnd();

   activeDevice->setShader(0);
   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);
}

