// header
#include "videooptionssdl.h"

// sdl
#ifdef SDL1
   #include "../sdl/SDL.h"
#endif


VideoOptionsSDL::VideoOptionsSDL()
 : VideoOptions()
{
}


void VideoOptionsSDL::initialize()
{
   #ifdef SDL1
      SDL_Init(SDL_INIT_VIDEO);
   #endif

   VideoOptions::initialize();
}


void VideoOptionsSDL::initializeModes()
{ 
   #ifdef SDL1
      SDL_Rect **modes;

      //  get available fullscreen/hardware modes
      modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);

      // check is there are modes available at all
      if (modes == (SDL_Rect **)0)
      {
         qDebug("VideoOptionsSDL::initializeModes(): No modes available!");
      }
      else
      {
         if (modes == (SDL_Rect **)-1)
         {
            qDebug("VideoOptionsSDL::initializeModes(): All resolutions available.");
         }

         float aspect = modes[0]->w / (float)modes[0]->h;
         float currentAspect = 1.0f;
         int index = 0;

         for (int i=0; modes[i]; i++)
         {
            currentAspect = modes[i]->w / (float)modes[i]->h;

            qDebug(
               "VideoOptionsSDL::initializeModes(): %d x %d (aspect: %f)",
               modes[i]->w,
               modes[i]->h,
               currentAspect
            );

            if (
                  !isAspectFixed()
               || (aspect == currentAspect)
            )
            {
   //            mResolutions.append(VideoMode(index, modes[i]->w, modes[i]->h));
               index++;
            }
         }
      }
   #endif
}


