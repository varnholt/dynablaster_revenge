// header
#include "headlessintegration.h"

// headlessclient
#include "headlessclient.h"

// game
#include "gamejoystickintegration.h"
#include "gamejoystickmapping.h"

// joystick
#include "joystick/joystickinterface.h"

// static
QList<HeadlessIntegration*> HeadlessIntegration::sInstances;


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
HeadlessIntegration::HeadlessIntegration(QObject *parent)
 : QObject(parent)
{
   sInstances.push_back(this);
}


//-----------------------------------------------------------------------------
/*!
*/
HeadlessIntegration::~HeadlessIntegration()
{
   sInstances.removeOne(this);
}


//-----------------------------------------------------------------------------
/*!
   \return instance count
*/
int HeadlessIntegration::getInstanceCount()
{
   return sInstances.count();
}


//-----------------------------------------------------------------------------
/*!
*/
HeadlessClient *HeadlessIntegration::getClient() const
{
    return mClient;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessIntegration::setClient(HeadlessClient *value)
{
    mClient = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessIntegration::initialize()
{
   // autologin
   connect(
      getClient(),
      SIGNAL(loginResponse(bool)),
      this,
      SLOT(joinGame())
   );

   // prepare joystick
   int jid = getJoystickId();
   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(jid);

   connect(
      gji->getJoystickMapping(),
      SIGNAL(keyPressed(int)),
      getClient(),
      SLOT(processKeyPressed(int))
   );

   connect(
      gji->getJoystickMapping(),
      SIGNAL(keyReleased(int)),
      getClient(),
      SLOT(processKeyReleased(int))
   );

   connect(
      mClient,
      SIGNAL(rumble(float,int)),
      gji,
      SLOT(rumble(float,int))
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessIntegration::debugJoysticks()
{
   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);
   JoystickInterface* ji = gji->getJoystickInterface();

   int joystickCount = ji->getJoystickCount();

   for (int i = 0; i < joystickCount; i++)
   {
      ji->setActiveJoystick(0);

      int axisCount   = ji->getAxisCount(i);
      int buttonCount = ji->getButtonCount(i);
      int hatCount    = ji->getHatCount(i);

      printf(
         "id: %d, name: %s, axes: %d, buttons: %d, hats: %d\n",
         i,
         qPrintable(ji->getName(i)),
         axisCount,
         buttonCount,
         hatCount
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
int HeadlessIntegration::getJoystickId() const
{
   return mJoystickId;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessIntegration::setJoystickId(int value)
{
   mJoystickId = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessIntegration::joinGame()
{
   mClient->joinGame(getGameId());
}


//-----------------------------------------------------------------------------
/*!
   \return game id
*/
int HeadlessIntegration::getGameId() const
{
   return mGameId;
}


//-----------------------------------------------------------------------------
/*!
   \param value game id
*/
void HeadlessIntegration::setGameId(int value)
{
   mGameId = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if player id relates to a headless player
*/
bool HeadlessIntegration::isHeadlessPlayer(int id)
{
   bool yup = false;

   foreach (HeadlessIntegration* i, sInstances)
   {
      if (i->getClient()->getPlayerId() == id)
      {
         yup = true;
         break;
      }
   }

   return yup;
}




