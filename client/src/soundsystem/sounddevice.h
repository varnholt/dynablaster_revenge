#pragma once

#include "sounddeviceinterface.h"

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#ifdef _LINUX_
#include <alsa/asoundlib.h>
#include <pthread.h>
#endif

#ifdef __APPLE__
#include <CoreAudio/CoreAudio.h>
#include <CoreServices/CoreServices.h>
#include <AudioToolBox/AudioToolbox.h>
#endif

class SoundDevice : public SoundDeviceInterface
{
public:
   SoundDevice();
   ~SoundDevice();

    bool init(unsigned int hz=44100, int blocksize= 256, int blockcount= 64);
    bool start();
    void stop();
    int postBlock(short*);
    int getTotalBlock() const;
    int getCurrentBlock();
    bool getCardNum(int& cardNum, int& devNum);

private:
#ifdef WIN32   
    WAVEHDR              mWave;
    HWAVEOUT             mHandle;
    HANDLE               mThread;
#endif

#ifdef _LINUX_
public:
    snd_pcm_t*           mWave;
    snd_async_handler_t* mHandler;

    snd_pcm_hw_params_t* mHWParams;
    snd_pcm_sw_params_t* mSWParams;
    pthread_t            mThread;
#endif

#ifdef __APPLE__
public:
    AudioQueueBufferRef           mCurrentBuffer;
private:
    AudioStreamBasicDescription   mDataFormat;
    AudioQueueRef                 mQueue;
    AudioQueueBufferRef           mBuffers[3];
    AudioFileID                   mAudioFile;
    UInt32                        bufferByteSize;
    SInt64                        mCurrentPacket;
    UInt32                        mNumPacketsToRead;
    AudioStreamPacketDescription  *mPacketDescs;
#endif
    bool                          mIsRunning;
};
