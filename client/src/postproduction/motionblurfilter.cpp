// header
#include "motionblurfilter.h"

// framework
#include "framework/gldevice.h"

// cmath
#include <math.h>


MotionBlurFilter::MotionBlurFilter()
 : Filter("motionblur"),
   mShader(0),
   mIntensityParam(-1),
   mIntensity(0.0f),
   mMotionDirParam(-1),
   mTextureParam(0)
{
}


MotionBlurFilter::~MotionBlurFilter()
{
}


bool MotionBlurFilter::init()
{
   mShader = activeDevice->loadShader(
      "motionblur-vert.glsl",
      "motionblur-frag.glsl"
   );

   mIntensityParam = activeDevice->getParameterIndex("intensity");
   mMotionDirParam = activeDevice->getParameterIndex("motionDir");
   mTextureParam = activeDevice->getParameterIndex("texture");

   return true;
}


void MotionBlurFilter::process(unsigned int /*texture*/, float /*u*/, float /*v*/)
{
   activeDevice->setShader(mShader);

   // set uniform parameters
   activeDevice->setParameter(getIntensityParam(), getIntensity());
   activeDevice->setParameter(getMotionDirParam(), getMotionDir());
   activeDevice->bindSampler(getTextureParam(), 0);

   // activeDevice->setShader(0);
}


void MotionBlurFilter::setIntensity(float intensity)
{
   mIntensity = intensity;
}


float MotionBlurFilter::getIntensity() const
{
   return mIntensity;
}


int MotionBlurFilter::getIntensityParam() const
{
   return mIntensityParam;
}


void MotionBlurFilter::setMotionDir(const Vector &dir)
{
   mMotionDir = dir;
}


const Vector &MotionBlurFilter::getMotionDir() const
{
   return mMotionDir;
}


int MotionBlurFilter::getMotionDirParam() const
{
   return mMotionDirParam;
}


void MotionBlurFilter::setTextureParam(int id)
{
   mTextureParam = id;
}


int MotionBlurFilter::getTextureParam() const
{
   return mTextureParam;
}


