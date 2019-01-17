// header
#include "botplayerinfo.h"

// shared
#include "constants.h"
#include "playerdisease.h"


//-----------------------------------------------------------------------------
/*!
*/
BotPlayerInfo::BotPlayerInfo()
   : mBombs(SERVER_DEFAULT_BOMBCOUNT),
     mFlames(SERVER_DEFAULT_FLAMECOUNT),
     mSpeedUps(SERVER_DEFAULT_SPEEDUPS),
     mKickEnabled(false)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void BotPlayerInfo::addBomb()
{
   mBombs++;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotPlayerInfo::addFlame()
{
   mFlames++;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotPlayerInfo::addSpeed()
{
   mSpeedUps++;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotPlayerInfo::reset()
{
   mBombs = SERVER_DEFAULT_BOMBCOUNT;
   mFlames = SERVER_DEFAULT_FLAMECOUNT;
   mSpeedUps = SERVER_DEFAULT_SPEEDUPS;
   mKickEnabled = false;
}


//-----------------------------------------------------------------------------
/*!
   \return number of bombs
*/
int BotPlayerInfo::getBombCount() const
{
   int bombCount = mBombs;

   if (getDisease())
   {
      Constants::SkullType skullType = getDisease()->getType();

      if (skullType == Constants::SkullNoBomb)
         bombCount = 0;
      else if (skullType == Constants::SkullMinimumBomb)
         bombCount = 1;
      else if (skullType == Constants::SkullMaximumBomb)
         bombCount = 10;
   }

   return bombCount;
}


//-----------------------------------------------------------------------------
/*!
   \return flame count
*/
int BotPlayerInfo::getFlameCount() const
{
   int flameCount = mFlames;

   if (getDisease())
   {
      Constants::SkullType skullType = getDisease()->getType();

      if (skullType == Constants::SkullMinimumBomb)
         flameCount = 1;
      else if (skullType == Constants::SkullMaximumBomb)
         flameCount = 10;
   }

   return flameCount;
}


//-----------------------------------------------------------------------------
/*!
   \return speed count
*/
int BotPlayerInfo::getSpeedCount() const
{
   return mSpeedUps;
}


//-----------------------------------------------------------------------------
/*!
   \param kick kick enabled flag
*/
void BotPlayerInfo::setKickEnabled(bool kick)
{
   mKickEnabled = kick;
}


//-----------------------------------------------------------------------------
/*!
   \return kick enabled flag
*/
bool BotPlayerInfo::isKickEnabled() const
{
   return mKickEnabled;
}



