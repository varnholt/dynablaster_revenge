#include "sounddevice.h"
#include "tools/array.h"
#include "audiosourceinterface.h"
#include "audiomixer.h"
#include <memory.h>
#include <stdio.h>
#include <QtGlobal>

static int mStop= -1;

SoundDevice::SoundDevice()
: SoundDeviceInterface()
#ifdef _LINUX_
, mWave(0)
, mHandler(0)
, mHWParams(0)
, mSWParams(0)
#endif
{
}

SoundDevice::~SoundDevice()
{
}


/*
#ifdef Q_OS_LINUX
static void callback(snd_async_handler_t *ahandler)
{
     snd_pcm_t *wave = snd_async_handler_get_pcm(ahandler);
     snd_pcm_sframes_t avail;

     SoundDevice *device= SoundDevice::Instance();

     avail = snd_pcm_avail_update(wave);
     while (avail >= device->getBlockSize())
     {
        device->processBlock();

        avail = snd_pcm_avail_update(wave);
     }
}
#endif
*/

#ifdef WIN32
static int run(unsigned int* /*param*/)
{
    SoundDevice *device= dynamic_cast<SoundDevice*>(SoundDevice::Instance());

    while (mStop==0)
    {
        int playingBlock= device->getCurrentBlock(); // block that's currently playing

        while (device->getProcessingBlock() != playingBlock)
        {
            //            printf("%x", device->getProcessingBlock());
            device->processBlock();
        }
        Sleep(1);
    };
    mStop=2;

    return 0;
}
#endif

#ifdef Q_OS_LINUX
void* run(void*)
{
    SoundDevice *device= (SoundDevice*)SoundDeviceInterface::Instance();

    snd_pcm_t *wave = device->mWave;
    snd_pcm_sframes_t avail;

    while (mStop==0)
    {
        avail = snd_pcm_avail_update(wave);
        while (avail >= device->getBlockSize())
        {
            device->processBlock();

            avail = snd_pcm_avail_update(wave);
        }
        usleep(10000);
    }
    mStop=2;

    return 0;
}
#endif

#ifdef __APPLE__
void AudioOutputCallback(
        void* inUserData,
        AudioQueueRef outAQ,
        AudioQueueBufferRef outBuffer)
{
    SoundDevice* soundDevice = (SoundDevice*)inUserData;
   // qDebug("Blah :)");
    soundDevice->mCurrentBuffer = outBuffer;
    int playingBlock = soundDevice->getCurrentBlock(); // block that's currently playing

   // while (soundDevice->getProcessingBlock() != playingBlock)
  //  {
//        qDebug("%x", soundDevice->getProcessingBlock());
  //      qDebug("While :P");
        soundDevice->processBlock();
//    }
}

#endif

