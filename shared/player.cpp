// header
#include "player.h"

// shared
#include "playerdisease.h"
#include "playerstats.h"


//----------------------------------------------------------------------------
/*!
  constructor
*/
Player::Player(int id)
   : mId(id),
     mLoggedIn(false),
     mX(0.5),
     mY(0.5),
     mBombCount(SERVER_DEFAULT_BOMBCOUNT),
     mFlameCount(SERVER_DEFAULT_FLAMECOUNT),
     mSpeed(SERVER_DEFAULT_SPEED),
     mKeysPressed(0),
     mKeysPreviouslyPressed(0),
     mBombsDroppedCount(0),
     mBombKeyLocked(false),
     mKilled(false),
     mKickEnabled(false),
     mPositionSkippedCounter(0),
     mOverallStats(nullptr),
     mRoundStats(nullptr),
     mColor(Constants::ColorWhite),
     mLoadingSynchronized(false),
     mBot(false),
     mDisease(nullptr)
{
   mOverallStats = new PlayerStats();
   mRoundStats = new PlayerStats();
}


//----------------------------------------------------------------------------
/*!
   destuctor
*/
Player::~Player()
{
   delete mOverallStats;
   delete mRoundStats;
}


//----------------------------------------------------------------------------
/*!
*/
void Player::reset()
{
   mKilled = false;
   mBombsDroppedCount = 0;
   mBombCount = SERVER_DEFAULT_BOMBCOUNT;
   mFlameCount = SERVER_DEFAULT_FLAMECOUNT;
   mSpeed = SERVER_DEFAULT_SPEED;
   mKickEnabled= false;
   mPlayerRotation.reset();
   delete mDisease;

   // reset key flags
   mKeysPressed = 0;
   mKeysPreviouslyPressed = 0;
   mPositionSkippedCounter = 0;
   mBombKeyLocked = false;
}


