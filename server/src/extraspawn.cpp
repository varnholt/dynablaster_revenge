// header
#include "extraspawn.h"

// shared
#include "map.h"

// server
#include "constants.h"


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
ExtraSpawn::ExtraSpawn(QObject *parent)
 : QObject(parent),
   mMap(0)
{
   mSpawnTimer.setInterval(SERVER_SPAWN_INTERVAL);
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if spawning has been enabled
*/
bool ExtraSpawn::isEnabled() const
{
   return mEnabled;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled enabled flag
*/
void ExtraSpawn::setEnabled(bool enabled)
{
   mEnabled = enabled;
}


//-----------------------------------------------------------------------------
/*!
   \param map game's map
*/
void ExtraSpawn::setMap(Map *map)
{
   mMap = map;
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraSpawn::activateSpawnTimer()
{
   mSpawnTimer.start();
}


//-----------------------------------------------------------------------------
/*!
   \brief this is the start condition, if this is function returned true once,
          spawn extras all the time
*/
bool ExtraSpawn::isExtraAvailable() const
{
   bool available = false;

   available = mMap->isHiddenExtraAvailable();

   return available;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if spawn timer is active
*/
bool ExtraSpawn::isSpawnTimerActive() const
{
   return mSpawnTimer.isActive();
}


//-----------------------------------------------------------------------------
/*!
   \return map
*/
Map *ExtraSpawn::getMap() const
{
   return mMap;
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraSpawn::reset()
{
   mSpawnTimer.stop();
}


//-----------------------------------------------------------------------------
/*!
*/
void ExtraSpawn::update()
{
   if (isEnabled())
   {
      if (!isSpawnTimerActive())
      {
         if (!isExtraAvailable())
         {
            activateSpawnTimer();
         }
      }
   }
}