bool SoundDevice::getCardNum(int& cardNum, int& devNum)
{
    // currently found "hw:<card>,<device>"
    devNum = -1;
    cardNum = -1;

#ifdef Q_OS_LINUX
    int err;
    int enumCard= -1;

    for (;;)
    {
        snd_ctl_t *cardHandle;

        // Get next sound card's card number. When "enumCard" == -1, then ALSA
        // fetches the first card
        if ((err = snd_card_next(&enumCard)) < 0)
        {
            printf("Can't get the next card number: %s\n", snd_strerror(err));
            break;
        }

        // No more cards? ALSA sets "enumCard" to -1 if so
        if (enumCard < 0) break;

        // Open this card's control interface. We specify only the card number -- not
        // any device nor sub-device too
        char   str[64];
        sprintf(str, "hw:%i", enumCard);
        if ((err = snd_ctl_open(&cardHandle, str, 0)) < 0)
        {
            continue;
        }

        // Start with the first wave device on this card
        int enumDev = -1;

        for (;;)
        {
            snd_pcm_info_t  *pcmInfo;
            int subDevCount, i;

            // Get the number of the next wave device on this card
            if ((err = snd_ctl_pcm_next_device(cardHandle, &enumDev)) < 0)
            {
                break;
            }

            // No more wave devices on this card? ALSA sets "enumDev" to -1 if so.
            // NOTE: It's possible that this sound card may have no wave devices on it
            // at all, for example if it's only a MIDI card
            if (enumDev < 0)
                break;

            // To get some info about the subdevices of this wave device (on the card), we need a
            // snd_pcm_info_t, so let's allocate one on the stack
            snd_pcm_info_alloca(&pcmInfo);
            memset(pcmInfo, 0, snd_pcm_info_sizeof());

            // Tell ALSA which device (number) we want info about
            snd_pcm_info_set_device(pcmInfo, enumDev);

            // Get info on the wave outs of this device
            snd_pcm_info_set_stream(pcmInfo, SND_PCM_STREAM_PLAYBACK);

            i = -1;
            subDevCount = 1;
            int prevId= -1;

            // More subdevices?
            while (++i < subDevCount)
            {
                // Tell ALSA to fill in our snd_pcm_info_t with info on this subdevice
                snd_pcm_info_set_subdevice(pcmInfo, i);
                if ((err = snd_ctl_pcm_info(cardHandle, pcmInfo)) < 0)
                {
                    continue;
                }

                // Print out how many subdevices (once only)
                if (!i)
                {
                    subDevCount = snd_pcm_info_get_subdevices_count(pcmInfo);
                }

                struct test_struct {
                    unsigned int device;		/* RO/WR (control): device number */
                    unsigned int subdevice;		/* RO/WR (control): subdevice number */
                    int stream;			/* RO/WR (control): stream direction */
                    int card;			/* R: card number */
                    unsigned char id[64];		/* ID (user selectable) */
                    unsigned char name[80];		/* name of this device */
                    unsigned char subname[32];	/* subdevice name */
                    int dev_class;			/* SNDRV_PCM_CLASS_* */
                    int dev_subclass;		/* SNDRV_PCM_SUBCLASS_* */
                    unsigned int subdevices_count;
                    unsigned int subdevices_avail;
                };

                test_struct* temp= (test_struct*)pcmInfo;

                // NOTE: If there's only one subdevice, then the subdevice number is immaterial,
                // and can be omitted when you specify the hardware name
                if (prevId != enumCard)
                {
                    printf("sounddevice %d,%d: %s \n", enumCard, enumDev, temp->id);
                    prevId= enumCard;
                }

                if (cardNum == -1 && devNum == -1)
                {
                    cardNum= enumCard;
                    devNum= enumDev;
                }
            }
        }

        // Close the card's control interface after we're done with it
        snd_ctl_close(cardHandle);
    }

    // ALSA allocates some mem to load its config file when we call some of the
    // above functions. Now that we're done getting the info, let's tell ALSA
    // to unload the info and free up that mem
    snd_config_update_free_global();
#endif
    return (cardNum>=0) && (devNum>=0);
}


