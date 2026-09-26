#include "keyevent.h"

KeyEvent::KeyEvent(SDL_Keycode key, std::string text, bool auto_repeat) : _key(key), _text(std::move(text)), _auto_repeat(auto_repeat)
{
}

SDL_Keycode KeyEvent::key() const
{
   return _key;
}

const std::string& KeyEvent::text() const
{
   return _text;
}

bool KeyEvent::isAutoRepeat() const
{
   return _auto_repeat;
}
