#pragma once

#include "filter.h"

class BrightnessFilter : public Filter
{
public:
   BrightnessFilter(float gamma = 2.2);
   ~BrightnessFilter();

   bool init();

   void setGamma(float gamma);

   void process(unsigned int texture, float u, float v);

private:
   unsigned int mShader;
   int          mParamGamma;
   int          mParamTexture;
   float        mGamma;
};
