#ifndef BOTPLAYERINFO_H
#define BOTPLAYERINFO_H

// base
#include "playerinfo.h"

class BotPlayerInfo : public PlayerInfo
{
   public:

      //! constructor
      BotPlayerInfo();

      //! reset player data
      void reset();

      //! player got a bomb extra
      void addBomb();

      //! player got a flame extra
      void addFlame();

      //! player got a speed extra
      void addSpeed();

      //! getter for bomb count
      int getBombCount() const;

      //! getter for flame count
      int getFlameCount() const;

      //! getter for speed count
      int getSpeedCount() const;

      //! setter for kick flag
      void setKickEnabled(bool);

      //! getter for kick flag
      bool isKickEnabled() const;


   protected:

      //! number of bombs
      int mBombs;

      //! number of flames
      int mFlames;

      //! number of speedups
      int mSpeedUps;

      //! player can kick
      bool mKickEnabled;
};

#endif // BOTPLAYERINFO_H
