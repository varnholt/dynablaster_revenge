#ifndef GAMEINFORMATION_H
#define GAMEINFORMATION_H

// Qt
#include <QDataStream>
#include <QString>

// shared
#include "constants.h"

class GameInformation
{
   public:

      //! constructor for reading
      GameInformation();

      //! constructor for writing
      GameInformation(
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
      );

      //! getter for game id
      int getId() const;

      //! getter for player count
      int getPlayerCount() const;

      //! getter for the player maximum count
      int getPlayerMaximumCount() const;

      //! getter for the game name
      const QString getGameName() const;

      //! getter for the level name
      const QString getLevelName() const;

      //! getter for the game creator id
      int getCreatorId() const;

      //! getter for the map dimension enum
      Constants::Dimension getMapDimensions() const;

      //! setter for the extra enum combination
      void setExtras(int extra);

      //! getter for the extra enum combination
      int getExtras() const;

      //! setter for the game duration
      void setDuration(int duration);

      //! getter for the game duration
      int getDuration() const;

      //! setter for the number of games played
      void setGamesPlayed(int gamesPlayed);

      //! getter for the number of games played
      int getGamesPlayed() const;

      //! getter for x map scale
      float getMapScaleX() const;

      //! getter for y map scale
      float getMapScaleY() const;

      //! getter for current round count
      int getCurrentRound() const;

      //! getter for round count
      int getRoundCount() const;

      //! getter for spawn extras flag
      bool isSpawnExtrasEnabled() const;

      //! setter for spawn extras flag
      void setSpawnExtrasEnabled(bool value);


   public:

      //! game id
      int mId;

      //! player count
      int mPlayerCount;

      //! maximum player count
      int mMaximumPlayerCount;

      //! game name
      QString mGameName;

      //! level name
      QString mLevelName;

      //! creator id
      int mCreatorId;

      //! game dimensions
      Constants::Dimension mDimensions;

      //! extras
      int mExtras;

      //! game duration
      int mDuration;

      //! number of games played
      int mGamesPlayed;

      //! current round
      int mCurrentRound;

      //! number of rounds
      int mRoundCount;

      //! spawn extra flag
      bool mSpawnExtras;
};

QDataStream& operator<<(QDataStream& out, const GameInformation& info);
QDataStream& operator>>(QDataStream& in, GameInformation& info);

#endif // GAMEINFORMATION_H
