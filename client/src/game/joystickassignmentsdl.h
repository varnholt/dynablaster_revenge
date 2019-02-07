#ifndef JOYSTICKASSIGNMENTSDL_H
#define JOYSTICKASSIGNMENTSDL_H

// base
#include "controllerassignment.h"

#include "sdl2/SDL.h"

class JoystickAssignmentSDL : public ControllerAssignment
{
   public:

      JoystickAssignmentSDL();

      //! main controls
      void setUp(SDL_GameControllerButton);
      SDL_GameControllerButton getUp() const;

      void setDown(SDL_GameControllerButton);
      SDL_GameControllerButton getDown() const;

      void setLeft(SDL_GameControllerButton);
      SDL_GameControllerButton getLeft() const;

      void setRight(SDL_GameControllerButton);
      SDL_GameControllerButton getRight() const;

      void setBomb(SDL_GameControllerButton);
      SDL_GameControllerButton getBomb() const;

      //! end game
      void setEndGame(SDL_GameControllerButton);
      SDL_GameControllerButton getEndGame() const;

      //! music controls
      void setTrackNext(SDL_GameControllerButton);
      SDL_GameControllerButton getTrackNext() const;

      void setTrackPrevious(SDL_GameControllerButton);
      SDL_GameControllerButton getTrackPrevious() const;

      //! zoom controls
      void setZoomIn(SDL_GameControllerButton);
      SDL_GameControllerButton getZoomIn() const;

      void setZoomOut(SDL_GameControllerButton);
      SDL_GameControllerButton getZoomOut() const;
};

#endif // JOYSTICKASSIGNMENTSDL_H
