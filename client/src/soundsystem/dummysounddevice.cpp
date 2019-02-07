#include "dummysounddevice.h"
#include "tools/array.h"
#include "audiosourceinterface.h"
#include "audiomixer.h"
#include "framework/globaltime.h"
#include <memory.h>
#include <stdio.h>

DummySoundDevice::DummySoundDevice(FILE* captureFile)
: QObject()
, SoundDeviceInterface()
, mDone(0.0f)
, mCaptureFile(captureFile)
, mCaptureEnabled(false)
{
   mUpdate= new QTimer(this);
   connect(mUpdate, SIGNAL(timeout()), this, SLOT(update()), Qt::QueuedConnection);
}

DummySoundDevice::~DummySoundDevice()
{
}

bool DummySoundDevice::init(unsigned int hz, int blocksize, int blockcount)
{
   // store settings
   mFrequency= hz;
   mBlockSize= blocksize;
   mBlockCount= blockcount;

   mMixer= AudioMixer::Instance();

   return true;
}

void DummySoundDevice::update()
{
   float curTime= GlobalTime::Instance()->getTime();
   float blockTime= (float)mBlockSize / mFrequency;
   while (mDone + blockTime <= curTime)
   {
      processBlock();
      mDone+= blockTime;
   }
}

int DummySoundDevice::postBlock(short *data)
{
   if (mCaptureFile && mCaptureEnabled)
      fwrite(data, 4, mBlockSize, mCaptureFile);
   return 0;
}

int DummySoundDevice::getCurrentBlock()
{
   return 0;
}

int DummySoundDevice::getTotalBlock() const
{
   return 0;
}

bool DummySoundDevice::start()
{
   mDone= GlobalTime::Instance()->getTime();
   mUpdate->start();
   return true;
}

void DummySoundDevice::stop()
{
   mUpdate->stop();
}

void DummySoundDevice::startCapture()
{
   mCaptureEnabled= true;
}

void DummySoundDevice::stopCapture()
{
   mCaptureEnabled= false;
}
