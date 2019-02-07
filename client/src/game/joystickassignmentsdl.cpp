#include "joystickassignmentsdl.h"


JoystickAssignmentSDL::JoystickAssignmentSDL()
 : ControllerAssignment(ControllerTypeJoystick)
{
}


void JoystickAssignmentSDL::setUp(SDL_GameControllerButton button)
{
   mUp = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getUp() const
{
   return (SDL_GameControllerButton)mUp;
}


void JoystickAssignmentSDL::setDown(SDL_GameControllerButton button)
{
   mDown = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getDown() const
{
   return (SDL_GameControllerButton)mDown;
}


void JoystickAssignmentSDL::setLeft(SDL_GameControllerButton button)
{
   mLeft = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getLeft() const
{
   return (SDL_GameControllerButton)mLeft;
}


void JoystickAssignmentSDL::setRight(SDL_GameControllerButton button)
{
   mRight = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getRight() const
{
   return (SDL_GameControllerButton)mRight;
}


void JoystickAssignmentSDL::setBomb(SDL_GameControllerButton button)
{
   mBomb = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getBomb() const
{
   return (SDL_GameControllerButton)mBomb;
}


void JoystickAssignmentSDL::setEndGame(SDL_GameControllerButton button)
{
   mEndGame = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getEndGame() const
{
   return (SDL_GameControllerButton)mEndGame;
}


void JoystickAssignmentSDL::setTrackNext(SDL_GameControllerButton button)
{
   mTrackNext = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getTrackNext() const
{
   return (SDL_GameControllerButton)mTrackNext;
}


void JoystickAssignmentSDL::setTrackPrevious(SDL_GameControllerButton button)
{
   mTrackPrevious = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getTrackPrevious() const
{
   return (SDL_GameControllerButton)mTrackPrevious;
}


void JoystickAssignmentSDL::setZoomIn(SDL_GameControllerButton button)
{
   mZoomIn = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getZoomIn() const
{
   return (SDL_GameControllerButton)mZoomIn;
}


void JoystickAssignmentSDL::setZoomOut(SDL_GameControllerButton button)
{
   mZoomOut = button;
}


SDL_GameControllerButton JoystickAssignmentSDL::getZoomOut() const
{
   return (SDL_GameControllerButton)mZoomOut;
}
