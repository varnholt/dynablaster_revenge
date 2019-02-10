#ifndef PLAYER_H
#define PLAYER_H

// Qt
#include <QPointer>
#include <QString>

// shared
#include "constants.h"
#include "playerrotation.h"

class PlayerDisease;
class PlayerStats;


class Player{

public:

   //! constructor
   Player(int id);

   //! destructor
   ~Player();

   //! reset player
   void reset();

   //! setter for logged-in flag
   void setLoggedIn(bool);

   //! getter for logged-in flag
   bool isLoggedIn() const;

   //! setter for keys pressed
   void setKeysPressed(int);

   //! getter for keys pressed
   int getKeysPressed();

   //! getter for keys previously pressed
   int getKeysPressedPreviously();

   //! setter for x position
   void setX(float);

   //! setter for y position
   void setY(float);

   //! getter for x position
   float getX();

   //! getter for y position
   float getY();

   //! setter for speed
   void setSpeed(float speed);

   //! getter for speed
   float getSpeed();

   //! getter for id
   qint8 getId();

   //! getter for the player's rotation
   PlayerRotation* getPlayerRotation();

   //! setter for the player's nick
   void setNick(const QString& nick);

   //! getter for the player's nick
   const QString& getNick();

   //! setter for bombs
   void setBombCount(qint8 count);

   //! getter for bombs
   qint8 getBombCount();

   //! setter for flames
   void setFlameCount(qint8 count);

   //! getter for flames
   qint8 getFlameCount();

   //! setter for bombs dropped
   void setBombsDroppedCount(qint8 count);

   //! getter for bombs dropped
   qint8 getBombsDroppedCount();

   //! setter for killed flag
   void setKilled(bool);

   //! getter for killed flag
   bool isKilled();

   //! setter for kick flag
   void setKickEnabled(bool);

   //! getter for kick flag
   bool isKickEnabled();

   //! getter for bomb count default
   static qint8 getBombCountDefault();

   //! getter for flame count default
   static qint8 getFlameCountDefault();

   //! getter for position skip counter
   int getPositionSkipCounter() const;

   //! setter for position skip counter
   void setPositionSkipCounter(int count);

   //! getter for overall stats
   PlayerStats* getOverallStats();

   //! getter for round stats
   PlayerStats* getRoundStats();

   //! setter for player color
   void setColor(Constants::Color color);

   //! getter for player color
   Constants::Color getColor() const;

   //! setter for synchronized loading flag
   void setLoadingSynchronized(bool);

   //! getter for synchronized loading flag
   bool isLoadingSynchronized() const;

   //! setter for bot flag
   void setBot(bool bot);

   //! getter for bot flag
   bool isBot() const;

   //! infect player
   void infect(QPointer<PlayerDisease> disease);

   //! check if player is infected
   bool isInfected() const;

   //! check if player is invincible
   bool isInvincible() const;

   //! getter for disease
   PlayerDisease* getDisease() const;

   //! increase number of kills
   void increaseKills();

   //! increase number of deaths
   void increaseDeaths();

   //! increase number of wins
   void increaseWins();

   //! increase survival time
   void increaseSurvivalTime(unsigned int survivalTime);

   //! increase extra count
   void increaseExtrasCollected();

   //! reset stats
   void resetStats();

   //! increase flame count
   void increaseFlameCount();

   //! increase bomb count
   void increaseBombCount();

   //! getter for bomb processed flag
   bool isBombKeyLocked() const;

   //! setter for bomb processed flag
   void setBombKeyLocked(bool processed);


private:

   //! id
   qint8 mId;

   //! player logged in
   bool mLoggedIn;

   //! x position
   float mX;

   //! y position
   float mY;

   //! nick
   QString mNick;

   //! number of bombs
   int mBombCount;

   //! number of flames
   int mFlameCount;

   //! speed
   float mSpeed;

   //! keys currently pressed
   int mKeysPressed;

   //! keys previously pressed
   int mKeysPreviouslyPressed;

   //! bombs dropped count
   int mBombsDroppedCount;

   //! bomb processed flag
   bool mBombKeyLocked;

   //! player rotation
   PlayerRotation mPlayerRotation;

   //! player is killed
   bool mKilled;

   //! player is able to kick
   bool mKickEnabled;

   //! Number of frames when last position packet was sent
   int mPositionSkippedCounter;

   //! player overall stats
   PlayerStats* mOverallStats;

   //! player round stats
   PlayerStats* mRoundStats;

   //! player color
   Constants::Color mColor;

   //! loading synchronized
   bool mLoadingSynchronized;

   //! player is a bot or not
   bool mBot;

   //! player can be infected
   QPointer<PlayerDisease> mDisease;
};

#endif
