#pragma once

#include "sounddeviceinterface.h"
#include <QObject>
#include <QTimer>

class DummySoundDevice : public QObject, public SoundDeviceInterface
{
   Q_OBJECT

public:
   DummySoundDevice(FILE* captureFile = 0);
   ~DummySoundDevice();

   bool init(unsigned int hz=44100, int blocksize= 256, int blockcount= 64);
   bool start();
   void stop();
   int postBlock(short*);
   int getTotalBlock() const;
   int getCurrentBlock();

   void startCapture();
   void stopCapture();

private slots:
   void update();

private:
   QTimer* mUpdate;
   float   mDone;
   FILE*   mCaptureFile;
   bool    mCaptureEnabled;
};