//----------------------------------------------------------------------------
/*!
   \param loggedIn logged in flag
*/
void Player::setLoggedIn(bool loggedIn)
{
   mLoggedIn = loggedIn;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if logged in
*/
bool Player::isLoggedIn() const
{
   return mLoggedIn;
}


//----------------------------------------------------------------------------
/*!
   \return keys currently pressed
*/
int Player::getKeysPressed()
{
   return mKeysPressed;
}


//----------------------------------------------------------------------------
/*!
   \return keys previously pressed
*/
int Player::getKeysPressedPreviously()
{
   return mKeysPreviouslyPressed;
}

//----------------------------------------------------------------------------
/*!
   \param x x position
*/
void Player::setX(float x)
{
   mX = x;
}


//----------------------------------------------------------------------------
/*!
   \param y y position
*/
void Player::setY(float y)
{
   mY = y;
}


//----------------------------------------------------------------------------
/*!
   \return x position
*/
float Player::getX()
{
   return mX;
}


//----------------------------------------------------------------------------
/*!
   \return y position
*/
float Player::getY()
{
   return mY;
}


//----------------------------------------------------------------------------
/*!
   \param speed new player speed
*/
void Player::setSpeed(float speed)
{
   mSpeed = speed;
}


//----------------------------------------------------------------------------
/*!
   \return speed
*/
float Player::getSpeed()
{
   float speed = mSpeed;

   if (isInfected())
   {
      if (getDisease()->getType() == Constants::SkullSlow)
         speed = SERVER_SKULL_SPEED_MIN;
      else if (getDisease()->getType() == Constants::SkullFast)
         speed = SERVER_SKULL_SPEED_MAX;
   }

   return speed;
}


//----------------------------------------------------------------------------
/*!
   \return id
*/
qint8 Player::getId()
{
   return mId;
}


//----------------------------------------------------------------------------
/*!
   \param keys keys pressed
*/
void Player::setKeysPressed(int keys)
{
   mKeysPreviouslyPressed = mKeysPressed;
   mKeysPressed = keys;
}


//----------------------------------------------------------------------------
/*!
   \return the player's rotation
*/
PlayerRotation* Player::getPlayerRotation()
{
   return &mPlayerRotation;
}



//----------------------------------------------------------------------------
/*!
   \param nick player's nick
*/
void Player::setNick(const QString& nick)
{
   mNick = nick;
}


//----------------------------------------------------------------------------
/*!
   \return the player's rotation
*/
const QString& Player::getNick()
{
   return mNick;
}


//----------------------------------------------------------------------------
/*!
   \param count bomb count
*/
void Player::setBombCount(qint8 count)
{
   mBombCount = count;
}


//----------------------------------------------------------------------------
/*!
   \return bombs count
*/
qint8 Player::getBombCount()
{
   qint8 bombCount = mBombCount;

   if (isInfected())
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


//----------------------------------------------------------------------------
/*!
   \param count flame count
*/
void Player::setFlameCount(qint8 count)
{
   mFlameCount = count;
}


//----------------------------------------------------------------------------
/*!
   \return flame count
*/
qint8 Player::getFlameCount()
{
   qint8 flameCount = mFlameCount;

   if (isInfected())
   {
      Constants::SkullType skullType = getDisease()->getType();

      if (skullType == Constants::SkullMinimumBomb)
         flameCount = 1;
      else if (skullType == Constants::SkullMaximumBomb)
         flameCount = 10;
   }

   return flameCount;
}


//----------------------------------------------------------------------------
/*!
   \param count bombs dropped
*/
void Player::setBombsDroppedCount(qint8 count)
{
   mBombsDroppedCount = count;
}


//----------------------------------------------------------------------------
/*!
   \return bombs dropped count
*/
qint8 Player::getBombsDroppedCount()
{
   return mBombsDroppedCount;
}


//----------------------------------------------------------------------------
/*!
   \param killed player is killed
*/
void Player::setKilled(bool killed)
{
   mKilled = killed;
}


//----------------------------------------------------------------------------
/*!
   \return true if player is killed
*/
bool Player::isKilled()
{
   return mKilled;
}


//----------------------------------------------------------------------------
/*!
   \return bomb count default
*/
qint8 Player::getBombCountDefault()
{
   return SERVER_DEFAULT_BOMBCOUNT;
}


//----------------------------------------------------------------------------
/*!
   \return flame count default
*/
qint8 Player::getFlameCountDefault()
{
   return SERVER_DEFAULT_FLAMECOUNT;
}


//----------------------------------------------------------------------------
/*!
   \param enabled kick is enabled
*/
void Player::setKickEnabled(bool enabled)
{
   mKickEnabled = enabled;
}


//----------------------------------------------------------------------------
/*!
   \return true if player is able to kick
*/
bool Player::isKickEnabled()
{
   return mKickEnabled;
}


//----------------------------------------------------------------------------
/*!
   \return number of skipped position packets
*/
int Player::getPositionSkipCounter() const
{
   return mPositionSkippedCounter;
}


//----------------------------------------------------------------------------
/*!
   \param count set number of skipped position packets
*/
void Player::setPositionSkipCounter(int count)
{
   mPositionSkippedCounter= count;
}


//----------------------------------------------------------------------------
/*!
   \return player overall stats
*/
PlayerStats* Player::getOverallStats()
{
   return mOverallStats;
}


//----------------------------------------------------------------------------
/*!
   \return player round stats
*/
PlayerStats *Player::getRoundStats()
{
   return mRoundStats;
}


//----------------------------------------------------------------------------
/*!
   \param color player color
*/
void Player::setColor(Constants::Color color)
{
   mColor = color;
}


//----------------------------------------------------------------------------
/*!
   \return player color
*/
Constants::Color Player::getColor() const
{
   return mColor;
}


//----------------------------------------------------------------------------
/*!
   \param synchronized loading has been synchronized
*/
void Player::setLoadingSynchronized(bool synchronized)
{
   mLoadingSynchronized = synchronized;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if loading has been synchronized
*/
bool Player::isLoadingSynchronized() const
{
   return mLoadingSynchronized;
}


//----------------------------------------------------------------------------
/*!
   \pram bot bot flag
*/
void Player::setBot(bool bot)
{
   mBot = bot;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if player is a bot
*/
bool Player::isBot() const
{
   return mBot;
}

//----------------------------------------------------------------------------
/*!
   \param disease disease to infect player with
*/
void Player::infect(QPointer<PlayerDisease> disease)
{
   if (mDisease)
   {
      mDisease.data()->abort();
   }

   mDisease = disease;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if player is infected
*/
bool Player::isInfected() const
{
   return (mDisease != nullptr);
}


//----------------------------------------------------------------------------
/*!
   \return \c true if player is invincible
*/
bool Player::isInvincible() const
{
   bool invincible = false;

   PlayerDisease* disease = getDisease();

   if (disease)
   {
      if (disease->getType() == Constants::SkullInvincible)
      {
         invincible = true;
      }
   }

   return invincible;
}


//----------------------------------------------------------------------------
/*!
   \return player disease
*/
PlayerDisease *Player::getDisease() const
{
   return mDisease;
}


//----------------------------------------------------------------------------
/*!
*/
void Player::increaseKills()
{
   getOverallStats()->increaseKills();
   getRoundStats()->increaseKills();
}


//----------------------------------------------------------------------------
/*!
*/
void Player::increaseDeaths()
{
   getOverallStats()->increaseDeaths();
   getRoundStats()->increaseDeaths();
}


//----------------------------------------------------------------------------
/*!
*/
void Player::increaseWins()
{
   getOverallStats()->increaseWins();
   getRoundStats()->increaseWins();
}


//----------------------------------------------------------------------------
/*!
   \param survivalTime player survival time
*/
void Player::increaseSurvivalTime(unsigned int survivalTime)
{
   getOverallStats()->increaseSurvivalTime(survivalTime);
   getRoundStats()->increaseSurvivalTime(survivalTime);
}


//----------------------------------------------------------------------------
/*!
*/
void Player::increaseExtrasCollected()
{
   getOverallStats()->increaseExtrasCollected();
   getRoundStats()->increaseExtrasCollected();
}


//----------------------------------------------------------------------------
/*!
*/
void Player::resetStats()
{
   getOverallStats()->reset();
   getRoundStats()->reset();
}


//----------------------------------------------------------------------------
/*!
*/
void Player::increaseFlameCount()
{
   mFlameCount++;
}


//----------------------------------------------------------------------------
/*!
*/
void Player::increaseBombCount()
{
   mBombCount++;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if bomb key is processed
*/
bool Player::isBombKeyLocked() const
{
   return mBombKeyLocked;
}


//----------------------------------------------------------------------------
/*!
   \param processed bomb key procesed
*/
void Player::setBombKeyLocked(bool processed)
{
   mBombKeyLocked = processed;
}



