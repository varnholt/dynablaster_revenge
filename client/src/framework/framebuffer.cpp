#include "framebuffer.h"
#include "gldevice.h"
#include <math.h>

FrameBuffer* FrameBuffer::mInstance = 0;
Array<FrameBuffer*> FrameBuffer::mStack;

FrameBuffer::FrameBuffer(int width, int height, int samples, int formatFlags)
: mTarget(0)
, mTempTarget(0)
, mTexture(0)
, mTempTexture(0)
, mDepthBuffer(0)
, mColorBuffer(0)
, mWidth(0)
, mHeight(0)
, mTempWidth(0)
, mTempHeight(0)
, mSamples(samples)
, mDirty(false)
, mFormatFlags(formatFlags)
, mDepthTexture(0)
{
   if (setResolution(width, height))
   {
      mWidth= width;
      mHeight= height;
   }
}


FrameBuffer::~FrameBuffer()
{
   discard();

   if (mTempTexture)
      glDeleteTextures(1, &mTempTexture);

   if (mDepthTexture)
      glDeleteTextures(1, &mDepthTexture);
}


FrameBuffer* FrameBuffer::Instance()
{
   return mInstance;
}


float FrameBuffer::getSizeFactor(float refWidth) const
{
   return (float) (mWidth * (float)sqrt( (double)mSamples ) / refWidth);
}

void FrameBuffer::discard()
{
   if (mTarget)
      glDeleteFramebuffers(1, &mTarget);

   if (mDepthBuffer)
      glDeleteRenderbuffers(1, &mDepthBuffer);

   if (mTempTarget)
      glDeleteFramebuffers(1, &mTempTarget);

   if (mColorBuffer)
      glDeleteRenderbuffers(1, &mColorBuffer);

   if (mTexture)
      glDeleteTextures(1, &mTexture);

   if (mDepthTexture)
      glDeleteRenderbuffers(1, &mDepthTexture);
}


unsigned int FrameBuffer::copyTexture()
{
   // size mismatch: create new texture
   if (mTempWidth != mWidth && mTempHeight != mHeight)
   {
      // delete old texture
      if (mTempTexture)
         glDeleteTextures(1, &mTempTexture);

      // create new texture
      glGenTextures(1, &mTempTexture);
      glBindTexture(GL_TEXTURE_2D, mTempTexture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
      mTempWidth= mWidth;
      mTempHeight= mHeight;
   }

   // create temporary framebuffer
   unsigned int target;
   glGenFramebuffers(1, &target);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, target);
   glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTempTexture, 0);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

   // copy framebuffers
   glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, mTarget );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, target );

   glReadBuffer( GL_COLOR_ATTACHMENT0 );
   glDrawBuffer( GL_COLOR_ATTACHMENT0 );

   glBlitFramebuffer( 0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST );

   glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, 0 );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, 0 );

   // remove temporary framebuffer
   glDeleteFramebuffers(1, &target);

   if (mInstance)
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, mInstance->target());

   // return texture
   return mTempTexture;
}


