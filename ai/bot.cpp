#include "bot.h"

// bot
#include "botaction.h"
#include "botconstants.h"
#include "botmap.h"
#include "botplayerinfo.h"
#include "botwalkaction.h"

// cmath
#include <math.h>

// Qt
#include <QElapsedTimer>

// defines
#define MIN_QUEUE_CHECK_SIZE 30


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
Bot::Bot(QObject *parent)
   : QThread(parent),
     mBotState(BotStateDead),
     mBotMap(0),
     mPlayerInfo(0),
     mX(0.0f),
     mY(0.0f),
     mXField(0.0f),
     mYField(0.0f),
     mId(-1),
     mBotKeysPressed(0),
     mDecisionRequired(false),
     mActionRequired(false),
     mStopped(false),
     mTransiterateTargetX(0),
     mTransiterateTargetY(0),
     mPlayerPositionValid(false)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::terminate()
{
   setState(BotStateDead);

   mWorkflowMutex.lock();
   setStopped(true);
   mIdleCondition.wakeOne();
   mWorkflowMutex.unlock();

   wait();

   cleanUpBot();
}


//-----------------------------------------------------------------------------
/*!
*/
Bot::~Bot()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::run()
{
   QMutexLocker locker(&mWorkflowMutex);
   qDebug("Bot::run(): waiting to wake up");

   while (!isStopped())
   {
      // initially bot is idle
      mIdleCondition.wait(&mWorkflowMutex);

      qDebug("Bot::run(): woken up");

      while (isActive())
      {
         if (isValid())
         {
            // some time measurement has been required since the bots
            // were acting weird in 10 player mode..
            //
            // "Bot::run(): player 6 took 26927ms for thinking"
            //
            // QElapsedTimer timer;
            // timer.start();

            think();

            // qDebug(
            //    "Bot::run(): player %d took %dms for thinking",
            //    getPlayerInfo()->getId(),
            //    timer.elapsed()
            // );
            //

            decide();
            act();
         }

         emit sync();
         msleep(100);
      }

      invalidate();
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if active
*/
bool Bot::isActive()
{
   return (mBotState == BotStateActive);
}


//-----------------------------------------------------------------------------
/*!
   \param botmap bot map
*/
void Bot::setBotMap(BotMap* botmap)
{
   mBotMap = botmap;
}


//-----------------------------------------------------------------------------
/*!
   \param info player info ptr
*/
void Bot::setPlayerInfo(BotPlayerInfo *info)
{
    mPlayerInfo = info;
}


//-----------------------------------------------------------------------------
/*!
   \return player info ptr
*/
BotPlayerInfo *Bot::getPlayerInfo() const
{
    return mPlayerInfo;
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::think()
{
   qDeleteAll(mOptions);
   mOptions.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::decide()
{
   mActions.clear();
   BotOption* bestOption = 0;
   int maxscore = INT_MIN;

   foreach (BotOption* option, mOptions)
   {
      if (option->getScore() > maxscore)
      {
         maxscore = option->getScore();
         bestOption = option;
      }

      // at the moment there's no option that is combinable
      if (option->isCombinable())
         mActions << option->getAction();
   }

   if (bestOption)
   {
      // do not execute an action twice
      if (!mActions.contains(bestOption->getAction()))
         mActions << bestOption->getAction();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::act()
{
   foreach (BotAction* action, mActions)
   {
      switch (action->getActionType())
      {
         case BotAction::ActionBomb:
         {
            if (DEBUG_EXECUTED_ACTIONS)
               qDebug("Bot::act(): BotAction::ActionBomb:");

            emit bomb();
            break;
         }

         case BotAction::ActionWalk:
         {
            if (DEBUG_EXECUTED_ACTIONS)
               qDebug("Bot::act(): BotAction::ActionWalk:");

            emit walk(((BotWalkAction*)action)->getWalkKeys());
            break;
         }

         case BotAction::ActionIdle:
         default:
            if (DEBUG_EXECUTED_ACTIONS)
               qDebug("Bot::act(): BotAction::ActionIdle:");

            emit walk(0);
            break;
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
*/
void Bot::updatePlayerId(int id)
{
   mId = id;
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
   \param x x position
   \param y y position
*/
void Bot::updatePlayerPosition(int id, float x, float y, float /*angle*/)
{
   if (id == mId)
   {
      mX = x;
      mY = y;

      mXField = floor(x);
      mYField = floor(y);

      setPlayerPositionValid(true);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return x position
*/
float Bot::getX() const
{
   return mX;
}


//-----------------------------------------------------------------------------
/*!
   \return y position
*/
float Bot::getY() const
{
   return mY;
}


//-----------------------------------------------------------------------------
/*!
   \return x field
*/
int Bot::getXField()
{
   return mXField;
}


//-----------------------------------------------------------------------------
/*!
   \return y field
*/
int Bot::getYField()
{
    return mYField;
}


//-----------------------------------------------------------------------------
/*!
  \param width map width
  \param height map height
  \return map
*/
BotMap *Bot::createMap(int width, int height)
{
   return new BotMap(width, height);
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::wakeUp()
{
   // clear bot state for next round
   reset();

   // qDebug("Bot::wakeUp: waking bot");
   setState(BotStateActive);
   mIdleCondition.wakeOne();
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::idle()
{
   setState(BotStateIdle);
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::die()
{
   setState(BotStateDead);
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::extraShake(int)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::markHazardousTemporary(int /*x*/, int /*y*/, int /*ms*/, int /*fieldCount*/)
{

}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::bombKicked(int /*startX*/, int /*startY*/, Constants::Direction, int /*flames*/)
{

}


//-----------------------------------------------------------------------------
/*!
   \return \c true if action is required
*/
bool Bot::isActionRequired()
{
   return mActionRequired;
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::reset()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::cleanUpBot()
{
}


//-----------------------------------------------------------------------------
/*!
   \param state bot state
*/
void Bot::setState(BotState state)
{
   mBotState = state;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if field has been reached
*/
bool Bot::isFieldReached()
{
   qFatal("Bot::isFieldReached(): rebel without a cause");

   bool reached = false;

   reached = (
            fabs(mX - ((float)mTransiterateTargetX + 0.5f)) < FIELD_REACHED_PRECISION
         && fabs(mY - ((float)mTransiterateTargetY + 0.5f)) < FIELD_REACHED_PRECISION
      );

   return reached;
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::invalidate()
{
   setPlayerPositionValid(false);
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if player data is valid
*/
bool Bot::isValid() const
{
   return isPlayerPositionValid();
}


//-----------------------------------------------------------------------------
/*!
   \param valid player postion valid flag
*/
void Bot::setPlayerPositionValid(bool valid)
{
   mPlayerPositionValid = valid;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if player position is valid
*/
bool Bot::isPlayerPositionValid() const
{
   return mPlayerPositionValid;
}



//-----------------------------------------------------------------------------
/*!
   \param keysPressed bot's keys pressed
*/
void Bot::setBotKeysPressed(qint8 keysPressed)
{
   mBotKeysPressed = keysPressed;
}


//-----------------------------------------------------------------------------
/*!
   \return bot's keys pressed
*/
qint8 Bot::getBotKeysPressed() const
{
   return mBotKeysPressed;
}


//-----------------------------------------------------------------------------
/*!
*/
void Bot::updatePositionQueue()
{
   QPoint p(getXField(), getYField());

   if (!mPositionQueue.isEmpty())
   {
      if (mPositionQueue.last() != p)
      {
         mPositionQueue.enqueue(p);
      }

      while (mPositionQueue.size() > MIN_QUEUE_CHECK_SIZE)
      {
         mPositionQueue.dequeue();
      }
   }
   else
   {
      mPositionQueue.enqueue(p);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param p point
   \return qpoint hash
*/
inline uint qHash(const QPoint& p)
{
    return p.x() * 1000 + p.y();
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if position queue recurs
*/
bool Bot::isPositionQueueRecurrent() const
{
   bool recurrent = false;

   if (mPositionQueue.size() >= MIN_QUEUE_CHECK_SIZE)
   {
      QSet<QPoint> points;

      foreach (const QPoint& p, mPositionQueue)
      {
         points.insert(p);
      }

      if (points.size() <= 3)
      {
         qWarning(
            "Bot::checkPositionQueueForRecurrence(): "
            "bot positioning recurs"
         );

         recurrent = true;
      }
   }

   return recurrent;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if the bot is stopped
*/
bool Bot::isStopped() const
{
   mMemberMutex.lock();
   bool stopped = mStopped;
   mMemberMutex.unlock();

   return stopped;
}


//-----------------------------------------------------------------------------
/*!
   \param stopped stopped flag
*/
void Bot::setStopped(bool stopped)
{
   mMemberMutex.lock();
   mStopped = stopped;
   mMemberMutex.unlock();
}


//-----------------------------------------------------------------------------
/*!
   \param config reference to server configuration
*/
void Bot::setServerConfiguration(const ServerConfiguration & config)
{
   mServerConfiguration = config;
}


//-----------------------------------------------------------------------------
/*!
   \return reference to server configuration
*/
const ServerConfiguration &Bot::getServerConfiguration() const
{
   return mServerConfiguration;
}


//-----------------------------------------------------------------------------
/*!
   \return walk keys
*/
qint8 Bot::computeWalkKeys() const
{
   qint8 keysPressed = 0;
   float fieldCenter = 0.5f;

   if (mX - fieldCenter < mTransiterateTargetX)
      keysPressed |= Constants::KeyRight;
   else if (mX - fieldCenter > mTransiterateTargetX)
      keysPressed |= Constants::KeyLeft;

   if (mY - fieldCenter < mTransiterateTargetY)
      keysPressed |= Constants::KeyDown;
   else if (mY - fieldCenter > mTransiterateTargetY)
      keysPressed |= Constants::KeyUp;

   return keysPressed;
}
