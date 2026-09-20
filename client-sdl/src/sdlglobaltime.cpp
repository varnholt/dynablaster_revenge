#include "sdlglobaltime.h"

#include <SDL3/SDL.h>

void SdlGlobalTime::update()
{
   mTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
}

float SdlGlobalTime::getTime() const
{
   return mTime;
}
