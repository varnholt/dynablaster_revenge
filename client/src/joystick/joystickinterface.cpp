#include "joystickinterface.h"


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
JoystickInterface::JoystickInterface(QObject* parent)
   : QObject(parent),
     mButtonMapping(0)
{
}


//-----------------------------------------------------------------------------
/*!
*/
JoystickInterface::~JoystickInterface()
{
}


//-----------------------------------------------------------------------------
/*!
   \return button mapping
*/
JoystickConstants::ControllerButton* JoystickInterface::getButtonMapping()
{
   return mButtonMapping;
}


//-----------------------------------------------------------------------------
/*!
   \param mapping button mapping
*/
void JoystickInterface::setButtonMapping(
   JoystickConstants::ControllerButton *mapping
)
{
   mButtonMapping = mapping;
}


//-----------------------------------------------------------------------------
/*!
   \param intensity rumble intensity
   \parma ms rumble time
*/
void JoystickInterface::rumble(float intensity, int ms)
{
   Q_UNUSED(intensity);
   Q_UNUSED(ms);
}
