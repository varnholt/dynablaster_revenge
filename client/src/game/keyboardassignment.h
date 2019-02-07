#ifndef KEYBOARDASSIGNMENT_H
#define KEYBOARDASSIGNMENT_H

// base
#include "controllerassignment.h"

// Qt
#include <Qt>


class KeyboardAssignment : public ControllerAssignment
{
   public:

      KeyboardAssignment();

      //! main controls
      void setUp(Qt::Key);
      Qt::Key getUp() const;

      void setDown(Qt::Key);
      Qt::Key getDown() const;

      void setLeft(Qt::Key);
      Qt::Key getLeft() const;

      void setRight(Qt::Key);
      Qt::Key getRight() const;

      void setBomb(Qt::Key);
      Qt::Key getBomb() const;

      //! end game
      void setEndGame(Qt::Key);
      Qt::Key getEndGame() const;

      //! music controls
      void setTrackNext(Qt::Key);
      Qt::Key getTrackNext() const;

      void setTrackPrevious(Qt::Key);
      Qt::Key getTrackPrevious() const;

      //! zoom controls
      void setZoomIn(Qt::Key);
      Qt::Key getZoomIn() const;

      void setZoomOut(Qt::Key);
      Qt::Key getZoomOut() const;
};

#endif // KEYBOARDASSIGNMENT_H
