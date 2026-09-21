#pragma once

#include "tools/array.h"

/// \brief GLES3 port of client/src/framework/framebuffer.cpp.
///
/// The legacy multisample-resolve path (EXT_framebuffer_object era), the DepthTexture/FloatColor
/// format flags, and copyTexture() are dropped - none of them are used by the one consumer so far
/// (MenuDrawable's page cross-fade), which only ever constructs a plain single-sample RGBA
/// framebuffer with an ordinary depth renderbuffer. Re-add whichever of these a real caller
/// needs once one shows up, rather than guessing at requirements now.
class FrameBuffer
{
public:
   enum FormatFlags
   {
      NoDepthBuffer = 1
   };

   FrameBuffer(int width, int height, int multiSample = 0, int formatFlags = 0);
   ~FrameBuffer();

   static void push(FrameBuffer* fb = 0);
   static void pop();
   static FrameBuffer* Instance();

   int width() const;
   int height() const;
   bool setResolution(int width, int height);
   bool resolutionChanged(int width, int height) const;
   unsigned int texture() const;
   unsigned int target() const;
   void bind(int width = 0, int height = 0);
   void unbind();

   void draw(float alpha);

private:
   void discard();

   unsigned int mTarget;
   unsigned int mTexture;
   unsigned int mDepthBuffer;
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
