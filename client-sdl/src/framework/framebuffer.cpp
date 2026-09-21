#include "framebuffer.h"
#include "gldevice.h"

FrameBuffer* FrameBuffer::mInstance = nullptr;
Array<FrameBuffer*> FrameBuffer::mStack;
unsigned int FrameBuffer::mQuadVertexBuffer = 0;

FrameBuffer::FrameBuffer(int width, int height, int /*multiSample*/, int formatFlags)
    : mTarget(0), mTexture(0), mDepthBuffer(0), mWidth(0), mHeight(0), mFormatFlags(formatFlags)
{
   if (setResolution(width, height))
   {
      mWidth = width;
      mHeight = height;
   }
}

FrameBuffer::~FrameBuffer()
{
   discard();
}

FrameBuffer* FrameBuffer::Instance()
{
   return mInstance;
}

void FrameBuffer::discard()
{
   if (mTarget)
      glDeleteFramebuffers(1, &mTarget);

   if (mDepthBuffer)
      glDeleteRenderbuffers(1, &mDepthBuffer);

   if (mTexture)
      glDeleteTextures(1, &mTexture);
}

bool FrameBuffer::setResolution(int width, int height)
{
   if (mWidth == width && mHeight == height)
      return true;

   discard();

   glGenTextures(1, &mTexture);
   glBindTexture(GL_TEXTURE_2D, mTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   glBindTexture(GL_TEXTURE_2D, 0);

   glGenFramebuffers(1, &mTarget);
   glBindFramebuffer(GL_FRAMEBUFFER, mTarget);

   if ((mFormatFlags & NoDepthBuffer) == 0)
   {
      glGenRenderbuffers(1, &mDepthBuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
   }

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

   const bool ok = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
   if (ok)
   {
      mWidth = width;
      mHeight = height;
   }
   else
   {
      mWidth = 0;
      mHeight = 0;
   }

   if ((mFormatFlags & NoDepthBuffer) == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_COLOR_BUFFER_BIT);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   return ok;
}

void FrameBuffer::bind(int width, int height)
{
   mInstance = this;
   glBindFramebuffer(GL_FRAMEBUFFER, mTarget);
   if (width && height)
      glViewport(0, 0, width, height);
   else
      glViewport(0, 0, mWidth, mHeight);
}

void FrameBuffer::unbind()
{
   mInstance = nullptr;
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::push(FrameBuffer* fb)
{
   mStack.add(mInstance);
   if (fb)
      fb->bind();
}

void FrameBuffer::pop()
{
   if (mStack.size() > 0)
   {
      FrameBuffer* prev = mStack.takeLast();
      if (prev)
         prev->bind();
      else
      {
         mInstance = nullptr;
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
         glViewport(activeDevice->getBorderLeft(), activeDevice->getBorderBottom(), activeDevice->getWidth(), activeDevice->getHeight());
      }
   }
}

int FrameBuffer::width() const
{
   return mWidth;
}

int FrameBuffer::height() const
{
   return mHeight;
}

bool FrameBuffer::resolutionChanged(int width, int height) const
{
   return (mWidth != width || mHeight != height);
}

unsigned int FrameBuffer::texture() const
{
   return mTexture;
}

unsigned int FrameBuffer::target() const
{
   return mTarget;
}

void FrameBuffer::draw(float alpha)
{
   // the legacy immediate-mode quad (glBegin(GL_QUADS) with per-vertex glTexCoord2f/glVertex3f,
   // drawn with both matrix stacks reset to identity right before) becomes a plain
   // attribute-array draw. The caller (MenuDrawable) binds the shared texalphaignore shader and
   // must push an identity world transform and an identity projection (see
   // GLDevice::setProjectionMatrix()) before calling this, so u_modelViewProjection works out to
   // identity too - reproducing the same "positions are already in clip space" effect the two
   // glLoadIdentity() calls gave the legacy fixed-function quad.
   if (mQuadVertexBuffer == 0)
   {
      static const float quad[] = {
         -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
         -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
      };

      glGenBuffers(1, &mQuadVertexBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
   }

   glBindTexture(GL_TEXTURE_2D, mTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   activeDevice->setParameter(activeDevice->getParameterIndex("alpha"), alpha);

   glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
}
