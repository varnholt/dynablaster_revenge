#ifndef CREATEGAMEDATA_H
#define CREATEGAMEDATA_H

// Qt
#include <QString>

// shared
#include "constants.h"

class CreateGameData
{    
   public:

      //! constructor
      CreateGameData();

      //! destructor
      virtual ~CreateGameData();

      //! game's name
      QString mName;

      //! game level name
      QString mLevel;

      //! number of rounds
      int mRounds;

      //! game duration (s)
      int mDuration;

      //! maximum player count
      int mMaxPlayers;

      //! bomb extra enabled
      bool mExtraBombEnabled;

      //! flame extra enabled
      bool mExtraFlameEnabled;

      //! speedup extra enabled
      bool mExtraSpeedupEnabled;

      //! kick extra enabled
      bool mExtraKickEnabled;

      //! skulls enabled
      bool mExtraSkullsEnabled;

      //! playfield dimension
      Constants::Dimension mDimension;
};

#endif // CREATEGAMEDATA_H
