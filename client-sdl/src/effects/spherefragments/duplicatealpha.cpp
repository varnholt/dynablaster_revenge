#include "duplicatealpha.h"
#include "gldevice.h"
#include "math/vector4.h"

DuplicateAlpha::DuplicateAlpha() : mShader(0), mColorParam(-1)
{
   mShader = activeDevice->loadShader("duplicatealpha-vert.glsl", "duplicatealpha-frag.glsl");

   mColorParam = activeDevice->getParameterIndex("color");
}

void DuplicateAlpha::process(unsigned int texture, const Vector4& color)
{
   activeDevice->setShader(mShader);
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   activeDevice->setParameter(mColorParam, color);
   glBindTexture(GL_TEXTURE_2D, texture);

   mQuad.drawUnit();

   activeDevice->setShader(0);
   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);
}
