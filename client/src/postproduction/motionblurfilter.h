#pragma once

// base
#include "filter.h"

// math
#include "math/vector.h"


class MotionBlurFilter : public Filter
{
public:

   MotionBlurFilter();
   virtual ~MotionBlurFilter();

   virtual bool init();
   virtual void process(unsigned int texture, float u, float v);

   const String& getName() const;

   void setIntensity(float intensity);
   float getIntensity() const;
   int getIntensityParam() const;

   void setMotionDir(const Vector& dir);
   const Vector& getMotionDir() const;
   int getMotionDirParam() const;


   void setTextureParam(int id);
   int getTextureParam() const;



protected:

   unsigned int mShader;

   int mIntensityParam;
   float mIntensity;

   int mMotionDirParam;
   Vector mMotionDir;

   int mTextureParam;
};

