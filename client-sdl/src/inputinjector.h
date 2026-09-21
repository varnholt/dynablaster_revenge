#pragma once

#include <cstdint>
#include <vector>

/// \brief pushes synthetic keyboard events onto SDL's event queue.
/// lets a headless self-test run drive the game exactly like a player would - move, place a
/// bomb, open a menu - without anyone touching a keyboard.
class InputInjector
{
public:
   struct Step
   {
      uint32_t keycode;  //!< SDLK_* value
      int hold_frames;   //!< frames the key stays "down" before the key-up
   };

   /// \brief pushes a key-down immediately followed by a key-up.
   /// \param keycode SDLK_* value.
   static void pressAndRelease(uint32_t keycode);

   /// \brief queues a scripted sequence, fed one step per frame from the main loop via advance().
   /// used by --selftest to script a short playback.
   /// \param steps ordered key presses with hold durations.
   void queue(const std::vector<Step>& steps);

   /// \brief advances the scripted playback by one frame.
   void advance();

   /// \brief indicates whether the queued sequence has fully played back.
   bool finished() const;

private:
   std::vector<Step> _steps;
   size_t _current = 0;
   int _frames_in_current_step = 0;
};
