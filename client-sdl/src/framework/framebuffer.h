#pragma once

#include "tools/array.h"

/// \brief GLES3 port of client/src/framework/framebuffer.cpp.
///
/// The legacy multisample-resolve path (EXT_framebuffer_object era) and copyTexture() are
/// dropped - unused by any real caller so far. DepthTexture (needed by PlayerDeathEffect, which
/// samples a dying player's depth buffer to seed its particle positions) is ported for real.
class FrameBuffer
{
public:
   enum FormatFlags
   {
      NoDepthBuffer = 1,
      DepthTexture = 2
   };

   FrameBuffer(int width, int height, int multiSample = 0, int formatFlags = 0);
   ~FrameBuffer();

   static void push(FrameBuffer* fb = 0);
   static void pop();
   static FrameBuffer* Instance();

   int width() const;
   int height() const;
   // matches the original's mWidth * sqrt(mSamples) / refWidth - this port has no multisampling
   // (mSamples is always effectively 1), so the sqrt(mSamples) term simplifies away.
   float getSizeFactor(float refWidth) const;
   bool setResolution(int width, int height);
   bool resolutionChanged(int width, int height) const;
   unsigned int texture() const;
   unsigned int target() const;
   unsigned int depthTexture() const;
   void bind(int width = 0, int height = 0);
   void unbind();

   void draw(float alpha);

private:
   void discard();

   unsigned int mTarget;
   unsigned int mTexture;
   unsigned int mDepthBuffer;
   unsigned int mDepthTexture;
   int mWidth;
   int mHeight;
   int mFormatFlags;

   static FrameBuffer* mInstance;
   static Array<FrameBuffer*> mStack;

   // shared full-screen quad used by draw() - lazily created, never torn down (mirrors the
   // process-lifetime GL objects the rest of the engine already keeps around, e.g. shader
   // programs in GLDevice's shader table).
   static unsigned int mQuadVertexBuffer;
};
