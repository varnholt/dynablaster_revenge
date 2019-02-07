// header
#include "gamejoystickintegration.h"

// game
#include "gamejoystickmapping.h"

// joystick
#ifdef Q_OS_MAC
#ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#undef MAC_OS_X_VERSION_MIN_REQUIRED
#endif
#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_9
#endif
#include "../joystick/joystickinterfacesdl.h"
#include "../joystick/joystickhandler.h"


GameJoystickIntegration* GameJoystickIntegration::sInstances[10];


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
GameJoystickIntegration::GameJoystickIntegration(QObject *parent)
 : QObject(parent),
   mJoystickInterface(0),
   mJoystickHandler(0),
   mJoystickMapping(0)
{
   mJoystickInterface = new JoystickInterfaceSDL(this);
   mJoystickHandler = new JoystickHandler(this);
   mJoystickHandler->setInterface(mJoystickInterface);
   mJoystickMapping = new GameJoystickMapping(this);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameJoystickIntegration::initializeAll()
{
   for (int i = 0; i < 10; i++)
      sInstances[i] = 0;

   // used for obtaining some information from sdl
   JoystickInterfaceSDL* tmp = new JoystickInterfaceSDL();

   int count = tmp->getJoystickCount();

   GameJoystickIntegration* gji = 0;

   for (int i = 0; i < count; i++)
   {
      gji = createInstance();
      gji->initialize(i);

      sInstances[i] = gji;
   }

   delete tmp;
}


//-----------------------------------------------------------------------------
/*!
*/
GameJoystickIntegration::~GameJoystickIntegration()
{
}


//-----------------------------------------------------------------------------
/*!
   \param id joystick identifier to initialize, default is 0
*/
void GameJoystickIntegration::initialize(int id)
{
   mJoystickHandler->setUpdateInterval(40);
   mJoystickHandler->initialize(id);

   // automatically select first in list
   if (mJoystickInterface->getJoystickCount() > id)
   {
      setActiveJoystick(id);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to joystick mapping
*/
GameJoystickMapping* GameJoystickIntegration::getJoystickMapping()
{
   return mJoystickMapping;
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to joystick interface
*/
JoystickInterface *GameJoystickIntegration::getJoystickInterface()
{
   return mJoystickInterface;
}


//-----------------------------------------------------------------------------
/*!
   \param intensity rumble intensity
   \param ms rumble time
*/
void GameJoystickIntegration::rumble(float intensity, int ms)
{
   mJoystickInterface->rumble(intensity, ms);
}


//-----------------------------------------------------------------------------
/*!
   \param id joystick id
*/
void GameJoystickIntegration::setActiveJoystick(int id)
{
   mJoystickInterface->setActiveJoystick(id);

   disconnect(
      mJoystickHandler,
      SIGNAL(data(JoystickInfo))
   );

   connect(
      mJoystickHandler,
      SIGNAL(data(JoystickInfo)),
      mJoystickMapping,
      SLOT(data(JoystickInfo))
   );
}


//-----------------------------------------------------------------------------
/*!
   \return joystick instance
*/
GameJoystickIntegration *GameJoystickIntegration::getInstance(int id)
{
   GameJoystickIntegration* gji = 0;

   if (id >= 0 && id < 10)
      gji = sInstances[id];

   return gji;
}


//-----------------------------------------------------------------------------
/*!
   \return new joystick instance
*/
GameJoystickIntegration *GameJoystickIntegration::createInstance()
{
   return new GameJoystickIntegration();
}

