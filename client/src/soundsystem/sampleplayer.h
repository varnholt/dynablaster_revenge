#pragma once

// base
#include "audiosourceinterface.h"

// engine
#include "math/vector.h"
#include "tools/array.h"

// Qt
#include <QObject>

class AudioSample;
class MixerChannel;
class SoundEffect;

class QFileSystemWatcher;

class SamplePlayer : public QObject, public AudioSourceInterface
{
   Q_OBJECT

public:
   SamplePlayer();
   ~SamplePlayer();

   int addSample(const char *filename);
   void play(int id, const Vector& pos = Vector(0,0,0));

   void SetData(short *left, short *right, int size);

   int process(int numSamples);
   int capacity() const;
   int getChannelCount() const;

   virtual void toggleMute();
   virtual bool isMuted() const;

private slots:
   void fileChanged(const QString&);

private:
   Array<SoundEffect*>  mSamples;
   Array<MixerChannel*> mChannels;
   char*                mFilename;
   QFileSystemWatcher*  mFileWatch;
};
