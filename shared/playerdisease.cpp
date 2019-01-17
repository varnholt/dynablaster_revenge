
// header
#include "playerdisease.h"

// shared
#include "player.h"

// Qt
#include <QTimer>


// static
QSet<Constants::SkullType> PlayerDisease::sSupportedSkulls;
QList<Constants::SkullType> PlayerDisease::sCubeFaces;

//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
PlayerDisease::PlayerDisease(QObject *parent) :
   QObject(parent),
   mType(Constants::SkullAutofire),
   mDuration(SERVER_SKULL_DURATION),
   mPlayerId(-1)
{
}


//-----------------------------------------------------------------------------
/*!
   \param type disease type
*/
void PlayerDisease::setType(Constants::SkullType type)
{
   mType = type;
}


//-----------------------------------------------------------------------------
/*!
   \return disease type
*/
Constants::SkullType PlayerDisease::getType() const
{
   return mType;
}


//-----------------------------------------------------------------------------
/*!
   \param duration disease duration
*/
void PlayerDisease::setDuration(int duration)
{
   mDuration = duration;
}


//-----------------------------------------------------------------------------
/*!
   \return disease duration
*/
int PlayerDisease::getDuration() const
{
   return mDuration;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if disease is active
*/
bool PlayerDisease::isActive() const
{
   return mActiveTime.elapsed() < getDuration();
}


//-----------------------------------------------------------------------------
/*!
*/
void PlayerDisease::activate()
{
   mActiveTime.start();

   QTimer::singleShot(
      getDuration(),
      this,
      SLOT(abort())
   ); 
}


//-----------------------------------------------------------------------------
/*!
*/
void PlayerDisease::randomizeType()
{
   setType((Constants::SkullType)(qrand() % Constants::SkullReset));
}


//-----------------------------------------------------------------------------
/*!
   \param keysPressed keys to modify
*/
void PlayerDisease::applyAutofire(qint8 &keysPressed)
{
   keysPressed |= Constants::KeyBomb;
}


//-----------------------------------------------------------------------------
/*!
   \param keysPressed keys to modify
*/
void PlayerDisease::applyKeyboardInvert(qint8 &keysPressed)
{
   qint8 invertedKeys = 0;

   // pass bomb bit
   if (keysPressed & Constants::KeyBomb)
      invertedKeys |= Constants::KeyBomb;

   // invert others
   if (keysPressed & Constants::KeyUp)
      invertedKeys |= Constants::KeyDown;
   if (keysPressed & Constants::KeyDown)
      invertedKeys |= Constants::KeyUp;
   if (keysPressed & Constants::KeyLeft)
      invertedKeys |= Constants::KeyRight;
   if (keysPressed & Constants::KeyRight)
      invertedKeys |= Constants::KeyLeft;

   keysPressed = invertedKeys;
}


//-----------------------------------------------------------------------------
/*!
   \return id of infected player
*/
int PlayerDisease::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param playerId infected player
*/
void PlayerDisease::setPlayerId(int playerId)
{
   mPlayerId = playerId;
}


//-----------------------------------------------------------------------------
/*!
   \param skulls supported skulls
*/
void PlayerDisease::setSupportedSkulls(const QSet<Constants::SkullType>& skulls)
{
   sSupportedSkulls = skulls;
}


//-----------------------------------------------------------------------------
/*!
   \return supported skulls
*/
QSet<Constants::SkullType> PlayerDisease::getSupportedSkulls()
{
   return sSupportedSkulls;
}


//-----------------------------------------------------------------------------
/*!
   \param skull face setup
*/
void PlayerDisease::setSkullFaces(QList<Constants::SkullType> &faces)
{
   sCubeFaces = faces;
}


//-----------------------------------------------------------------------------
/*!
   \return skull faces setup
*/
QList<Constants::SkullType> PlayerDisease::getSkullFaces()
{
   return sCubeFaces;
}


//-----------------------------------------------------------------------------
/*!
   \return randomized skull faces
*/
QList<Constants::SkullType> PlayerDisease::generateSkullFaces()
{
   // TODO: really generate random faces in the future
   //       for now, the setup defined in the server settings is used.
   return sCubeFaces;
}


//-----------------------------------------------------------------------------
/*!
*/
void PlayerDisease::abort()
{
   emit stopped();
   deleteLater();
}