bool SoundDevice::init(unsigned int hz, int blocksize, int blockcount)
{
    // store settings
    mFrequency= hz;
    mBlockSize= blocksize;
    mBlockCount= blockcount;


#ifdef Q_OS_LINUX
    int err;
    char device[64];

    mWave= 0;

    // initially try "default"
    strcpy(device, "default");
    err = snd_pcm_open(&mWave, device, SND_PCM_STREAM_PLAYBACK, 0);

    if (err < 0)
    {
        // "default" doesn't work: enumerate sound devices and pick first plausible one
        int cardNum, devNum;
        if (!getCardNum(cardNum, devNum))
            return false;

        sprintf(device, "plughw:%d,%d", cardNum, devNum);

        //   mOutput= 0;
        //   err = snd_output_stdio_attach(&mOutput, stdout, 0);
        //   if (err < 0) return false;

        err = snd_pcm_open(&mWave, device, SND_PCM_STREAM_PLAYBACK, 0);
        if (err < 0) return false;
    }


    snd_pcm_hw_params_alloca(&mHWParams);
    snd_pcm_sw_params_alloca(&mSWParams);

    int dir= 1;

    /* choose all parameters */
    err = snd_pcm_hw_params_any(mWave, mHWParams);
    if (err < 0) return false;

    /* set hardware resampling */
    err = snd_pcm_hw_params_set_rate_resample(mWave, mHWParams, 1);
    if (err < 0) return false;

    /* set the interleaved read/write format */
    err = snd_pcm_hw_params_set_access(mWave, mHWParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) return false;

    /* set the sample format */
    err = snd_pcm_hw_params_set_format(mWave, mHWParams, SND_PCM_FORMAT_S16);
    if (err < 0) return false;

    /* set the count of channels */
    err = snd_pcm_hw_params_set_channels(mWave, mHWParams, 2);
    if (err < 0) return false;

    /* set the stream rate */
    err = snd_pcm_hw_params_set_rate_near(mWave, mHWParams, &mFrequency, 0);
    if (err < 0) return false;
    if (hz != mFrequency) return false;

    /* set the buffer time */
    //   unsigned int buffer_time = 500000;               /* ring buffer length in us */
    //   err = snd_pcm_hw_params_set_buffer_time_near(mWave, mHWParams, &buffer_time, &dir);
    //   err = snd_pcm_hw_params_get_buffer_size(mHWParams, &total);
    snd_pcm_uframes_t total = mBlockSize * mBlockCount;
    err = snd_pcm_hw_params_set_buffer_size_near(mWave, mHWParams, &total);
    if (err < 0) return false;

    /* set the period time */
    //   unsigned int period_time = mBlockSize*1000000/mFrequency;/* period time in us */
    //   err = snd_pcm_hw_params_set_period_time_near(mWave, mHWParams, &period_time, &dir);
    //   err = snd_pcm_hw_params_get_period_size(mHWParams, (snd_pcm_uframes_t*)&mBlockSize, &dir);
    err = snd_pcm_hw_params_set_period_size_near(mWave, mHWParams, &mBlockSize, &dir);
    if (err < 0) return false;

    mBlockCount= total / mBlockSize;

    // write the parameters to device
    err = snd_pcm_hw_params(mWave, mHWParams);
    if (err < 0) return false;

    /* get the current swparams */
    err = snd_pcm_sw_params_current(mWave, mSWParams);
    if (err < 0) return false;

    /* start the transfer when the buffer is almost full: */
    err = snd_pcm_sw_params_set_start_threshold(mWave, mSWParams, total);
    if (err < 0) return false;

    /* allow the transfer when at least period_size samples can be processed */
    /* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
    err = snd_pcm_sw_params_set_avail_min(mWave, mSWParams, total);
    if (err < 0) return false;

    /* enable period events when requested */
    err = snd_pcm_sw_params_set_period_event(mWave, mSWParams, 1);
    if (err < 0) return false;

    /* write the parameters to the playback device */
    err = snd_pcm_sw_params(mWave, mSWParams);
    if (err < 0) return false;

    //   err = snd_async_add_pcm_handler(&mHandler, mWave, callback, 0);
    //   if (err < 0) return false;

    mBuffer= new short[mBlockSize*2];
    memset(mBuffer, 0, mBlockSize<<2);
    for (int count = 0; count < 2; count++)
    {
        err = snd_pcm_writei(mWave, mBuffer, mBlockSize);
        if (err < 0)
            return false;
    }

    mProcessingBlock= 2;

#endif

#ifdef __APPLE__
    UInt32 propertySize;
    AudioObjectPropertyAddress propertyAddress;
    AudioDeviceID outputDeviceID;
    OSStatus status = 0;

    propertyAddress.mSelector = kAudioHardwarePropertyDevices;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;
    AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &propertySize);
    propertySize = propertySize / sizeof(AudioDeviceID);

    qDebug("Number of cards: %d", propertySize);
    if (! propertySize) {
        return false;
    }

    // Get default output device
    propertySize = sizeof(AudioDeviceID);
    propertyAddress.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;
    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &propertySize, &outputDeviceID);

    if (status) {
        qDebug("Failed with status %d", status);
        return false;
    }

    if (kAudioDeviceUnknown == outputDeviceID) {
        qDebug("Unknown audio device");
        return false;
    }

    // Configure audio format
    mDataFormat.mSampleRate = mFrequency;
    mDataFormat.mFormatID = kAudioFormatLinearPCM;
    mDataFormat.mBitsPerChannel = 16;
    mDataFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
    mDataFormat.mBytesPerFrame = 4;
    mDataFormat.mBytesPerPacket = 4;
    mDataFormat.mFramesPerPacket = 1;
    mDataFormat.mChannelsPerFrame = 2;
    mDataFormat.mReserved = 0;

    qDebug("Audio init done");
#endif

#ifdef WIN32   
    WAVEFORMATEX	pcmwf;
    UINT			   hr;

    // create sample buffer
    int size= mBlockSize * mBlockCount << 1;
    mBuffer= new short[size];
    memset(mBuffer, 0, size<<1);

    pcmwf.wFormatTag		= WAVE_FORMAT_PCM;
    pcmwf.nChannels		= 2;
    pcmwf.nSamplesPerSec	= mFrequency;
    pcmwf.nAvgBytesPerSec= pcmwf.nSamplesPerSec * pcmwf.nChannels * 16 >> 3;
    pcmwf.nBlockAlign		= pcmwf.nChannels * 16 >> 3;
    pcmwf.wBitsPerSample	= 16;
    pcmwf.cbSize			= 0;

    hr = waveOutOpen(&mHandle, WAVE_MAPPER, &pcmwf, 0, 0, 0);
    if (hr)
        return false;

    mWave.lpData			= (LPSTR)mBuffer;
    mWave.dwBufferLength	= size<<1;
    mWave.dwBytesRecorded= 0;
    mWave.dwUser			= 0;
    mWave.dwFlags			= WHDR_BEGINLOOP | WHDR_ENDLOOP;
    mWave.dwLoops			= -1;

    hr = waveOutPrepareHeader(mHandle, &mWave, sizeof(WAVEHDR));
    if (hr)
        return false;
