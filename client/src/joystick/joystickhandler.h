#ifndef JOYPICK_H
#define JOYPICK_H

#include <QWidget>
#include <QTimer>
#include <QList>

#include "joystickinfo.h"

class JoystickInterface;

class JoystickHandler : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      JoystickHandler(QObject *parent = 0);

      //! destructor
      ~JoystickHandler();

      //! initialize joysticks
      void initialize(int id = 0);

      //! setter for joystick interface
      void setInterface(JoystickInterface*);

      //! setter for update interval
      void setUpdateInterval(int interval);


   signals:

      //! joystick data
      void data(const JoystickInfo&);


   protected slots:

      //! update joystick data
      void updateData();


   protected:

      //! debug collected data
      void debug();

      //! joystick interface
      JoystickInterface* mJoystickInterface;

      //! joystick update timer
      QTimer* mUpdateTimer;

      //! current joystick info
      JoystickInfo mJoystickInfo;
};

#endif // JOYPICK_H
