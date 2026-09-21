#include "glescontext.h"

#include "gles3.h"

bool GlesContext::init(const std::string& title, int width, int height)
{
   if (!SDL_Init(SDL_INIT_VIDEO))
   {
      SDL_Log("SDL_Init failed: %s", SDL_GetError());
      return false;
   }

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

   _window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
   if (_window == nullptr)
   {
      SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
      return false;
   }

   // On Windows, a freshly created window launched from a console/IDE doesn't always receive
   // input focus automatically (the OS's focus-stealing prevention can leave it behind the
   // launching process's window) - real mouse clicks then never reach this window's SDL event
   // queue at all, even though the window is visibly on screen. Explicitly claim focus so mouse
   // input actually arrives.
   SDL_RaiseWindow(_window);

   _context = SDL_GL_CreateContext(_window);
   if (_context == nullptr)
   {
      SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
      return false;
   }

   if (!loadGles3Functions())
   {
      SDL_Log("failed to resolve one or more GLES3 entry points");
      return false;
   }

   SDL_GL_SetSwapInterval(1);

   _width = width;
   _height = height;

   return true;
}

void GlesContext::swap() const
{
   SDL_GL_SwapWindow(_window);
}

GlesContext::~GlesContext()
{
   if (_context != nullptr)
   {
      SDL_GL_DestroyContext(_context);
   }

   if (_window != nullptr)
   {
      SDL_DestroyWindow(_window);
   }

   SDL_Quit();
}
