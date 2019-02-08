#include "stdafx.h"
#include "renderdevice.h"

// static render-device object
RenderDevice *activeDevice= NULL;

RenderDevice::RenderDevice()
: mWidth(0),
  mHeight(0),
  mAspect(16.0f/9.0f),
  mActive(0),
  mAbort(0),
  mTime(0),
  mCurShader(0),
  mBorderLeft(0),
  mBorderTop(0),
  mBorderRight(0),
  mBorderBottom(0)
{
   for (int i = 0; i < 256; i++)
      keys[i] = 0;

   activeDevice= this;
   mAbort=0;
}

RenderDevice::~RenderDevice()
{
}

int RenderDevice::getBorderLeft() const
{
   return mBorderLeft;
}

int RenderDevice::getBorderBottom() const
{
   return mBorderBottom;
}

void RenderDevice::setKey(int num, int state)
{
   keys[num]= state;
}

int RenderDevice::getKey(int num)
{
   return keys[num];
}

void RenderDevice::setAbort()
{
   mAbort=1;
}

void RenderDevice::setActive(bool state)
{
   mActive= state;
}

bool RenderDevice::active()
{
   return (mActive != 0);
}

bool RenderDevice::abort()
{
   return (mAbort != 0);
}

Matrix RenderDevice::getCameraMatrix() const
{
   return mCamera;
}

void RenderDevice::setBorder(int left, int top, int right, int bottom)
{
   mBorderLeft= left;
   mBorderTop= top;
   mBorderRight= right;
   mBorderBottom= bottom;
}

float RenderDevice::getWidth() const
{
   return (float)mWidth - mBorderLeft - mBorderRight;
}


float RenderDevice::getHeight() const
{
   return (float)mHeight - mBorderTop - mBorderBottom;
}

