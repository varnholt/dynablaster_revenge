#include "playerinvincibleinstance.h"
#include "framework/gldevice.h"

PlayerInvincibleInstance::PlayerInvincibleInstance()
   : mMaterial(0)
   , mWidth(256)
   , mHeight(256)
   , mElapsed(0.0f)
   , mMin(0.0f)
   , mMax(0.0f)
   , mCenter(0.0f)
   , mFade(0.0f)
   , mRemove(false)
{
   // create double buffer texture to store vertex attributes
   for (int i=0; i<2; i++)
   {
      // double buffer for vertex positions (ping/pong rendering)
      glGenTextures(1, &mTexture[i]);
      glBindTexture(GL_TEXTURE_2D, mTexture[i]);
      if (i==0)
      {
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      }
      else
      {
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   // create double buffer framebuffers
   for (int i=0; i<2; i++)
   {
      glGenFramebuffers(1, &mTarget[i]);
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTexture[i], 0);
      glClear(GL_COLOR_BUFFER_BIT);
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
   }
}

PlayerInvincibleInstance::~PlayerInvincibleInstance()
{
   glDeleteFramebuffers(1, &mTarget[0]);
   glDeleteFramebuffers(1, &mTarget[1]);
   glDeleteTextures(1, &mTexture[0]);
   glDeleteTextures(1, &mTexture[1]);
}

void PlayerInvincibleInstance::remove()
{
    mRemove= true;
}

void PlayerInvincibleInstance::setRemove(bool remove)
{
    mRemove= remove;
}

bool PlayerInvincibleInstance::update( float dt )
{
   dt*=0.025f;
   if (mRemove)
   {
      if (mFade > 0.0f)
      {
         mFade -= dt;
         if (mFade <= 0.0f)
         {
            mFade= 0.0f;
            return false;
         }
      }
      else
      {
         return false;
      }
   }
   else
   {
      if (mFade < 1.0f)
      {
         mFade += dt;
         if (mFade > 1.0f)
            mFade= 1.0f;
      }
   }
   return true;
}

void PlayerInvincibleInstance::setMaterial( Material* mat )
{
   mMaterial= mat;
}

Material* PlayerInvincibleInstance::getMaterial() const
{
   return mMaterial;
}

int PlayerInvincibleInstance::width() const
{
   return mWidth;
}

int PlayerInvincibleInstance::height() const
{
   return mHeight;
}

unsigned int PlayerInvincibleInstance::texture(int id) const
{
   return mTexture[id];
}

void PlayerInvincibleInstance::bind(int id)
{
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget[id]);
   glViewport(0,0,mWidth,mHeight);
}

void PlayerInvincibleInstance::unbind()
{
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}

void PlayerInvincibleInstance::setCenter(const Vector& center)
{
   mCenter= center;
}

const Vector& PlayerInvincibleInstance::getCenter() const
{
   return mCenter;
}

void PlayerInvincibleInstance::setRect(const Vector& min, const Vector& max)
{
   mMin= min;
   mMax= max;
}

const Vector& PlayerInvincibleInstance::min2d() const
{
   return mMin;
}

const Vector& PlayerInvincibleInstance::max2d() const
{
   return mMax;
}

float PlayerInvincibleInstance::getFade() const
{
   return mFade;
}

void PlayerInvincibleInstance::setFade(float fade)
{
   mFade= fade;
}
