#include "brightnessfilter.h"
#include "framework/gldevice.h"

BrightnessFilter::BrightnessFilter(float gamma)
: Filter("brightness")
, mShader(0)
, mParamGamma(-1)
, mParamTexture(-1)
, mGamma(gamma)
{
}

BrightnessFilter::~BrightnessFilter()
{
}

bool BrightnessFilter::init()
{
   mShader= activeDevice->loadShader("brightness-vert.glsl", "brightness-frag.glsl");

   mParamTexture= activeDevice->getParameterIndex("texture");
   mParamGamma=   activeDevice->getParameterIndex("gamma");

   return true;
}

void BrightnessFilter::setGamma(float gamma)
{
   mGamma= gamma;
}

void BrightnessFilter::process(unsigned int texture, float u, float v)
{
   activeDevice->setShader( mShader );

   // bind texture
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   activeDevice->bindSampler(mParamTexture, 0);
   
   // set uniform parameters
   activeDevice->setParameter(mParamGamma, mGamma);

   glColor4f(1,1,1,1);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(u,0); glVertex3f( 1,-1,-1);
   glTexCoord2f(u,v); glVertex3f( 1, 1,-1);
   glTexCoord2f(0,v); glVertex3f(-1, 1,-1);
   glEnd();

   activeDevice->setShader( 0 );
}

