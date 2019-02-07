#pragma once

#include "filter.h"
#include "framework/framebuffer.h"

class BlurFilter : public Filter
{
public:
   BlurFilter(float radius=1.0f);
   ~BlurFilter();

   bool init();

   void setRadius(float radius);
   void setAlpha(float alpha);

   void process(unsigned int texture, float u=1.0f, float v=1.0f);


   void matchFilters(unsigned int texture);
   void matchInterpolation(unsigned int texture);

private:
   void begin();
   void end();

   void downSamplePass(FrameBuffer* dst, int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight, float deltaU, float deltaV, float texelU, float texelV, float border);
   unsigned int downSample(unsigned int texture, int width, int height, int pass);

   void gaussPass(FrameBuffer* dst, int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight, float deltaU, float deltaV, float texelU, float texelV);
   unsigned int gauss(float radius, unsigned int texture, int width, int height);

   void draw(int dstWidth, int dstHeight, unsigned int texture, int srcWidth, int srcHeight);


   unsigned int mDownsample;
   unsigned int mGauss;

   int          mParamOffsetX1;
   int          mParamOffsetY1;
   int          mParamClampU1;
   int          mParamClampV1;
   int          mParamTexture1;

   int          mParamOffsetX2;
   int          mParamOffsetY2;
   int          mParamTexture2;
   int          mParamClampU2;
   int          mParamClampV2;
   int          mParamRadius;
   int          mParamKernel;

   float        mRadius;
   FrameBuffer* mTemp[2];

   float        mAlpha;
};
