#pragma once

#include "tools/array.h"

class FrameBuffer
{
public:
   enum FormatFlags
   {
      NoDepthBuffer = 1,
      DepthTexture = 2,
      FloatColor = 4
   };

   FrameBuffer(int width, int height, int multiSample = 0, int formatFlags = 0);
   ~FrameBuffer();

   static void push(FrameBuffer* fb= 0);
   static void pop();
   static FrameBuffer* Instance();

   int width() const;
   int height() const;
   int samples() const;
   float getSizeFactor(float refWidth) const;
   bool setResolution(int width, int height);
   bool resolutionChanged(int width, int height, int samples = 0) const;
   unsigned int texture() const;
   unsigned int depthTexture() const;
   unsigned int target() const;
   unsigned int copyTexture();
   void bind(int width = 0, int height = 0);
   void unbind();

   void draw(float alpha);

private:
   void discard();

   unsigned int mTarget;
   unsigned int mTempTarget;
   unsigned int mTexture;
   unsigned int mTempTexture;
   unsigned int mDepthBuffer;
   unsigned int mColorBuffer;
   int mWidth;
   int mHeight;
   int mTempWidth;
   int mTempHeight;
   int mSamples;
   bool mDirty;
   bool mDepth;
   int mFormatFlags;
   unsigned int mDepthTexture;

   static FrameBuffer* mInstance;
   static Array<FrameBuffer*> mStack;
};

