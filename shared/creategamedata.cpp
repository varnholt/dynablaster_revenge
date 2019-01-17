#include "creategamedata.h"


CreateGameData::CreateGameData()
 : mRounds(0),
   mDuration(0),
   mMaxPlayers(0),
   mExtraBombEnabled(false),
   mExtraFlameEnabled(false),
   mExtraSpeedupEnabled(false),
   mExtraKickEnabled(false),
   mExtraSkullsEnabled(false),
   mDimension(Constants::Dimension13x11)
{
}


CreateGameData::~CreateGameData()
{
}
