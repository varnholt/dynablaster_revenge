#include "joystickinfo.h"

//-----------------------------------------------------------------------------
/*!
*/
JoystickInfo::JoystickInfo()
{
}


//-----------------------------------------------------------------------------
/*!
   \param axisValues axis values
   \param buttonValues button values
   \param ballValues ball values
   \param hatValues hat values
*/
JoystickInfo::JoystickInfo(
   const QList<int>& axisValues,
   const QList<bool>& buttonValues,
   const QList<JoystickBallVector>& ballValues,
   const QList<JoystickConstants::Hat>& hatValues
)
   : mAxisValues(axisValues),
     mButtonValues(buttonValues),
     mBallValues(ballValues),
     mHatValues(hatValues)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void JoystickInfo::clear()
{
   mAxisValues.clear();
   mButtonValues.clear();
   mBallValues.clear();
   mHatValues.clear();
}


//-----------------------------------------------------------------------------
/*!
   \param val value to append
*/
void JoystickInfo::addAxisValue(int val)
{
   mAxisValues.append(val);
}


//-----------------------------------------------------------------------------
/*!
   \param val value to append
*/
void JoystickInfo::addButtonState(bool val)
{
   mButtonValues.append(val);
}


//-----------------------------------------------------------------------------
/*!
   \param val value to append
*/
void JoystickInfo::addBallValue(const JoystickBallVector& val)
{
   mBallValues.append(val);
}

//-----------------------------------------------------------------------------
/*!
   \param val value to append
*/
void JoystickInfo::addHatValue(JoystickConstants::Hat val)
{
   mHatValues.append(val);
}


//-----------------------------------------------------------------------------
/*!
   \return axis values
*/
const QList<int>& JoystickInfo::getAxisValues() const
{
   return mAxisValues;
}


//-----------------------------------------------------------------------------
/*!
   \return button values
*/
const QList<bool>& JoystickInfo::getButtonValues() const
{
   return mButtonValues;
}


//-----------------------------------------------------------------------------
/*!
   \return ball values
*/
const QList<JoystickBallVector>& JoystickInfo::getBallValues() const
{
   return mBallValues;
}


//-----------------------------------------------------------------------------
/*!
   \return hat values
*/
const QList<JoystickConstants::Hat>& JoystickInfo::getHatValues() const
{
   return mHatValues;
}
