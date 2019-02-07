#include "duplicatealpha.h"
#include "math/vector4.h"
#include "framework/gldevice.h"

DuplicateAlpha::DuplicateAlpha()
{
   mShader = activeDevice->loadShader(
      "duplicatealpha-vert.glsl",
      "duplicatealpha-frag.glsl"
   );

   mColorParam = activeDevice->getParameterIndex("color");
}



void DuplicateAlpha::process(unsigned int texture, const Vector4& color)
{
   activeDevice->setShader(mShader);
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   activeDevice->setParameter(mColorParam, color);
   glBindTexture(GL_TEXTURE_2D, texture);

   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f( 1,-1,-1);
   glTexCoord2f(1,1); glVertex3f( 1, 1,-1);
   glTexCoord2f(0,1); glVertex3f(-1, 1,-1);
   glEnd();

   activeDevice->setShader(0);
   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);
}
