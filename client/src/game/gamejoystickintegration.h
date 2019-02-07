#ifndef GAMEJOYSTICKINTEGRATION_H
#define GAMEJOYSTICKINTEGRATION_H

// Qt
#include <QObject>

// forward declarations
class GameJoystickMapping;
class JoystickHandler;
class JoystickInterface;

class GameJoystickIntegration : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      GameJoystickIntegration(QObject *parent = 0);

      //! initialize all instances
      static void initializeAll();

      //! static instance getter
      static GameJoystickIntegration* getInstance(int id);

      //! create instance
      static GameJoystickIntegration* createInstance();

      //! destructor
      virtual ~GameJoystickIntegration();

      //! initialize joystick integration
      void initialize(int id = 0);

      //! select a joystick
      void setActiveJoystick(int id);

      //! getter for joystick mapping
      GameJoystickMapping* getJoystickMapping();

      //! getter for joystick integration
      JoystickInterface* getJoystickInterface();


   public slots:

      //! let joystick rumble
      void rumble(float intensity, int ms);


   signals:

      void keyTyped(int);


   private:

      //! joystick integration instances
      static GameJoystickIntegration* sInstances[10];


      // members

      //! joystick interface
      JoystickInterface* mJoystickInterface;

      //! joystick handler
      JoystickHandler* mJoystickHandler;

      //! joystick mapping
      GameJoystickMapping* mJoystickMapping;

};

#endif // GAMEJOYSTICKINTEGRATION_H
