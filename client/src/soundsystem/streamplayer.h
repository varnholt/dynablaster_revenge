#pragma once

#include <QObject>
#include <QMutex>
#include "audiosourceinterface.h"

class AudioSample;
class MixerChannel;
class MpegStream;
class Id3Tag;

class StreamPlayer : public QObject, public AudioSourceInterface
{
   Q_OBJECT

public:
   StreamPlayer();
   ~StreamPlayer();

   bool play(char *buffer, int size);
   void stop();
   void pause();

   int available() const;
   int process(int numSamples);
   int capacity() const;

   const Id3Tag& getId3() const;

   virtual void setVolume(float);

   virtual void toggleMute();
   virtual bool isMuted() const;


signals:
   void finished();

private:
   void decode(int size);
   void enqueue(short *src, int size);
   void dequeue(int size);
   void storeData(short *dst, short *src, int size);

   mutable QMutex mMutex;
   MpegStream*    mStream;       // mp3 stream decoder
   int            mChannels;     // number of channels
   AudioSample*   mSample[2];    // ring buffer samples for left/right
   MixerChannel*  mChannel[2];   // mixer channels
   char*          mStreamData;   // mp3 file

   // ring buffer for decoded data
   short*         mBuffer[2];    // decoded mp3 data
   int            mSize;         // size of buffer(s)
   int            mBack;         // back end
   int            mFront;        // fresh data

   int            mBlock;        // last filled block in sample
   int            mBlockCount;   // number of total blocks in sample
   bool           mFinished;
};
