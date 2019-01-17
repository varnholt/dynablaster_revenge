// header
#include "playerinfo.h"

// shared
#include "playerdisease.h"


PlayerInfo::PlayerInfo()
   : mId(-1),
     mColor(Constants::ColorWhite),
     mX(0.0f),
     mY(0.0f),
     mAngle(0.0f),
     mDeltaX(0.0f),
     mDeltaY(0.0f),
     mDeltaAngle(0.0f),
     mKilled(false),
     mDisease(0),
     mDirections(0)
{
}


void PlayerInfo::setId(int id)
{
   mId = id;
}


void PlayerInfo::setColor(Constants::Color color)
{
   mColor = color;
}


void PlayerInfo::setNick(const QString& nick)
{
   mNick = nick;
}


void PlayerInfo::setPosition(float x, float y, float angle)
{
   mX = x;
   mY = y;
   mAngle = angle;
}

void PlayerInfo::setPositionDelta(float dx, float dy, float dAngle)
{
   setDeltaX(dx);
   setDeltaY(dy);
   setDeltaAngle(dAngle);
}


float PlayerInfo::getX() const
{
   return mX;
}


float PlayerInfo::getY() const
{
   return mY;
}


float PlayerInfo::getAngle() const
{
   return mAngle;
}


void PlayerInfo::setDeltaX(float val)
{
   mDeltaX = val;
}


void PlayerInfo::setDeltaY(float val)
{
   mDeltaY = val;
}


void PlayerInfo::setDeltaAngle(float val)
{
   mDeltaAngle = val;
}


int PlayerInfo::getId() const
{
   return mId;
}


Constants::Color PlayerInfo::getColor() const
{
   return mColor;
}


QString PlayerInfo::getNick() const
{
   return mNick;
}


//-----------------------------------------------------------------------------
/*!
   \return player delta x
*/
float PlayerInfo::getDeltaX() const
{
   return mDeltaX;
}


//-----------------------------------------------------------------------------
/*!
   \return player delta y
*/
float PlayerInfo::getDeltaY() const
{
   return mDeltaY;
}


//-----------------------------------------------------------------------------
/*!
   \return player angle delta
*/
float PlayerInfo::getAngleDelta() const
{
   return mDeltaAngle;
}


//-----------------------------------------------------------------------------
/*!
   \param stats overall stats
*/
void PlayerInfo::setOverallStats(const PlayerStats & stats)
{
   mOverallStats = stats;
}


//-----------------------------------------------------------------------------
/*!
   \return overall stats
*/
PlayerStats &PlayerInfo::getOverallStats()
{
   return mOverallStats;
}


//-----------------------------------------------------------------------------
/*!
   \param stats round stats
*/
void PlayerInfo::setRoundStats(const PlayerStats & stats)
{
   mRoundStats = stats;
}


//-----------------------------------------------------------------------------
/*!
   \return round stats
*/
PlayerStats &PlayerInfo::getRoundStats()
{
   return mRoundStats;
}


//----------------------------------------------------------------------------
/*!
   \param killed killed flag
*/
void PlayerInfo::setKilled(bool killed)
{
   mKilled = killed;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if player is killed
*/
bool PlayerInfo::isKilled() const
{
   return mKilled;
}


//----------------------------------------------------------------------------
/*!
   \param disease disease to infect player with
*/
void PlayerInfo::infect(QPointer<PlayerDisease> disease)
{
   if (mDisease)
      mDisease.data()->deleteLater();

   mDisease = disease;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if player is infected
*/
bool PlayerInfo::isInfected() const
{
   return (mDisease != 0);
}


//----------------------------------------------------------------------------
/*!
   \return player disease
*/
PlayerDisease *PlayerInfo::getDisease() const
{
   return mDisease;
}


//-----------------------------------------------------------------------------
/*!
   \return player directions
*/
qint8 PlayerInfo::getDirections() const
{
   return mDirections;
}


//-----------------------------------------------------------------------------
/*!
   \param directions player directions
*/
void PlayerInfo::setDirections(qint8 directions)
{
   mDirections = directions;
}
