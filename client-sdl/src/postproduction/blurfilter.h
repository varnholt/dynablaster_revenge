#pragma once

// GLES3 port of client/src/postproduction/blurfilter.cpp. matchFilters()/matchInterpolation()
// (interactive filter-coefficient calibration tools, only ever driven by a hardcoded toggle
// nothing sets - confirmed zero callers anywhere in the real engine) are dropped; setRadius()/
// setAlpha()/init()/process() are the real, used API (process() by SphereFragmentsDrawable,
// setAlpha() by the separate in-game GameWindDrawable blur instance - not ported yet, but this
// class stays generic enough to serve it too when that subsystem's turn comes).

#include "filter.h"
#include "framework/framebuffer.h"
#include "fullscreenquad.h"

class BlurFilter : public Filter
{
public:
   BlurFilter(float radius=1.0f);
   ~BlurFilter();

   bool init();

   void setRadius(float radius);
   void setAlpha(float alpha);

   void process(unsigned int texture, float u=1.0f, float v=1.0f);

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

   // final upscale blit (draw()) used a shader-less fixed-function textured quad in the original
   // (activeDevice->setShader(0), modulated by the current glColor4f alpha) - GLES3 has no
   // fixed-function fallback, so this is a small dedicated shader instead (not from the original).
   unsigned int mBlit;
   int          mParamBlitTexture;
   int          mParamBlitAlpha;

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

   FullScreenQuad mQuad;
};
