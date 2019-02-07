#pragma once

class Stream;

class AudioSample
{       
public:
   enum LoopMode
   {
      NoLoop= 1,
      NormalLoop= 2,
      BiDirLoop= 4
   };

   AudioSample();
   AudioSample(int size, int rate);
   virtual ~AudioSample();

   virtual bool load(Stream *stream);
   virtual bool save(Stream *stream);

   void clear();

   short       	*mBuffer;		// pointer to sound data
   int            mLength;      	// sample length (samples)
   int            mLoopStart;   	// sample loop start (samples)
   int            mLoopLen;     	// sample loop length (samples)
   unsigned char	mLoopMode;    	// loop type 
   int            mSampleRate;
};
