#include "keyboardassignment.h"


KeyboardAssignment::KeyboardAssignment()
 : ControllerAssignment(ControllerTypeKeyboard)
{
}


void KeyboardAssignment::setUp(Qt::Key button)
{
   mUp = button;
}


Qt::Key KeyboardAssignment::getUp() const
{
   return (Qt::Key)mUp;
}


void KeyboardAssignment::setDown(Qt::Key button)
{
   mDown = button;
}


Qt::Key KeyboardAssignment::getDown() const
{
   return (Qt::Key)mDown;
}


void KeyboardAssignment::setLeft(Qt::Key button)
{
   mLeft = button;
}


Qt::Key KeyboardAssignment::getLeft() const
{
   return (Qt::Key)mLeft;
}


void KeyboardAssignment::setRight(Qt::Key button)
{
   mRight = button;
}


Qt::Key KeyboardAssignment::getRight() const
{
   return (Qt::Key)mRight;
}


void KeyboardAssignment::setBomb(Qt::Key button)
{
   mBomb = button;
}


Qt::Key KeyboardAssignment::getBomb() const
{
   return (Qt::Key)mBomb;
}


void KeyboardAssignment::setEndGame(Qt::Key button)
{
   mEndGame = button;
}


Qt::Key KeyboardAssignment::getEndGame() const
{
   return (Qt::Key)mEndGame;
}


void KeyboardAssignment::setTrackNext(Qt::Key button)
{
   mTrackNext = button;
}


Qt::Key KeyboardAssignment::getTrackNext() const
{
   return (Qt::Key)mTrackNext;
}


void KeyboardAssignment::setTrackPrevious(Qt::Key button)
{
   mTrackPrevious = button;
}


Qt::Key KeyboardAssignment::getTrackPrevious() const
{
   return (Qt::Key)mTrackPrevious;
}


void KeyboardAssignment::setZoomIn(Qt::Key button)
{
   mZoomIn = button;
}


Qt::Key KeyboardAssignment::getZoomIn() const
{
   return (Qt::Key)mZoomIn;
}


void KeyboardAssignment::setZoomOut(Qt::Key button)
{
   mZoomOut = button;
}


Qt::Key KeyboardAssignment::getZoomOut() const
{
   return (Qt::Key)mZoomOut;
}

