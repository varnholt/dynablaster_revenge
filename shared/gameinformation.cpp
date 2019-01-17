// header
#include "gameinformation.h"


//-----------------------------------------------------------------------------
/*!
*/
GameInformation::GameInformation()
   : mId(-1),
     mPlayerCount(0),
     mMaximumPlayerCount(0),
     mCreatorId(-1),
     mDimensions(Constants::DimensionInvalid),
     mExtras(0),
     mDuration(0),
     mGamesPlayed(0),
     mCurrentRound(0),
     mRoundCount(0),
     mSpawnExtras(false)
{
}


//-----------------------------------------------------------------------------
/*!
   \param id game id
   \param playerCount players in this game
   \param playerMaximum count maximum players for this game
   \param gameName this game's name
   \param levelName the level for this game
   \param creatorId id of the game creator
   \param dimensions playfield dimensions
   \param extras extra enums combined
   \param duration round duration
   \param roundsPlayer rounds played
   \param currentRound current round
   \param roundCount number of rounds to be played
   \param spawnExtras spawn extras
*/
GameInformation::GameInformation(
   int id,
   int playerCount,
   int playerMaximumCount,
   const QString& gameName,
   const QString& levelName,
   int creatorId,
   Constants::Dimension dimensions,
   int extras,
   int duration,
   int roundsPlayed,
   int currentRound,
   int roundCount,
   bool spawnExtras
)
   : mId(id),
     mPlayerCount(playerCount),
     mMaximumPlayerCount(playerMaximumCount),
     mGameName(gameName),
     mLevelName(levelName),
     mCreatorId(creatorId),
     mDimensions(dimensions),
     mExtras(extras),
     mDuration(duration),
     mGamesPlayed(roundsPlayed),
     mCurrentRound(currentRound),
     mRoundCount(roundCount),
     mSpawnExtras(spawnExtras)
{
}


//-----------------------------------------------------------------------------
/*!
   \return game id
*/
int GameInformation::getId() const
{
   return mId;
}


//-----------------------------------------------------------------------------
/*!
   \return player count
*/
int GameInformation::getPlayerCount() const
{
   return mPlayerCount;
}


//-----------------------------------------------------------------------------
/*!
   \return player maximum count
*/
int GameInformation::getPlayerMaximumCount() const
{
   return mMaximumPlayerCount;
}


//-----------------------------------------------------------------------------
/*!
   \return game name
*/
const QString GameInformation::getGameName() const
{
   return mGameName;
}


//-----------------------------------------------------------------------------
/*!
   \return level name
*/
const QString GameInformation::getLevelName() const
{
   return mLevelName;
}


//-----------------------------------------------------------------------------
/*!
   \return game creator id
*/
int GameInformation::getCreatorId() const
{
   return mCreatorId;
}


//-----------------------------------------------------------------------------
/*!
   \return map dimension enum
*/
Constants::Dimension GameInformation::getMapDimensions() const
{
   return mDimensions;
}


//-----------------------------------------------------------------------------
/*!
   \param extras extras for this game
*/
void GameInformation::setExtras(int extras)
{
   mExtras = extras;
}


//-----------------------------------------------------------------------------
/*!
   \return extras for this game
*/
int GameInformation::getExtras() const
{
   return mExtras;
}


//-----------------------------------------------------------------------------
/*!
   \param duration game duration
*/
void GameInformation::setDuration(int duration)
{
   mDuration = duration;
}


//-----------------------------------------------------------------------------
/*!
   \return game duration
*/
int GameInformation::getDuration() const
{
   return mDuration;
}


//-----------------------------------------------------------------------------
/*!
   \param roundsPlayed number of gamesPlayed
*/
void GameInformation::setGamesPlayed(int roundsPlayed)
{
   mGamesPlayed = roundsPlayed;
}


//-----------------------------------------------------------------------------
/*!
   \return games played
*/
int GameInformation::getGamesPlayed() const
{
   return mGamesPlayed;
}


//-----------------------------------------------------------------------------
/*!
   \return map x scale
*/
float GameInformation::getMapScaleX() const
{
   float scale = 1.0f;

   switch (getMapDimensions())
   {
      case Constants::Dimension13x11:
         scale = 1.0f;
         break;

      case Constants::Dimension19x17:
         scale = 19.0f/13.0f;
         break;

      case Constants::Dimension25x21:
         scale = 25.0f/13.0f;
         break;

      default:
         break;
   }

   return scale;
}


//-----------------------------------------------------------------------------
/*!
   \return map y scale
*/
float GameInformation::getMapScaleY() const
{
   float scale = 1.0f;

   switch (getMapDimensions())
   {
      case Constants::Dimension13x11:
         scale = 1.0f;
         break;

      case Constants::Dimension19x17:
         scale = 17.0f/11.0f;
         break;

      case Constants::Dimension25x21:
         scale = 21.0f/11.0f;
         break;

      default:
         break;
   }

   return scale;
}


//-----------------------------------------------------------------------------
/*!
   \return current round
*/
int GameInformation::getCurrentRound() const
{
   return mCurrentRound;
}


//-----------------------------------------------------------------------------
/*!
   \return round count
*/
int GameInformation::getRoundCount() const
{
   return mRoundCount;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if extra spawning is enabled
*/
bool GameInformation::isSpawnExtrasEnabled() const
{
   return mSpawnExtras;
}


//-----------------------------------------------------------------------------
/*!
   \param value extra spawn enabled
*/
void GameInformation::setSpawnExtrasEnabled(bool value)
{
   mSpawnExtras = value;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream out
   \param info gameinfo object to stream
*/
QDataStream& operator<<(QDataStream& out, const GameInformation& info)
{
   out << info.mId;
   out << info.mPlayerCount;
   out << info.mMaximumPlayerCount;
   out << info.mGameName;
   out << info.mLevelName;
   out << info.mCreatorId;
   out << (int)info.mDimensions;
   out << info.mExtras;
   out << info.mDuration;
   out << info.mGamesPlayed;
   out << info.mCurrentRound;
   out << info.mRoundCount;

   return out;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream in
   \param info gameinfo object to stream
*/
QDataStream& operator>>(QDataStream& in, GameInformation& info)
{
   int dimensions = -1;

   in >> info.mId;
   in >> info.mPlayerCount;
   in >> info.mMaximumPlayerCount;
   in >> info.mGameName;
   in >> info.mLevelName;
   in >> info.mCreatorId;
   in >> dimensions;
   in >> info.mExtras;
   in >> info.mDuration;
   in >> info.mGamesPlayed;
   in >> info.mCurrentRound;
   in >> info.mRoundCount;

   info.mDimensions = (Constants::Dimension)dimensions;

   return in;
}

