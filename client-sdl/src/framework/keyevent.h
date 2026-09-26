#pragma once

#include <SDL3/SDL_keycode.h>

#include <string>

// SDL-native key event, mirroring the subset of QKeyEvent's accessor shape actually used
class KeyEvent
{
public:
   KeyEvent() = default;
   KeyEvent(SDL_Keycode key, std::string text, bool auto_repeat);

   SDL_Keycode key() const;
   const std::string& text() const;
   bool isAutoRepeat() const;

private:
   SDL_Keycode _key = SDLK_UNKNOWN;
   std::string _text;
   bool _auto_repeat = false;
};
