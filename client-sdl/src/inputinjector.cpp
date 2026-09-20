#include "inputinjector.h"

#include <SDL3/SDL.h>

void InputInjector::pressAndRelease(uint32_t keycode)
{
   SDL_Event down{};
   down.type = SDL_EVENT_KEY_DOWN;
   down.key.key = static_cast<SDL_Keycode>(keycode);
   down.key.down = true;
   SDL_PushEvent(&down);

   SDL_Event up{};
   up.type = SDL_EVENT_KEY_UP;
   up.key.key = static_cast<SDL_Keycode>(keycode);
   up.key.down = false;
   SDL_PushEvent(&up);
}

void InputInjector::queue(const std::vector<Step>& steps)
{
   _steps = steps;
   _current = 0;
   _frames_in_current_step = 0;
}

void InputInjector::advance()
{
   if (finished())
      return;

   if (_frames_in_current_step == 0)
   {
      SDL_Event down{};
      down.type = SDL_EVENT_KEY_DOWN;
      down.key.key = static_cast<SDL_Keycode>(_steps[_current].keycode);
      down.key.down = true;
      SDL_PushEvent(&down);
   }

   ++_frames_in_current_step;

   if (_frames_in_current_step >= _steps[_current].hold_frames)
   {
      SDL_Event up{};
      up.type = SDL_EVENT_KEY_UP;
      up.key.key = static_cast<SDL_Keycode>(_steps[_current].keycode);
      up.key.down = false;
      SDL_PushEvent(&up);

      ++_current;
      _frames_in_current_step = 0;
   }
}

bool InputInjector::finished() const
{
   return _current >= _steps.size();
}