#endif

    mMixer= AudioMixer::Instance();

    return true;
}

int SoundDevice::postBlock(short *data)
{
    int res= 0;


#ifdef Q_OS_LINUX
    int size= mBlockSize;
    res= snd_pcm_writei(mWave, data, size);

    snd_pcm_sframes_t frames;
    snd_pcm_delay(mWave, &frames);

    if (res < 0)
        printf("Write error: %s\n", snd_strerror(res));

    if (res != size)
    {
        //             printf("Write error: written %i expected %li\n", err, size);
    }

#endif    

#ifdef WIN32
    short *dst= mBuffer + (mBlockSize*mProcessingBlock*2);
    memcpy(dst, data, mBlockSize*4);
#endif

#ifdef __APPLE__
    if (mStop == 2) return res;

    mCurrentBuffer->mAudioDataByteSize = 4 * mBlockSize;
    memcpy(mCurrentBuffer->mAudioData, data, 4 * mBlockSize);
    AudioQueueEnqueueBuffer(mQueue, mCurrentBuffer, 0, NULL);
#endif

    mProcessingBlock++;
    if (mProcessingBlock >= mBlockCount)
        mProcessingBlock= 0;

    return res;
}

int SoundDevice::getCurrentBlock()
{
#ifdef WIN32  
    MMTIME	mmt;

    mmt.wType = TIME_SAMPLES;
    waveOutGetPosition(mHandle, &mmt, sizeof(MMTIME));

    return (mmt.u.sample / mBlockSize) % mBlockCount;
#endif

#ifdef Q_OS_LINUX
    return mProcessingBlock;
#endif
}

int SoundDevice::getTotalBlock() const
{
#ifdef WIN32  
    MMTIME	mmt;

    mmt.wType = TIME_SAMPLES;
    waveOutGetPosition(mHandle, &mmt, sizeof(MMTIME));

    return (mmt.u.sample / mBlockSize);
#endif

#ifdef Q_OS_LINUX
    return 0;
#endif
}

bool SoundDevice::start()
{
    mStop = 0;

#ifdef WIN32
    DWORD	threadID;

    for (int i=0;i<mBlockCount;i++)
        void processBlock();

    mThread= CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)run, 0,0, &threadID);
    SetThreadPriority(mThread, THREAD_PRIORITY_TIME_CRITICAL);	// THREAD_PRIORITY_HIGHEST);

    waveOutWrite(mHandle, &mWave, sizeof(WAVEHDR));
#endif

#ifdef Q_OS_LINUX
    if (mWave && snd_pcm_state(mWave) == SND_PCM_STATE_PREPARED)
    {
        for (int i=0;i<mBlockCount;i++)
            void processBlock();

        pthread_create(&mThread, NULL, run, 0);

        int err = snd_pcm_start(mWave);
        if (err < 0) return false;
    }
#endif

#ifdef __APPLE__
    qDebug("Starting up audio engine");
    OSStatus err;

    qDebug("AudioQueueNewOutput() calling");
    err = AudioQueueNewOutput (&mDataFormat, AudioOutputCallback, this, NULL, NULL, 0, &mQueue);
    if (err != noErr) {
        qDebug("AudioQueueNewOutput() error: %d", err);
        return false;
    }
    // Enqueue buffers
    for (int i = 0; i < 3; i++) {
        err = AudioQueueAllocateBuffer(mQueue,  4 * mBlockSize, &mBuffers[i]);
        if (err != noErr) {
            qDebug("AudioQueueAllocateBuffer() error: %d", err);
            return false;
        }
        AudioOutputCallback(this, mQueue, mBuffers[i]);
    }

    err = AudioQueueStart(mQueue, NULL);
    if (err != noErr) {
        qDebug("failed to start audio queue %d\n", err);
        return false;
    }
#endif
    mIsRunning = true;
    return true;
}

void SoundDevice::stop()
{
#ifdef WIN32  
    // wait for thread to end
    if (mStop==0)
    {
        mStop=1;
        while (mStop!=2)
            Sleep(100);
    }

    waveOutUnprepareHeader(mHandle, &mWave, sizeof(WAVEHDR));
    mWave.dwFlags &= ~WHDR_PREPARED;

    delete[] mBuffer;
    mBuffer= NULL;

    waveOutReset(mHandle);

    waveOutClose(mHandle);
#endif

#ifdef Q_OS_LINUX
    // wait for thread to end
    if (mStop==0)
    {
        mStop=1;
        while (mStop!=2)
            sleep(100);
    }

    if (mWave != 0)
        snd_pcm_close(mWave);
#endif
}