bool FrameBuffer::setResolution(int width, int height)
{
   if (mWidth == width && mHeight == height)
      return true;

   discard();

   GLenum status;

   // create texture
   glGenTextures(1, &mTexture);
   glBindTexture(GL_TEXTURE_2D, mTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // _MIPMAP_LINEAR
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   if (mFormatFlags & FloatColor)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, 0);
   else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   glGenFramebuffers(1, &mTarget);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget);

   if (mSamples > 1 && glRenderbufferStorageMultisample)
   {
      int wid,hei,fmt,sam;

      // create depth buffer
      if ((mFormatFlags & NoDepthBuffer) == 0)
      {
         glGenRenderbuffers(1, &mDepthBuffer);
         glBindRenderbuffer(GL_RENDERBUFFER_EXT, mDepthBuffer);
         glRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, mSamples, GL_DEPTH_COMPONENT24, width, height);
         status= glGetError();
         if (glGetRenderbufferParameteriv)
         {
            glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_WIDTH,&wid);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_HEIGHT,&hei);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_INTERNAL_FORMAT,&fmt);
            glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_SAMPLES,&sam);
            printf("Depth buffer: %dx%d, fmt=%4x, samples=%d\n",wid,hei,fmt,sam);
            mSamples= sam;
         }
         glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
      }

      // create color buffer
      glGenRenderbuffers(1, &mColorBuffer);
      glBindRenderbuffer(GL_RENDERBUFFER_EXT, mColorBuffer);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, mSamples, GL_RGBA, width, height);
      status= glGetError();
      if (glGetRenderbufferParameteriv)
      {
         glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_WIDTH,&wid);
         glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_HEIGHT,&hei);
         glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_INTERNAL_FORMAT,&fmt);
         glGetRenderbufferParameteriv(GL_RENDERBUFFER_EXT,GL_RENDERBUFFER_SAMPLES,&sam);
         printf("Colorbuffer: %dx%d, fmt=%4x, samples=%d\n",wid,hei,fmt,sam);
         mSamples= sam;
      }
      glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);

      // attach
      if ((mFormatFlags & NoDepthBuffer) == 0)
         glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthBuffer);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, mColorBuffer);


      status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
      if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
      {
         mSamples= -1;
         printf("FBO not ready\n");
      }

      // create final fbo and attach textures
      glGenFramebuffers(1, &mTempTarget);
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTempTarget);
      glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTexture, 0);
      glClear(GL_COLOR_BUFFER_BIT);
   }
   else
   {
      if ((mFormatFlags & NoDepthBuffer) == 0)
      {
         if (mFormatFlags & DepthTexture) // depth texture
         {
            // create depth texture and bind to framebuffer
            // depth textures only make sense for non multi sampled framebuffers
            // because downsampling depth textures gives is highly questionable results!
            glGenTextures(1, &mDepthTexture);
            glBindTexture(GL_TEXTURE_2D, mDepthTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // _MIPMAP_LINEAR
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mDepthTexture, 0);
         }
         else
         {
            // create and bind depth buffer
            glGenRenderbuffers(1, &mDepthBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER_EXT, mDepthBuffer);
            glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthBuffer);
         }
      }
      glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTexture, 0);
      mSamples= 1;
   }

   status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
   {
      printf("FBO not ready\n");
      mWidth= 0;
      mHeight= 0;
      mSamples= -1;
   }
   else
   {
      mWidth= width;
      mHeight= height;
   }

   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget);
   if ((mFormatFlags & NoDepthBuffer) == 0)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   else
      glClear(GL_COLOR_BUFFER_BIT);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

   return (mSamples >= 1);
}


void FrameBuffer::bind(int width, int height)
{
   mInstance= this;
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget);
   if (width && height)
      glViewport(0,0, width, height);
   else
      glViewport(0,0, mWidth, mHeight);
   mDirty= true;
}


void FrameBuffer::unbind()
{
   mInstance= 0;

   if (mSamples>1)
   {
      // blit multisampled fbo to texture

      GLenum status;
      glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, mTarget );
      status= glGetError();
//      if (status != 0)
      {
         glReadBuffer( GL_COLOR_ATTACHMENT0 );

         glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, mTempTarget );
         status= glGetError();
         glDrawBuffer( GL_COLOR_ATTACHMENT0 );

         glBlitFramebuffer( 0, 0, mWidth, mHeight, 0, 0, mWidth, mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST );

         glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, 0 );
         glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, 0 );
         glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
         status= glGetError();
         if (status != GL_NO_ERROR)
         {
            printf("error: %4x \n", (int)status);
         }
      }
   }

   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}


void FrameBuffer::push(FrameBuffer* fb)
{
   mStack.add( mInstance );
   if (fb)
      fb->bind();
}


void FrameBuffer::pop()
{
   if (mStack.size()>0)
   {
      FrameBuffer *prev= mStack.takeLast();
      if (prev)
         prev->bind();
      else
      {
         mInstance= 0;
         glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
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


int FrameBuffer::samples() const
{
   return mSamples;
}


bool FrameBuffer::resolutionChanged(int width, int height, int samples) const
{
   if (mWidth != width || mHeight != height)
      return true;
   if (samples>0 && samples != mSamples)
      return true;
   return false;
}


unsigned int FrameBuffer::texture() const
{
   return mTexture;
}

unsigned int FrameBuffer::depthTexture() const
{
   return mDepthTexture;
}

unsigned int FrameBuffer::target() const
{
   return mTarget;
}

void FrameBuffer::draw(float alpha)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   // bind texture
   glBindTexture(GL_TEXTURE_2D, mTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   glColor4f(1,1,1,alpha);

   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f( 1,-1,-1);
   glTexCoord2f(1,1); glVertex3f( 1, 1,-1);
   glTexCoord2f(0,1); glVertex3f(-1, 1,-1);
   glEnd();

   // restore projection
   glPopMatrix();

   // restore modelview
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
}
