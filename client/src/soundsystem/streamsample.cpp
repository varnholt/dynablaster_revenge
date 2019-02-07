#include "streamsample.h"
#include <memory.h>

StreamSample::StreamSample(int size)
: AudioSample()
{
   mBuffer= new short[size+2];
   mLength= size;
   mLoopStart= 0;
   mLoopLen= size;
   mLoopMode= AudioSample::NormalLoop;
   
   memset(mBuffer, 0, (size+2)*2);
}

