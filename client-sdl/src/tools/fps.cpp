#include "fps.h"

#include <SDL3/SDL.h>

namespace
{
// SDL_GetTicks() instead of the Windows-only GetTickCount() - same millisecond tick, works on
// every platform SDL supports.
unsigned int GetTickCount()
{
   return static_cast<unsigned int>(SDL_GetTicks());
}
}  // namespace

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

