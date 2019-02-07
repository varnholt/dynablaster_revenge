#ifndef JOYSTICKINFO_H
#define JOYSTICKINFO_H

// Qt
#include <QList>

// joystick
#include "joystickballvector.h"
#include "joystickconstants.h"


class JoystickInfo
{
   public:

      //! constructor a
      JoystickInfo();

      //! constructor b
      JoystickInfo(
         const QList<int>& axisValues,
         const QList<bool>& buttonValues,
         const QList<JoystickBallVector>& ballValues,
         const QList<JoystickConstants::Hat>& hatValues
      );

      //! clear all lists
      void clear();

      //! add axis value
      void addAxisValue(int);

      //! add button state
      void addButtonState(bool);

      //! add ball value
      void addBallValue(const JoystickBallVector&);

      //! add hat value
      void addHatValue(JoystickConstants::Hat);

      //! getter for axis values
      const QList<int>& getAxisValues() const;

      //! getter for button values
      const QList<bool>& getButtonValues() const;

      //! getter for ball values
      const QList<JoystickBallVector>& getBallValues() const;

      //! getter for hat values
      const QList<JoystickConstants::Hat>& getHatValues() const;


   protected:

      //! axis values
      QList<int> mAxisValues;

      //! button values
      QList<bool> mButtonValues;

      //! ball values
      QList<JoystickBallVector> mBallValues;

      //! hat values
      QList<JoystickConstants::Hat> mHatValues;
};

#endif // JOYSTICKINFO_H
