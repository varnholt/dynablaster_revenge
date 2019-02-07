#include "stdafx.h"
#include "fps.h"


#ifndef WIN32
unsigned int GetTickCount() { return 0; }
#endif


FPS::FPS(int period)
{
   if (period<=0) period=1;
   mPeriod= period;
   mCurFPS= 0.0f;
   mFrame= 0;
   mTime= 0;
}


FPS::~FPS()
{
}


void FPS::next()
{
   mFrame++;
   if (mFrame >= mPeriod)
   {
      int curTime= GetTickCount();
      mCurFPS= (float) mFrame * 1000 / (curTime - mTime); 
      mTime= curTime;
      mFrame= 0;
   }
}


float FPS::get()
{
   return mCurFPS;
}

