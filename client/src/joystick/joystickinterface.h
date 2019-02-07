#ifndef JOYSTICKINTERFACE_H
#define JOYSTICKINTERFACE_H

// Qt
#include <QObject>

// joystick
#include "joystickconstants.h"
#include "joystickinfo.h"

class JoystickInterface : public QObject
{
   Q_OBJECT

   public:

       //! constructor
       JoystickInterface(QObject* parent = 0);

       //! destructor
       virtual ~JoystickInterface();

       //! get number of joysticks
       virtual int getJoystickCount() = 0;

       //! setter for active joystick
       virtual void setActiveJoystick(int id) = 0;

       //! getter for active joystick
       virtual int getActiveJoystick() = 0;

       //! getter for button mapping
       JoystickConstants::ControllerButton* getButtonMapping();

       //! setter for button mapping
       void setButtonMapping(JoystickConstants::ControllerButton* mapping);


       // information about the current joystick

       //! getter for the joystick's name
       virtual QString getName(int id) = 0;

       //! getter for the axis count
       virtual int getAxisCount(int id) = 0;

       //! getter for the button count
       virtual int getButtonCount(int id) = 0;

       //! getter for the ball count
       virtual int getBallCount(int id) = 0;

       //! getter for the hat count
       virtual int getHatCount(int id) = 0;

       //! update axis and button infos
       virtual void update(JoystickInfo& info) = 0;


   public slots:

       //! make joystic rumble, intensity from 0..1, time in millis
       virtual void rumble(float intensity, int ms);


   protected:


       //! button mapping
       JoystickConstants::ControllerButton* mButtonMapping;
};

#endif // JOYSTICKINTERFACE_H
