// header
#include "shroomfilter.h"

// framework
#include "framework/gldevice.h"

// cmath
#include <math.h>


ShroomFilter::ShroomFilter()
 : Filter("shroom"),
   mShader(0),
   mIntensityParam(-1),
   mIntensity(0.0f),
   mTimeParam(-1),
   mTime(0.0f),
   mTextureParam(-1)
{
}


ShroomFilter::~ShroomFilter()
{
}


bool ShroomFilter::init()
{
   mShader = activeDevice->loadShader(
      "mushroom-vert.glsl",
      "mushroom-frag.glsl"
   );

   mIntensityParam = activeDevice->getParameterIndex("intensity");
   mTimeParam = activeDevice->getParameterIndex("time");
   mTextureParam = activeDevice->getParameterIndex("texture");

   return true;
}


void ShroomFilter::process(unsigned int texture, float u, float v)
{
   activeDevice->setPerspective(1.0f, 1.0f, 0.5f, 2.0f);

   activeDevice->setShader(mShader);

   // disable ztest & zwrite
   glDepthMask(GL_FALSE);
   glDisable(GL_DEPTH_TEST);

   // bind texture
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   activeDevice->bindSampler(mTextureParam, 0);

   // set uniform parameters
   activeDevice->setParameter(getIntensityParam(), getIntensity());
   activeDevice->setParameter(getTimeParam(), getTime());

   float scale = 1.0f + 0.1f * getIntensity();

   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-scale,-scale, -1);
   glTexCoord2f(u,0); glVertex3f( scale,-scale, -1);
   glTexCoord2f(u,v); glVertex3f( scale, scale, -1);
   glTexCoord2f(0,v); glVertex3f(-scale, scale, -1);
   glEnd();

   activeDevice->setShader(0);

   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);
}


void ShroomFilter::setIntensity(float intensity)
{
   mIntensity = intensity;
}


float ShroomFilter::getIntensity() const
{
   return mIntensity;
}


int ShroomFilter::getIntensityParam() const
{
   return mIntensityParam;
}


void ShroomFilter::setTime(float time)
{
   mTime = time;
}


float ShroomFilter::getTime() const
{
   return mTime;
}


int ShroomFilter::getTimeParam() const
{
   return mTimeParam;
}

