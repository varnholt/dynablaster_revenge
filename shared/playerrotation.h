//
// C++ Interface: playerrotation
//
// Description: 
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLAYERROTATION_H
#define PLAYERROTATION_H

// Qt
#include <QVector2D>

/**
	@author Matthias Varnholt <matto@gmx.de>
*/
class PlayerRotation
{

   public:

      //! constructor
      PlayerRotation();

      //! setter for angle increment 0..360
      static void setAngleIncrement(float angle);

      //! setter for target direction (position on the unit circle)
      void setTargetVector(const QVector2D&);

      //! getter for the target vector
      const QVector2D& getTargetVector();

      //! update angle
      void updateAngle();

      //! getter for the current angle
      float getAngle();

      //! getter for the previous angle
      float getPreviousAngle();

      //! getter for angle delta
      float getAngleDelta() const;

      //! reset angle
      void reset();


   private:

      //! angle to increment by each update
      static float mAngleIncrement;

      //! current angle
      float mAngle;

      //! previous angle
      float mPreviousAngle;

      //! target angle
      float mTargetAngle;

      //! delta from previous angle to current angle
      float mDelta;

      //! target vector
      QVector2D mTargetVector;

      //! down vector
      static QVector2D mDown;
};

#endif
