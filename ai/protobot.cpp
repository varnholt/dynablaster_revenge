// header
#include "protobot.h"

// astar
#include "astarmap.h"
#include "astarpathfinding.h"

// ai
#include "bombchainreaction.h"
#include "botbombaction.h"
#include "botbombmapitem.h"
#include "botcharacter.h"
#include "botconstants.h"
#include "botidleaction.h"
#include "botmap.h"
#include "botplayerinfo.h"
#include "botwalkaction.h"
#include "protobotinsults.h"
#include "protobotmemory.h"

// shared
#include "mapitem.h"
#include "playerdisease.h"
#include "weighted.h"

// math
#include <math.h>

// Qt
#include <QElapsedTimer>
#include <QStringList>
#include <QTime>

// defines
#define MANHATTAN_LENGTH_MAX_EXTRAS 8
#define MANHATTAN_LENGTH_MAX_ATTACK 5
#define MANHATTAN_LENGTH_ESCAPE_NEAR_MAX 5
#define MANHATTAN_LENGTH_ESCAPE_MEDIUM_MIN 6
#define MANHATTAN_LENGTH_ESCAPE_MEDIUM_MAX 10


//-----------------------------------------------------------------------------
/*!
*/
ProtoBot::ProtoBot()
 : mFieldScores(0),
   mFieldBombTimes(0),
   mHazardousTemorary(0),
   mBotCharacter(0),
   mPlayerInfoMap(0),
   mInsults(0),
   mBombChainReaction(0),
   mMemory(0),
   mIdle(0),
   mScoringCurrentHazardous(false),
   mScoringPrepareAttack(false),
   mScoringEscape(false),
   mScoringExtra(false),
   mScoringPrepareBombStone(false),
   mScoringBomb(false),
   mScoringAttackPossible(false),
   mScoringBombStonePossible(false),
   mDebugBreakpoint(false),
   mDebugPaths(DEBUG_PATH),
   mDebugEscapePaths(DEBUG_ESCAPE_PATH),
   mDebugMapItems(DEBUG_MAPITEMS),
   mDebugScores(DEBUG_SCORES),
   mDebugKeysPressed(DEBUG_KEYSPRESSED),
   mDebugPossibleActions(DEBUG_POSSIBLE_ACTIONS),
   mDebugExecutedActions(DEBUG_EXECUTED_ACTIONS),
   mDebugCurrentHazardous(DEBUG_CURRENT_HAZARDOUS),
   mDebugWalkAction(DEBUG_WALK_ACTION),
   mDebugBombDrop(DEBUG_BOMB_DROP)
{
   qsrand(QTime::currentTime().msec());

   mBotCharacter = new BotCharacter();
   mBotCharacter->setCharacter(
      randomize(2, 4),  // extra (2 is minimum as the value is multiplied)
      randomize(2, 4),  // bomb stone
      10 // randomize(0, 4)   // attack
   );

   mDirections << QPoint(0, -1);
   mDirections << QPoint(0, 1);
   mDirections << QPoint(-1, 0);
   mDirections << QPoint(1, 0);
   // mInsults = new ProtoBotInsults(this);

   mBombChainReaction = new BombChainReaction();
   mBombChainReaction->initDirections();

   mMemory = new ProtoBotMemory();
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::cleanUpBot()
{
   Bot::cleanUpBot();

   delete mBotCharacter;
   delete mBombChainReaction;
   delete mMemory;   
   delete[] mFieldScores;
   delete[] mFieldBombTimes;
   delete[] mHazardousTemorary;

   mBotCharacter = 0;
   mBombChainReaction = 0;
   mMemory = 0;
   mFieldScores = 0;
   mFieldBombTimes = 0;
   mHazardousTemorary = 0;
}


//-----------------------------------------------------------------------------
/*!
*/
ProtoBot::~ProtoBot()
{
   terminate();
}


//-----------------------------------------------------------------------------
/*!
   \param player id
   \param x position
   \param y position
   \param ang player angle
*/
void ProtoBot::updatePlayerPosition(int id, float x, float y, float ang)
{
   Bot::updatePlayerPosition(id, x, y, ang);

   // these are for debugging purposes only
   // remove this code as soon we're cool.
   // updatePositionQueue();

   /*
   if (isPositionQueueRecurrent())
   {
      setDebugBreakpointEnabled(true);

      // setDebugEscapePathsEnabled(true);
      // setDebugBombDropEnabled(true);
      // setDebugCurrentHazardousEnabled(true);
      // setDebugExecutedActionsEnabled(true);
      // setDebugKeysPressedEnabled(true);
      // setDebugMapItemsEnabled(true);
      // setDebugPathsEnabled(true);
      // setDebugPossibleActionsEnabled(true);
      // setDebugScoresEnabled(true);
      // setDebugWalkActionEnabled(true);
   }
   */
}


//-----------------------------------------------------------------------------
/*!
   \param itemId item that contains an extra
*/
void ProtoBot::extraShake(int itemId)
{
   Bot::extraShake(itemId);

   mExtraShakeIds << itemId;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::wakeUp()
{
   Bot::wakeUp();
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::decide()
{
   Bot::decide();
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::act()
{
   Bot::act();
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::reset()
{
   mExtraShakeIds.clear();
   mMemory->reset();
   resetIdleCounter();
   resetHazardousTemporary();
   resetFieldBombTimes();
}


//-----------------------------------------------------------------------------
/*!
   \param random minimum
   \param random maximum
   \return randomized value
*/
int ProtoBot::randomize(int min, int max)
{
   return qrand() % ((max + 1) - min) + min;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \return field score at x,y
*/
int ProtoBot::getRemainingBombTime(int x, int y) const
{
   return mFieldBombTimes[y * mBotMap->getWidth() + x];
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param score score to set
*/
void ProtoBot::setRemainingBombTime(int x, int y, int score)
{
   int currentScore = getRemainingBombTime(x, y);
   mFieldBombTimes[y * mBotMap->getWidth() + x] = qMin(currentScore, score);
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \return field score at x,y
*/
int ProtoBot::getScore(int x, int y) const
{
   return mFieldScores[y * mBotMap->getWidth() + x];
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param score score to set
*/
void ProtoBot::setScore(int x, int y, int score)
{
   mFieldScores[y * mBotMap->getWidth() + x] = score;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param factor factor to apply
*/
void ProtoBot::multiplyScore(int x, int y, int factor)
{
   if (factor != 0)
   {
      mFieldScores[y * mBotMap->getWidth() + x] *= factor;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \return ms value
*/
int ProtoBot::getHazardousTemporary(int x, int y) const
{
   return mHazardousTemorary[y * mBotMap->getWidth() + x];
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param ms millis
   \param fieldCount no of fields to mark
*/
void ProtoBot::markHazardousTemporary(int x, int y, int ms, int fieldCount)
{
   if (
         x >= 0
      && x < mBotMap->getWidth()
      && y >= 0
      && y < mBotMap->getHeight()
   )
   {
      mHazardousTemorary[y * mBotMap->getWidth() + x] = ms;
   }

   if (fieldCount > 0)
   {
      for (int i = 1; i <= fieldCount; i++)
      {
         int left  = x - i;
         int right = x + i;
         int up    = y - i;
         int down  = y + i;

         if (left >= 0)
            markHazardousTemporary(left, y, ms);

         if (right < mBotMap->getWidth())
            markHazardousTemporary(right, y, ms);

         if (up >= 0)
            markHazardousTemporary(x, up, ms);

         if (down < mBotMap->getHeight())
            markHazardousTemporary(x, down, ms);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param ms elapsed
*/
void ProtoBot::updateHazardousTemporary(int ms)
{
   int diff = 0;
   for (int y = 0; y < mBotMap->getHeight(); y++)
   {
      for (int x = 0; x < mBotMap->getWidth(); x++)
      {
         int val = getHazardousTemporary(x, y);

         if (val > 0)
         {
            diff = val - ms;
            markHazardousTemporary(x, y, qMax(0, diff));

            if (diff > 0)
               setScore(x, y, -1);
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::resetHazardousTemporary()
{
   int fieldSize = mBotMap->getWidth() * mBotMap->getHeight();
   memset(mHazardousTemorary, 0, sizeof(int) * fieldSize);
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::resetFieldBombTimes()
{
   for (int y = 0; y < mBotMap->getHeight(); y++)
      for (int x = 0; x < mBotMap->getWidth(); x++)
         mFieldBombTimes[y * mBotMap->getWidth() + x] = 0xFFFF;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::bugTrack1()
{
   /*
      [S][S][S]
      [S][ ][S]
      [S][P][S]
      [S][ ][S]
      [S][S][S]

   p.x = 6
   p.y = 5

   => reachable position count = 3

   */

   if (
         getXField() == 6
      && getYField() == 5
   )
   {
      if (mBotMap->getReachablePositions().size() == 3)
      {
         qDebug("ProtoBot::bugTrack1(): issue 1 spotted");
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::bugTrack2()
{
   if (
         mBotMap->getItem(getXField(), getYField())
      && mBotMap->getItem(getXField(), getYField())->getType() == MapItem::Bomb
      && getScore(getXField(), getYField()) == 1
   )
   {
      qDebug("ErrorConditionTest");
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::bugTrack3()
{
   increaseIdleCounter();

   if (getIdleCounter() % 5)
   {
      QPoint p(getXField(), getYField());

      mLastPositions.enqueue(p);

      while (mLastPositions.size() > 100)
         mLastPositions.dequeue();

      if (mLastPositions.size() > 90)
      {
         bool diff = true;
         QPoint prev = mLastPositions.first();
         foreach(const QPoint& p, mLastPositions)
         {
            if (
                  prev.x() != p.x()
               || prev.y() != p.y()
            )
            {
               diff = false;
            }

            prev = p;
         }

         if (diff)
         {
            qDebug("player is totally idle :(");
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \return idle counter
*/
int ProtoBot::getIdleCounter() const
{
   return mIdle;
}


//-----------------------------------------------------------------------------
/*!
  \param value idle counter
*/
void ProtoBot::setIdleCounter(int value)
{
   mIdle = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::resetIdleCounter()
{
   mIdle = 0;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::increaseIdleCounter()
{
   mIdle++;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if we can escape safely
*/
bool ProtoBot::isSafeEscapePossible()
{
   QList<QPoint> reachablePositionsFiltered =
      reachablePositionsLeft(
         getXField(),
         getYField(),
         mPlayerInfo->getFlameCount(),
         mBotMap->getReachablePositions()
      );

   // optimization
   // filter list of points again by manhattan length
   reachablePositionsFiltered =
      Map::getManhattanFiltered(
         QPoint(getXField(), getYField()),
         reachablePositionsFiltered,
         MANHATTAN_LENGTH_MAX_ATTACK
      );

   if (reachablePositionsFiltered.size() > 100)
   {
      qWarning("ProtoBot::isAttackPossible(): cpu usage exceeded");
   }

   bool foundSafeDropPos = false;
   foreach (const QPoint& potentialSafePoint, reachablePositionsFiltered)
   {
      // score path to bomb drop position
      findPath(potentialSafePoint.x(), potentialSafePoint.y());
      int pathLength = mPathFinding.getPathLength();

      // a safe position must not be our current position
      if (pathLength > 0)
      {
         if  (!isPathHazardous(mPathFinding.getPath()))
         {
            foundSafeDropPos = true;
         }
      }

      clearPath();

      if (foundSafeDropPos)
         break;
   }

   return foundSafeDropPos;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if attack is a cool idea
*/
bool ProtoBot::isAttackPossible()
{   
   if (isNoBombInfectionActive())
      return false;

   bool attackPossible = false;

   if (mBotMap->isBombAmountConsumed(mId, mPlayerInfo->getBombCount()))
   {
      bool bombDropDeadly =
         mBotMap->isBombDropDeadly(
            getXField(),
            getYField(),
            mPlayerInfo->getFlameCount(),
            mEnemyPositions
         );

      bool foundSafeDropPos = isSafeEscapePossible();

      attackPossible = bombDropDeadly && foundSafeDropPos;
   }

   return attackPossible;
}


//-----------------------------------------------------------------------------
/*!
*/
bool ProtoBot::isBombStonePossible()
{
   int xField = getXField();
   int yField = getYField();
   QPoint bombStonePosition = getBombStonePosition();

   bool atBombStonePosition = (
         xField == bombStonePosition.x()
      && yField == bombStonePosition.y()
   );


   /*
      goal: only place bombs in the X'ed areas of a field

      +---+-----------+---+
      |   |           |   |
      +---+-----------+---+
      |   |XXXXXXXXXXX|   |
      |   |XXXXXXXXXXX|   |
      |   |XXXXXXXXXXX|   |
      |   |XXXXXXXXXXX|   |
      |   |XXXXXXXXXXX|   |
      +---+-----------+---+
      |   |           |   |
      +---+-----------+---+

   */

   bool withinEpsilon = false;

   if (atBombStonePosition)
   {
      float eps = 0.05f;

      float x = (float)xField;
      float y = (float)yField;

      withinEpsilon =  (
            getX() > (x + eps)
         && getX() < (x + 1.0f - eps)
      )
      &&
      (
            getY() > (y + eps)
         && getY() < (y + 1.0f - eps)
      );
   }

   bool escapePossible = isSafeEscapePossible();

   return atBombStonePosition && escapePossible && withinEpsilon;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::markReachableFields()
{
   setScore(getXField(), getYField(), 1);

   foreach (const QPoint& p, mBotMap->getReachablePositions())
   {
      setScore(
         p.x(),
         p.y(),
         1
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::markHazardousFields()
{
   // don't
   QList<BotBombMapItem *> bombs = mBotMap->getBombs();

   int x = 0;
   int y = 0;
   int xi = 0;
   int yi = 0;

   foreach (BotBombMapItem* bomb, bombs)
   {
      x = bomb->getX();
      y = bomb->getY();

      setScore(x, y, -1);

      foreach (const QPoint& dir, mDirections)
      {
         // players infected with "small bomb" disease could have
         // only one flame..
         //
         //         if (bomb->getFlames() < 2)
         //         {
         //            qWarning(
         //               "ProtoBot::markHazardousFields(): the bot is "
         //               "obviously fucked. that is too bad."
         //            );
         //         }

         for (int i = 1; i <= bomb->getFlames(); i++)
         {
            xi = x + i * dir.x();
            yi = y + i * dir.y();

            if (
                  xi >= 0 && xi < mBotMap->getWidth()
               && yi >= 0 && yi < mBotMap->getHeight()
            )
            {  
               setScore(xi, yi, -1);

               // we hit something
               if (mBotMap->getItem(xi, yi))
               {
                  break;
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::updateRemainingBombTimes()
{
   // init
   QTime currentTime = QTime::currentTime();
   int tickTime = getServerConfiguration().getBombTickTime();
   int timeDiff = 0;
   int timeLeft = 0;
   int x = 0;
   int y = 0;
   int xi = 0;
   int yi = 0;

   // clear remaining bomb time array
   resetFieldBombTimes();

   mBombChainReaction->setBotMap(mBotMap);
   mBombChainReaction->compute();

   BombChainReaction::ChainList chains = mBombChainReaction->getDetonationChain();

   for (int c = 0; c < chains.size(); c++)
   {
      QList<BotBombMapItem*> connectedBombs = chains.at(c);

      int min = 0xFFFF;

      foreach (BotBombMapItem* item, connectedBombs)
      {
         timeDiff = item->getDropTime().msecsTo(currentTime);
         timeLeft = qMax(tickTime - timeDiff, 0);

         min = qMin(min, timeLeft);
      }

      // now create a map of detonation times
      foreach (BotBombMapItem* item, connectedBombs)
      {
         x = item->getX();
         y = item->getY();

         setRemainingBombTime(x, y, min);

         foreach (const QPoint& dir, mDirections)
         {
            for (int i = 1; i <= item->getFlames(); i++)
            {
               xi = x + i * dir.x();
               yi = y + i * dir.y();

               if (
                     xi >= 0 && xi < mBotMap->getWidth()
                  && yi >= 0 && yi < mBotMap->getHeight()
               )
               {
                  setRemainingBombTime(xi, yi, min);

                  // we hit something
                  if (mBotMap->getItem(xi, yi))
                     break;
               }
               else
               {
                  // we hit a wall
                  break;
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::resetScores()
{
   int fieldSize = mBotMap->getWidth() * mBotMap->getHeight();

   memset(
      mFieldScores,
      0,
      fieldSize * sizeof(int)
   );

   /*
   for (int yi = 0; yi < mBotMap->getHeight(); yi++)
   {
      for (int xi = 0; xi < mBotMap->getWidth(); xi++)
      {
         setScore(xi, yi, 0);
      }
   }
   */
}


//-----------------------------------------------------------------------------
/*!
   \return bomb stone position
*/
QPoint ProtoBot::getBombStonePosition() const
{
   return mBombStonePosition;
}


//-----------------------------------------------------------------------------
/*!
   \param weightedPoint bomb stone position
*/
void ProtoBot::setBombStonePosition(const QPoint& weightedPoint)
{
   mBombStonePosition = weightedPoint;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::resetBombStonePosition()
{
   // make a backup for debugging purposes
   setBombStonePositionPrevious(getBombStonePosition());

   // reset position
   mBombStonePosition.setX(-1);
   mBombStonePosition.setY(-1);
}


//-----------------------------------------------------------------------------
/*!
   \return previous bomb stone position
*/
QPoint ProtoBot::getBombStonePositionPrevious() const
{
   return mBombStonePositionPrevious;
}


//-----------------------------------------------------------------------------
/*!
   \param previous previous bomb stone position
*/
void ProtoBot::setBombStonePositionPrevious(const QPoint& previous)
{
   mBombStonePositionPrevious = previous;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::resetScoringFlags()
{
   mScoringCurrentHazardous    = false;
   mScoringPrepareAttack       = false;
   mScoringEscape              = false;
   mScoringExtra               = false;
   mScoringPrepareBombStone    = false;
   mScoringBomb                = false;
   mScoringAttackPossible      = false;
   mScoringBombStonePossible   = false;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::scoreFields()
{
   resetScoringFlags();

   // reset bomb bombdrop position
   resetBombStonePosition();

   // these ought to be removed at a later stage
   //   int timeForUpdateReachablePositions = 0;
   //   int timeForMarkHazardousFields = 0;
   //   int timeForUpdateEscapeScore = 0;
   //   int timeForUpdateExtraScore = 0;
   //   int timeForUpdateAttackScore = 0;
   //   int timeForUpdateBombStoneScore = 0;
   //   int timeForIsAttackPossible = 0;
   //   int timeForIsAtBombStonePosition = 0;
   //
   // QElapsedTimer timer;
   // timer.start();

   // init
   // mBotMap->updateReachablePositions(getXField(), getYField());
   mBotMap->updateReachablePositionsRandomized(getXField(), getYField());

   // timeForUpdateReachablePositions = timer.elapsed();
   // timer.restart();
   //
   //   if (mScoringCurrentHazardous)
   //   {
   //      if (isDebugCurrentHazardousEnabled())
   //      {
   //         qDebug("ProtoBot::scoreFields: current field must be left!");
   //      }
   //   }

   // reset scores
   resetScores();

   // didn't re-occur
   // bugTrack3();

   // mark hazardous fields
   // timer.restart();
   markReachableFields();
   markHazardousFields();
   // markHazardousDeadEnds();
   updateHazardousTemporary(100);

   // timeForMarkHazardousFields = timer.elapsed();

   // update bomb detonation chains
   updateRemainingBombTimes();

   mScoringCurrentHazardous = (getScore(getXField(), getYField()) < 0);

   // if current position must be left, 'highlight' path to a save field
   if (mScoringCurrentHazardous)
   {
      // timer.restart();
      mScoringEscape = updateEscapeScore();
      // timeForUpdateEscapeScore = timer.elapsed();

      // we can't escape, which sucks
      if (!mScoringEscape)
      {
         updateLeastHazardousField();
      }
   }

   // increase individual field score by marking extras
   // calculate path to extras, find nearest extras (if max length is not exceeded)
   // then multiply path to extras by 2
   if (!mScoringCurrentHazardous)
   {
      // timer.restart();
      mScoringExtra = updateExtraScore();
      // timeForUpdateExtraScore = timer.elapsed();
   }

   if (!mScoringCurrentHazardous && !mScoringExtra)
   {
      // timer.restart();
      mScoringPrepareAttack = updateAttackScore();
      // timeForUpdateAttackScore = timer.elapsed();
   }

   // walk to good bomb-drop position. find that reachable field that has
   // the maximum stones to bomb away. the chosen position must have an escape
   // path.
   if (
        ! mScoringEscape
      &&! mScoringExtra
      &&! mScoringPrepareAttack
   )
   {
      // timer.restart();
      mScoringPrepareBombStone = updateBombStoneScore();
      // timeForUpdateBombStoneScore = timer.elapsed();
   }

   // timer.restart();
   mScoringAttackPossible = isAttackPossible();
   // timeForIsAttackPossible = timer.elapsed();

   // timer.restart();
   mScoringBombStonePossible = isBombStonePossible();
   // timeForIsAtBombStonePosition = timer.elapsed();

   // bomb action. drop a bomb if there is a path to escape after dropping it
   if (
         (
            mScoringPrepareBombStone
         && mScoringBombStonePossible
      )
      || (
            mScoringPrepareAttack
         && mScoringAttackPossible
      )
   )
   {
      if (mPlayerInfo->getBombCount() > mBotMap->getBombs(mId).size())
      {
         BotOption* option = new BotOption();

         // bombs have 'leading' :)
         option->setScore(2.0f);

         option->setAction(new BotBombAction());
         mOptions.push_back(option);
         mScoringBomb = true;
      }
   }

   // long distance walk action
   // presumes all other actions failed
   if (
         !mScoringEscape
      && !mScoringExtra
      && !mScoringPrepareBombStone
      && !mScoringBomb
      && !mScoringPrepareAttack
   )
   {
      int ldX = mXField;
      int ldY = mYField;

      if (evaluateLongDistance(ldX, ldY))
      {
         // checking the score of this field is a bit redundant since
         // we already checked for the presence of any items within the
         // given manhattan distance.. i.e. the score would be smaller
         // than 0 if there was a bomb item nearby; which is not the case.
         // anyway.. entering a hazardous field is a bad idea, so we just
         // do the check here.
         if (getScore(ldX, ldY) >= 0)
            setScore(ldX, ldY, 2);
      }
   }

   /*
   if (mScoringCurrentHazardous)
   {
      QList<QPoint> deadEnd = analyzeDeadEnd(mXField, mYField);

      if (!deadEnd.isEmpty())
      {
         qDebug("dead end.");
      }
   }
   */

//   if (isDebugPossibleActionsEnabled())
//   {
//      // increase field scores by marking good paths (origin = current position)
//      // remember not to mark the player's current field
//      if (
//            mScoringEscape
//         || mScoringExtra
//         || mScoringPrepareBombStone
//         || mScoringBomb
//         || mScoringPrepareAttack
//      )
//      {
//         qDebug(
//            "ProtoBot::scoreFields: action: escape: %d, extra: %d, "
//            "gotobombplacepos %d, placebomb: %d, attack: %d",
//            mScoringEscape,
//            mScoringExtra,
//            mScoringPrepareBombStone,
//            mScoringBomb,
//            mScoringPrepareAttack
//         );
//      }
//   }
//
//   if (isDebugMapItemsEnabled())
//      dynamic_cast<AStarMap*>(mBotMap)->debugMapItems();
//
//   if (isDebugScoresEnabled())
//      debugScores();

   /*
   qDebug(
      "consumed: REACHP: %d, HAZARDF: %d, ESC: %d, EXTRA: %d, ATTSCR: %d, "
      "BSTONE: %d, ATTPSS: %d, BSTONEPSS: %d",
      timeForUpdateReachablePositions,
      timeForMarkHazardousFields,
      timeForUpdateEscapeScore,
      timeForUpdateExtraScore,
      timeForUpdateAttackScore,
      timeForUpdateBombStoneScore,
      timeForIsAttackPossible,
      timeForIsAtBombStonePosition
   );
   */
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::debugScores()
{
   qDebug("ProtoBot::debugScores(): start");

   for (int yi = 0; yi < mBotMap->getHeight(); yi++)
   {
      QString line;
      for (int xi = 0; xi < mBotMap->getWidth(); xi++)
      {
         line.append(QString("%1").arg(getScore(xi, yi), 3));

         // put >< around current field
         if (yi == getYField() && xi + 1 == getXField())
            line.append(">");
         else if (yi == getYField() && xi == getXField())
            line.append("<");
         else
            line.append("|");
      }

      qDebug("%s", qPrintable(line));
   }

   qDebug("ProtoBot::debugScores(): end");
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if escape is a good idea
*/
bool ProtoBot::updateEscapeScore()
{
   int score = 0;
   int enemyScore = 0;
   bool escape = false;
   int shortestPathLength = INT_MAX;
   int pathLength = 0;

   // a little hacky:
   // those safe positions that end at a position an enemy
   // is already on should be avoided
   // this is done by just increasing the calculated path length
   QList<QPoint> enemyPositions = getLivingEnemyPositions();
   QList<QPoint> futureEnemyPositions = getLivingEnemyFuturePositions();

   int fieldSize = mBotMap->getWidth() * mBotMap->getHeight();
   int* enemies = new int[fieldSize];

   memset(enemies, 0, sizeof(int)*fieldSize);

   foreach (const QPoint& e, enemyPositions)
   {
      // having an enemy on our escape path is something to avoid
      enemies[e.x() + mBotMap->getWidth() * e.y()] += 3;

      /*
         avoid enemy positions

         +---+---+---+
         |   | E |   |
         +---+---+---+
         |XXX| ^ |XXX|
         +---+-|-+---+
         |  <-(P)->  |
         +---+---+---+
      */

      // score fields directly around an enemy with -2
      if (e.x() - 1 >= 0)
         enemies[(e.x() - 1) + mBotMap->getWidth() * e.y()] += 2;

      if (e.x() + 1 < mBotMap->getWidth())
         enemies[(e.x() + 1) + mBotMap->getWidth() * e.y()] += 2;

      if (e.y() - 1 >= 0)
         enemies[e.x() + mBotMap->getHeight() * (e.y() - 1)] += 2;

      if (e.y() + 1 < mBotMap->getHeight())
         enemies[e.x() + mBotMap->getWidth() * (e.y() + 1)] += 2;

      // score fields nearby an enemy with -1
      if (e.x() - 2 >= 0)
         enemies[(e.x() - 2) + mBotMap->getWidth() * e.y()] += 1;

      if (e.x() + 2 < mBotMap->getWidth())
         enemies[(e.x() + 2) + mBotMap->getWidth() * e.y()] += 1;

      if (e.y() - 2 >= 0)
         enemies[e.x() + mBotMap->getHeight() * (e.y() - 2)] += 1;

      if (e.y() + 2 < mBotMap->getHeight())
         enemies[e.x() + mBotMap->getWidth() * (e.y() + 2)] += 1;
   }

   // the future enemy position is damn important. if we don't set this
   // to *at least* the value the enemy positions have, both players will
   // follow each other until the time is up. that looks totally annoying
   // and surely does not help winning the game.
   foreach (const QPoint& e, futureEnemyPositions)
      enemies[e.x() + mBotMap->getWidth() * e.y()] += 2;

   // check if neighbour positions are safe before we're going to find an
   // escape path
   QList<QPoint> neighbours =
      mBotMap->getReachableNeighborPositionsRandomized(
         getXField(),
         getYField()
      );

   QList<Weighted<QPoint, int> > weightedPoints;

   foreach (const QPoint& p, neighbours)
   {
      if (getScore(p.x(), p.y()) >= 0)
      {
         // set escape action
         escape = true;

         int enemyCount = enemies[p.x() + p.y() * mBotMap->getWidth()];
         weightedPoints.append(Weighted<QPoint, int>(p, enemyCount));
      }
   }

   if (escape)
   {
      qSort(weightedPoints);
      QPoint best = weightedPoints.last().getObject();

      // make a copy of the computed path
      mBestEscapePath.clear();
      mBestEscapePath.prepend(best);
   }

   // there's no direct neighbour position that's safe; so we have to find
   // a full escape path
   if (!escape)
   {
      // maybe use these 3 groups
      // - manhattan length 1..5
      // - manhattan length 6..10
      // - manhattan length > 11 <- choosing this group is pointless
      //                            the bot will never survive the way there
      //
      // => if escape is true, then break
      QList<QPoint> reachablePoints = mBotMap->getReachablePositions();
      QList<QPoint> reachablePointsNear;
      QList<QPoint> reachablePointsMedium;

      // only work with reachable points within short and medium
      // distance, omit all the others
      // also pre-sort the reachable points by their manhattan distance
      // to the current field
      reachablePointsNear =
         Map::getManhattanFiltered(
            QPoint(mXField, mYField),
            reachablePoints,
            MANHATTAN_LENGTH_ESCAPE_NEAR_MAX
         );

      reachablePointsMedium =
         Map::getManhattanFiltered(
            QPoint(mXField, mYField),
            reachablePoints,
            MANHATTAN_LENGTH_ESCAPE_MEDIUM_MIN,
            MANHATTAN_LENGTH_ESCAPE_MEDIUM_MAX
         );

      QList< QList<QPoint> > reachablePointLists;
      reachablePointLists << reachablePointsNear;
      reachablePointLists << reachablePointsMedium;

      foreach(const QList<QPoint>& list, reachablePointLists)
      {
         // if one group of reachable points delivered a suitable escape path
         // then abort here
         if (!escape)
         {
            // find shortest path to non-hazardous position within the reachable positions
            foreach (const QPoint& p, list)
            {
               // reachable fields get a score of 1, checking against 0 doesn't
               // matter though
               score = getScore(p.x(), p.y());

               if (
                     score >= 0
                  && (
                         p.x() != getXField()
                      || p.y() != getYField()
                  )
               )
               {
                  // since we can escape
                  // -> set escape action (phew!)
                  escape = true;

                  // now find a nice short path to that safe position
                  findPath(p.x(), p.y());
                  pathLength = mPathFinding.getPathLength();

                  // path was found
                  if (pathLength > 0)
                  {
                     // go through every field of that path and apply the enemy
                     // effect on the path length (the more enemies on the path
                     // the worse the path length).
                     foreach (AStarNode* node, mPathFinding.getPath())
                     {
                        enemyScore =
                           enemies[
                                node->getX()
                              + node->getY() * mBotMap->getWidth()
                           ];

                        pathLength *= (enemyScore + 1);
                     }

                     // if (isPathHazardous(mPathFinding.getPath()))
                     pathLength *=
                        (getHazardousFieldCount(mPathFinding.getPath()) + 1);

                     // path is better
                     if (pathLength < shortestPathLength)
                     {
                        shortestPathLength = pathLength;

                        // make a copy of the computed path
                        mBestEscapePath.clear();

                        foreach (AStarNode* node, mPathFinding.getPath())
                           mBestEscapePath.prepend(QPoint(node->getX(), node->getY()));

                        // if only one field needs to be traversed this is most likely
                        // the best path we'll get
                        if (pathLength == 1)
                           break;
                     }
                  }

                  // this happens only when a previously free field is suddenly blocked
                  // by a kicked bomb => the else case is pretty boring and harmless
                  //
                  // else
                  // {
                  //    qWarning("ProtoBot::isEscapePossible(): no path found to a reachable position");
                  // }

                  clearPath();
               }
            }
         }
      }
   }

   // QStringList escapePathString;

   if (escape)
   {
      foreach(const QPoint& p, mBestEscapePath)
      {
         // as only the neighbored fields are examined later, increase
         // the score with each field is not required here. it's only
         // required the *current* field has a negative score and the
         // surrounding fields get more than that.
         // also do not alter the current field's score as it must be kept
         // at -1 to indicate it must be left!
         if (
               p.x() != getXField()
            || p.y() != getYField()
         )
         {
            setScore(p.x(), p.y(), 2);
         }
      }

   //      if (mDebugEscapePaths)
   //      {
   //         foreach(const QPoint& p, mBestEscapePath)
   //            escapePathString
   //               << QString("(%1, %2)")
   //                  .arg(p.x())
   //                  .arg(p.y());
   //      }
   }
   // else
   // {
   //     qDebug("ProtoBot::scoreFields(): no path found!");
   // }

   //   if (mDebugEscapePaths)
   //   {
   //      qDebug(
   //         "ProtoBot::scoreFields(): escape from (%d, %d) via: %s",
   //         getXField(),
   //         getYField(),
   //         qPrintable(escapePathString.join(" -> "))
   //      );
   //   }

   delete[] enemies;

   return escape;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if going for extras is a good idea
*/
bool ProtoBot::updateExtraScore()
{
   mBotMap->updateReachableExtras();
   QList<QPoint> extraPoints;
   int pathLength = 0;
   bool goodIdea = false;

   // let's check our memory first: if we have an extra in our memory
   // then we should focus on that and omit the standard extra evaluation
   // procedure.
   if (mMemory->isExtraPositionValid())
   {
      MapItem* memoryExtra = 0;

      memoryExtra =
         mBotMap->getItem(
            mMemory->getExtraPositionX(),
            mMemory->getExtraPositionY()
         );

      if (
            memoryExtra
         && memoryExtra->getType() == MapItem::Extra
      )
      {
         extraPoints.append(
            QPoint(
               mMemory->getExtraPositionX(),
               mMemory->getExtraPositionY()
            )
         );
      }
      else
      {
         // the extra is not there any more
         mMemory->invalidateExtraPosition();
      }
   }

   // when we look around for extras, then don't look for extras
   // which are located too far away
   if (extraPoints.isEmpty())
   {
      extraPoints = mBotMap->getReachableExtras();

      extraPoints =
         Map::getManhattanFiltered(
            QPoint(getXField(), getYField()),
            extraPoints,
            MANHATTAN_LENGTH_MAX_EXTRAS
         );
   }

   QList<QPoint> shortestExtraPath;

   foreach(const QPoint& p, extraPoints)
   {
      // check if the extra is "occupied" by an enemy
      if (!evaluateDeadEndSituation(p.x(), p.y()))
         continue;

      findPath(p.x(), p.y());
      pathLength = mPathFinding.getPathLength();

      if (pathLength > 0)
      {
         QList<QPoint> points;

         QList<AStarNode*> path = mPathFinding.getPath();

         // there is a safe way that leads to an extra
         if (!isPathHazardous(path))
         {
            goodIdea = true;

            foreach (AStarNode* node, path)
            {
               QPoint p(node->getX(), node->getY());
               points << p;
            }

            // if the extra path is empty, assign the new path immediately;
            // if the path is not empty, assign it if the path is shorter than
            // the existing one.
            if (shortestExtraPath.isEmpty())
            {
               shortestExtraPath = points;
            }
            else
            {
               if (points.size() < shortestExtraPath.size())
               {
                  shortestExtraPath = points;
               }
            }
         }
      }

      clearPath();
   }

   if (goodIdea)
   {
      // the extra point is the first position in the path
      // this one is stored in memory and re-used in the next cycle
      mMemory->setExtraPosition(
         shortestExtraPath.first().x(),
         shortestExtraPath.first().y()
      );
   }
   else
   {
      mMemory->invalidateExtraPosition();
   }

   for (int i = 0; i < shortestExtraPath.size(); i++)
   {
      QPoint p = shortestExtraPath[i];

      // only multiply the score here; hazardous fields need to be
      // kept intact
      multiplyScore(
         p.x(),
         p.y(),
         mBotCharacter->getScoreForExtras()
      );
   }

   return goodIdea;
}


//-----------------------------------------------------------------------------
/*!
   \param path path to examine
   \return \c true if path looks dangerous
*/
bool ProtoBot::isPathHazardous(const QList<AStarNode*>& path) const
{
   bool dangerous = false;

   foreach (AStarNode* node, path)
   {
      if (getScore(node->getX(), node->getY()) < 0)
      {
         dangerous = true;
         break;
      }
   }

   return dangerous;
}


//-----------------------------------------------------------------------------
/*!
   \param path path to examine
   \return \c true if path looks dangerous
*/
int ProtoBot::getHazardousFieldCount(const QList<AStarNode*>& path) const
{
   int count = 0;

   foreach (AStarNode* node, path)
   {
      if (getScore(node->getX(), node->getY()) < 0)
      {
         count++;
      }
   }

   return count;
}


//-----------------------------------------------------------------------------
/*!
   \return list of living enemies
*/
QList<QPoint> ProtoBot::getLivingEnemyPositions() const
{
   QList<BotPlayerInfo *> enemies = getEnemies();

   QList<QPoint> enemyPositions;

   foreach (BotPlayerInfo* enemy, enemies)
   {
      if (!enemy->isKilled())
        enemyPositions << QPoint(floor(enemy->getX()), floor(enemy->getY()));
   }

   return enemyPositions;
}


//-----------------------------------------------------------------------------
/*!
   \return list of living enemies
*/
QList<QPoint> ProtoBot::getLivingEnemyFuturePositions() const
{
   QList<BotPlayerInfo *> enemies = getEnemies();
   QList<QPoint> enemyPositions;

   int x = 0;
   int y = 0;

   QPoint current;
   qint8 dirs = 0;

   foreach (BotPlayerInfo* enemy, enemies)
   {
      if (!enemy->isKilled())
      {
         // init
         x = 0;
         y = 0;

         // read enemy directions
         current = QPoint(floor(enemy->getX()), floor(enemy->getY()));
         dirs = enemy->getDirections();

         // apply enemy directions to current field
         if (enemy->getDeltaX() != 0.0f)
         {
            if (dirs & Constants::KeyLeft)
               x = -1;
            else if (dirs & Constants::KeyRight)
               x = 1;
         }

         if (enemy->getDeltaY() != 0.0f)
         {
            if (dirs & Constants::KeyUp)
               y = -1;
            else if (dirs & Constants::KeyDown)
               y = 1;
         }

         x += current.x();
         y += current.y();

         if (
               x >= 0 && x < mBotMap->getWidth()
            && y >= 0 && y < mBotMap->getHeight()
         )
         {
            QPoint future(x, y);
            enemyPositions << future;
         }
      }
   }

   return enemyPositions;
}


//-----------------------------------------------------------------------------
/*!
   \return list of enemies
*/
QList<BotPlayerInfo *> ProtoBot::getEnemies() const
{
   QList<BotPlayerInfo *> enemies;

   foreach (BotPlayerInfo* player, *mPlayerInfoMap)
   {
      if (player->getId() != mId)
         enemies << player;
   }

   return enemies;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if going to a bomb drop position looks suitable
*/
bool ProtoBot::updateBombStoneScore()
{
   if (isNoBombInfectionActive())
      return false;

   // init
   int pathFindingLoops1 = 0;
   int pathFindingLoops2 = 0;
   // int timePathFinding = 0;
   // QElapsedTimer measureTime;
   // measureTime.start();
   int* stonesToBeBombed = mBotMap->getStonesToBeBombedMap();
   bool foundSafeDropPos = false;
   QList<QPoint> reachablePoints = mBotMap->getReachablePositions();   
   QPoint previousBombStonePos = getBombStonePositionPrevious();

   // do the same optimization as for the extras.. check the bot's memory
   // for an existing bomb stone position. if that position is reachable
   // and has the same "bomb stone count" as it had when we stored it in
   // the bot's memory, there is absolutely no need to compute a new position.
   bool useMemoryPosition = false;
   if (mMemory->isBombStonePositionValid())
   {
      int score =
         mBotMap->getStoneCountAroundPoint(
            mMemory->getBombStonePositionX(),
            mMemory->getBombStonePositionY(),
            mPlayerInfo->getFlameCount()
         );

      foreach (const QPoint& p, reachablePoints)
      {
         if (
               p.x() == mMemory->getBombStonePositionX()
            && p.y() == mMemory->getBombStonePositionY()
            && score == mMemory->getBombStoneCount()
         )
         {
            // the bomb stone position stored in memory has still is still
            // reachable and has the same score as the bot remembered. there
            // does not seem to be any reason we shouldn't use it for all
            // further computations...
            useMemoryPosition = true;
         }
      }
   }

   // collect points to be weighted
   int weightedIndex = 0;
   QList<Weighted<QPoint, int> > weightedPoints;

   if (useMemoryPosition)
   {
      QPoint p(
         mMemory->getBombStonePositionX(),
         mMemory->getBombStonePositionY()
      );

      Weighted<QPoint, int> weighted(p, mMemory->getBombStoneCount());
      weightedPoints << weighted;
   }
   else
   {
      // this is the default way to compute bomb stone positions
      // i.e. they way positions are computed without using the bot's
      // memory
      foreach (const QPoint& p, reachablePoints)
      {
         int score =
            mBotMap->getStoneCountAroundPoint(
               p.x(),
               p.y(),
               mPlayerInfo->getFlameCount()
            );

         // "stones to be bombed" contains a value = -1 if the stone is going
         // to be bombed away by another bomb
         score += stonesToBeBombed[p.y() * mBotMap->getWidth() + p.x()];

         if (score > 0)
         {
            if (!evaluateDeadEndSituation(p.x(), p.y()))
               continue;

            /*

            // if score counts (score > 0), multiply it with number of extras in the stones
            score *= mBotMap->getExtraStoneCountAroundPoint(
                  p.x(),
                  p.y(),
                  mPlayerInfo->getFlameCount(),
                  getExtraShakeIds()
               );

            */

            // this is done in order to avoid "losing" the bomb stone position.
            // the reason for this is the fact positions may have the same score
            // but they're sorted differently depending on the bot's position. the
            // effect of this is, the bot runs to the one position, detects an equal
            // score at another position, runs into the other direction, detects an
            // equal score again.. and repeats that procedure.. all the time..
            // this can be avoided by giving the previous position a higher score.
            // this is only done when the previous position is still included in the
            // list of available positions.
            if (p == previousBombStonePos)
            {
               if (score > 0)
                  score++;
            }

            // store weighted point
            Weighted<QPoint, int> weighted(p, score);
            weightedPoints << weighted;
            weightedIndex++;
         }
      }
   }

   delete[] stonesToBeBombed;

   // now sort the weighted positions
   qSort(weightedPoints);

   // analyze current pos
   QPoint current = QPoint(getXField(), getYField());

   int currentScore =
      mBotMap->getStoneCountAroundPoint(
         current.x(),
         current.y(),
         mPlayerInfo->getFlameCount()
      );

   Weighted<QPoint, int> weightedCurrent(current, currentScore);

   if (!weightedPoints.isEmpty())
   {
      if (currentScore >=weightedPoints.at(0).getWeight())
      {
         weightedPoints.prepend(weightedCurrent);
      }
   }

   // find a place to hide from the bomb either by
   // going one field up or down or by going a field left or right
   //
   // start with the best point and go to the last
   int flameSize = mPlayerInfo->getFlameCount();

   QPoint weightedPoint;
   Weighted<QPoint, int> weighted;
   foreach(weighted, weightedPoints)
   {
      weightedPoint = weighted.getObject();

      // optimization:
      // check every bloody point in case we have less than 10 of them
      // or check all points with a manhattan length below 5
      int manhattanDist =
         Map::getManhattanLength(
            getXField(),
            getYField(),
            weightedPoint.x(),
            weightedPoint.y()
         );

      if (
            weightedPoints.size() < 10 // works for memorized position, too
         || manhattanDist <= 5
         || weightedPoint == previousBombStonePos
      )
      {
         // take ALL reachable positions and remove those that will be burned
         // by the bomb as soon it will have been placed. if there's just
         // ONE position left that we're able to find a path to, we're cool.
         QList<QPoint> reachablePositionsFiltered =
            reachablePositionsLeft(
               weightedPoint.x(),
               weightedPoint.y(),
               flameSize,
               reachablePoints
            );

         // sort this list of reachable points by starting at the detonation
         // point;
         // after doing that pick a limited number of points suited for
         // further inspection.
         QList< Weighted<QPoint, int> > reachablePositionsByManhattanLength;
         foreach (const QPoint& p, reachablePositionsFiltered)
         {
            int manhattan = -(p - weightedPoint).manhattanLength();
            reachablePositionsByManhattanLength << Weighted<QPoint, int>(p, manhattan);
         }
         qSort(reachablePositionsByManhattanLength);
         reachablePositionsFiltered.clear();

         int iterations = 0;
         int max = 30;
         Weighted<QPoint, int> w;
         foreach (w, reachablePositionsByManhattanLength)
         {
            reachablePositionsFiltered << w.getObject();

            if (iterations > max)
               break;

            iterations++;
         }

         // then find the way to the possibly shortest safe point.
         // this 1st path-finding has just one purpose which is to
         // ensure there is a way to escape from the bomb-drop-position
         // in spe to the safe position (safePoint)
         foreach (const QPoint& safePoint, reachablePositionsFiltered)
         {
            // score path to bomb drop position
            findPathFromTo(
               weightedPoint.x(),
               weightedPoint.y(),
               safePoint.x(),
               safePoint.y()
            );

            int pathLength = mPathFinding.getPathLength();
            pathFindingLoops1++;

            if (pathFindingLoops1 > 300)
            {
               qWarning("ProtoBot::updateBombStoneScore(): cpu usage exceeded");
            }

            // a safe position must not be our current position
            if (pathLength > 0)
            {
               bool safePath = !isPathHazardous(mPathFinding.getPath());
               clearPath();

               if (safePath)
               {
                  // optimization:
                  // if we're already on the field we want to bomb
                  // there's no need to to find a path
                  if (
                        weightedPoint.x() == getXField()
                     && weightedPoint.y() == getYField()
                  )
                  {
                     // assign bomb drop position
                     setBombStonePosition(weightedPoint);

                     foundSafeDropPos = true;
                  }
                  else
                  {
                     findPath(weightedPoint.x(), weightedPoint.y());

                     pathFindingLoops2++;

                     QList<AStarNode*> foundPath = mPathFinding.getPath();

                     if (!foundPath.isEmpty())
                     {
                        foreach (AStarNode* node, foundPath)
                        {
                           multiplyScore(
                              node->getX(),
                              node->getY(),
                              mBotCharacter->getScoreForPrepareBombDrop()
                           );
                        }

                        // assign bomb drop position
                        setBombStonePosition(weightedPoint);

                        // yup. we can drop a bomb
                        foundSafeDropPos = true;
                     }
                     else
                     {
                        qDebug(
                           "ProtoBot::updateBombStoneScore(): path finding failed "
                           "finding a path from (%d;%d) to (%d;%d)",
                           mXField,
                           mYField,
                           weightedPoint.x(),
                           weightedPoint.y()
                        );
                     }

                     clearPath();
                  }
               }
            }
            else
            {
               clearPath();
            }

            if (foundSafeDropPos)
               break;
         }

         if (foundSafeDropPos)
            break;
      }
   }

//   timePathFinding = measureTime.elapsed();
//
//   if (timePathFinding > 1000)
//   {
//      qWarning(
//         "bool ProtoBot::updateBombStoneScore(): cpu usage critical, %dms "
//         "loops1: %d, loops2: %d",
//         timePathFinding,
//         pathFindingLoops1,
//         pathFindingLoops2
//      );
//   }

   if (foundSafeDropPos)
   {
      int stoneCount =
         mBotMap->getStoneCountAroundPoint(
            getBombStonePosition().x(),
            getBombStonePosition().y(),
            mPlayerInfo->getFlameCount()
         );

      mMemory->setBombStonePosition(
         getBombStonePosition().x(),
         getBombStonePosition().y()
      );

      mMemory->setBombStoneCount(stoneCount);
   }
   else
   {
      mMemory->invalidateBombStonePosition();
   }

   return foundSafeDropPos;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if attack preparation sounds good
*/
bool ProtoBot::updateAttackScore()
{
   if (isNoBombInfectionActive())
      return false;

   bool attack = false;

   if (mBotMap->isBombAmountConsumed(mId, mPlayerInfo->getBombCount()))
   {
      // locate enemies
      mEnemyPositions = getLivingEnemyPositions();
      QList<QPoint> reachablePoints = mBotMap->getReachablePositions();
      QList<QPoint> reachableEnemies;

      foreach(const QPoint& reachable, reachablePoints)
      {
         if (mEnemyPositions.contains(reachable))
            reachableEnemies << reachable;
      }

      if (!reachableEnemies.isEmpty())
      {
         int shortestPathLength = INT_MAX;
         int pathLength = 0;

         // find shortest path to enemy
         QPoint enemyPosition;
         foreach (enemyPosition, reachableEnemies)
         {
            findPath(enemyPosition.x(), enemyPosition.y());

            pathLength = mPathFinding.getPathLength();

            // choose shortest path
            if (pathLength < shortestPathLength)
            {
               attack = true;

               shortestPathLength = pathLength;

               // make a copy of the computed path
               mBestAttackPath.clear();

               // if the path length is zero, we're most likely standing on our
               // enemy right now
               if (pathLength == 0)
               {
                  mBestAttackPath.append(QPoint(getXField(), getYField()));
               }
               else
               {
                  foreach (AStarNode* node, mPathFinding.getPath())
                     mBestAttackPath.prepend(QPoint(node->getX(), node->getY()));
               }

               // if only one field needs to be traversed this is most likely
               // the best path we'll get
               if (pathLength <= 1)
               {
                  clearPath();
                  break;
               }
            }

            clearPath();
         }

         if (!mBestAttackPath.isEmpty())
         {
            foreach(const QPoint& p, mBestAttackPath)
            {
               // as only the neighbored fields are examined later, increase
               // the score with each field is not required here. it's only
               // required the *current* field has a negative score and the
               // surrounding fields get more than that.
               // also do not alter the current field's score as it must be kept
               // at -1 to indicate it must be left!
               if (
                     p.x() != getXField()
                  || p.y() != getYField()
               )
               {
                  multiplyScore(p.x(), p.y(), mBotCharacter->getScoreForAttack());
               }
            }
         }
      }
   }

   return attack;
}


//-----------------------------------------------------------------------------
/*!

   if the neighbor field score is not any more interesting than
   the current one...
   and we're on our way to a bomb some stones away...
   but we're not yet on that bomb stone position's center
   though already on the bomb score position...
   then trick the code to keep on walking just a little more
   until we're really in the center of a field

   \return \c true if score should be overriden
*/
bool ProtoBot::overrideFieldScore()
{
   int xField = getXField();
   int yField = getYField();

   bool override = false;

   if (mScoringPrepareBombStone)
   {
      if (!mScoringBombStonePossible)
      {
         QPoint bombStonePosition = getBombStonePosition();

         if (
               bombStonePosition.x() == xField
            && bombStonePosition.y() == yField
         )
         {
            override = true;
         }
      }
   }

   return override;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::chooseNextField()
{
   int xField = getXField();
   int yField = getYField();

   // list reachable neighbor positions (up, down, left, right)
   // proper implementation: find shortest path to a save position!
   QList<QPoint> neighbors =
      mBotMap->getReachableNeighborPositions(
         xField,
         yField
      );

   // if neighbors count == 0 consider yourself dead
   if (neighbors.size() > 0)
   {
      // init maxScore with current field's score
      int maxScore = getScore(xField, yField);
      int fieldScore = -1;
      bool otherFieldScoreIsHigher = false;

      // calculate the maximum score and add all neighbors with the
      // maximum score to the bestpoints
      foreach (const QPoint& p, neighbors)
      {
         fieldScore = getScore(p.x(), p.y());

         if (fieldScore > maxScore)
         {
            otherFieldScoreIsHigher = true;
            maxScore = fieldScore;
         }
      }

      // not sure if the "bestpoints" part is really needed
      // its purpose is to "keep on track" if there are more than one
      // points with the best score => just walk into the same direction
      QList<QPoint> bestPoints;
      foreach (const QPoint& p, neighbors)
      {
         fieldScore = getScore(p.x(), p.y());

         if (fieldScore == maxScore)
            bestPoints << p;
      }

      if (!bestPoints.isEmpty())
      {
         if (bestPoints.contains(mLastTarget))
         {
            mTransiterateTargetX = mLastTarget.x();
            mTransiterateTargetY = mLastTarget.y();
         }
         else
         {
            mLastTarget = bestPoints.at(randomize(0, bestPoints.size()-1));
         }
      }

      /*
      qDebug(
         "CurrentHazardous: %d, PrepareAttack: %d, Escape: %d, Extra: %d, "
         "PrepareBombStone: %d, Bomb: %d, AttackPossible: %d, AtBombStonePosition: %d",
         mScoringCurrentHazardous,
         mScoringPrepareAttack,
         mScoringEscape,
         mScoringExtra,
         mScoringPrepareBombStone,
         mScoringBomb,
         mScoringAttackPossible,
         mScoringAtBombStonePosition
      );
      */

      if (!otherFieldScoreIsHigher)
      {
         otherFieldScoreIsHigher = overrideFieldScore();
      }

      if (otherFieldScoreIsHigher)
      {
         // only walk into a field that does NOT explode within the next second ;)
         // if it's 1000ms, we can enter it anyway, if our current remaining
         // field time is even worse.. there's nothing to lose :)
         int targetRemaining =
            getRemainingBombTime(
               mTransiterateTargetX,
               mTransiterateTargetY
            );

         int currentRemaining =
            getRemainingBombTime(
               mXField,
               mYField
            );

         if (
               targetRemaining > 1000
            || targetRemaining >= currentRemaining
         )
         {
            /*
            qDebug(
               "target: %d, current: %d",
               targetRemaining,
               currentRemaining
            );
            */

            // add walk action
            BotOption* option = new BotOption();
            option->setScore(1);
            option->setAction(getWalkActionInstance());
            mOptions.push_back(option);
         }
      }
   }

   // wait! maybe we can kick our way out :)
   else
   {
      int x = mXField;
      int y = mYField;

      if (isKickEscapePossible(x, y))
      {
         mTransiterateTargetX = x;
         mTransiterateTargetY = y;

         // add walk action
         BotOption* option = new BotOption();
         option->setScore(1);
         option->setAction(getWalkActionInstance());
         mOptions.push_back(option);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::think()
{
   Bot::think();

   // calc scores for all reachable fields
   scoreFields();

   chooseNextField();

   if (mOptions.isEmpty())
   {
      BotOption* option = new BotOption();
      option->setAction(new BotIdleAction());
      mOptions.push_back(option);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return new walk action
*/
BotWalkAction* ProtoBot::getWalkActionInstance()
{
   BotWalkAction* action = 0;

   qint8 keysPressed = computeWalkKeys();

   action = new BotWalkAction();
   action->setWalkKeys(keysPressed);   

//   if (isDebugWalkActionEnabled())
//   {
//      qDebug(
//         "ProtoBot::getWalkActionInstance(): x: %d; y: %d; (%f, %f); "
//         "tx:%d, ty: %d;",
//         getXField(),
//         getYField(),
//         mX,
//         mY,
//         mTransiterateTargetX,
//         mTransiterateTargetY
//      );
//   }
//
//   if (isDebugKeysPressedEnabled())
//   {
//      QString keysPressedString =
//         QString("keysPressed: %1|%2|%3|%4")
//            .arg( (keysPressed & Constants::KeyUp   ) ? "up"    : "--")
//            .arg( (keysPressed & Constants::KeyDown ) ? "down"  : "----")
//            .arg( (keysPressed & Constants::KeyLeft ) ? "left"  : "----")
//            .arg( (keysPressed & Constants::KeyRight) ? "right" : "----");
//
//      qDebug("%s", qPrintable(keysPressedString));
//   }

   // store latest walk information
   setBotKeysPressed(keysPressed);

   return action;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::clearPath()
{
   dynamic_cast<AStarMap*>(mBotMap)->clearNodes();
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::debugPathWithMap()
{
   dynamic_cast<AStarMap*>(mBotMap)->debugPath(mPathFinding.getPath());
}


//-----------------------------------------------------------------------------
/*!
   \param botmap botmap
*/
void ProtoBot::setBotMap(BotMap *botmap)
{
   Bot::setBotMap(botmap);

   // reinit fields
   int size = botmap->getWidth() * botmap->getHeight();
   mFieldScores = new int[size];
   mFieldBombTimes = new int[size];
   mHazardousTemorary = new int[size];

   mPathFinding.setMap(dynamic_cast<AStarMap*>(mBotMap));
}


//-----------------------------------------------------------------------------
/*!
   \param target x
   \param target y
*/
void ProtoBot::findPath(int targetX, int targetY)
{
   findPathFromTo(getXField(), getYField(), targetX, targetY);
}


//-----------------------------------------------------------------------------
/*!
   \param startX start x position
   \param startY start y position
   \param targetX target x position
   \param targetY target y position
*/
void ProtoBot::findPathFromTo(int startX, int startY, int targetX, int targetY)
{
   // init a star map
   dynamic_cast<AStarMap*>(mBotMap)->buildNodes();

   // init pathfinding
   mPathFinding.setStart(startX, startY);
   mPathFinding.setTarget(targetX, targetY);

   // find path
   mPathFinding.findPath();

   // debug map plus map with path
   if (mDebugPaths)
   {
      mPathFinding.debugPathShort();

      // mPathFinding.debugPath();
      // debugPathWithMap();
   }
}


//-----------------------------------------------------------------------------
/*!
  \param width map width
  \param height map height
  \return map
*/
BotMap* ProtoBot::createMap(int width, int height)
{
   return new AStarMap(width, height);
}


//-----------------------------------------------------------------------------
/*!
   \param map player info map
*/
void ProtoBot::setPlayerInfoMap(QMap<int, BotPlayerInfo *> *map)
{
   mPlayerInfoMap = map;
}


//-----------------------------------------------------------------------------
/*!
   \return insult message handler
*/
ProtoBotInsults *ProtoBot::getInsults()
{
   return mInsults;
}


//-----------------------------------------------------------------------------
/*!
   \return insult message handler
*/
const QList<int> &ProtoBot::getExtraShakeIds() const
{
   return mExtraShakeIds;
}


//-----------------------------------------------------------------------------
/*!
   \param x x pos
   \param y y pos
   \param flames no of flames
   \param reachablePositions reachable positions
   \return \c true if a bomb drop is safe
*/
QList<QPoint> ProtoBot::reachablePositionsLeft(
   int x,
   int y,
   int flames,
   const QList<QPoint> &reachablePositions
) const
{
   int dirX = 0;
   int dirY = 0;

   QList<QPoint> copy = reachablePositions;

   // filter those out that are hazardous
   foreach (const QPoint& p, reachablePositions)
   {
      if (getScore(p.x(), p.y()) < 0)
      {
         copy.removeOne(p);
      }
   }

   foreach (Constants::Direction dir, mBotMap->getDirectionsAndCurrent())
   {
      switch (dir)
      {
         case Constants::DirectionUp:
            dirX = 0;
            dirY = -1;
            break;
         case Constants::DirectionDown:
            dirX = 0;
            dirY = 1;
            break;
         case Constants::DirectionLeft:
            dirX = -1;
            dirY = 0;
            break;
         case Constants::DirectionRight:
            dirX = 1;
            dirY = 0;
            break;
         default:
            dirX = 0;
            dirY = 0;
            break;
      }

      // example:
      //    2 flames
      //    => check x+0, y
      //    => check x+1, y
      //    => check x+2, y
      for (int i = 1; i <= flames; i++)
      {
         int posX = x + i * dirX;
         int posY = y + i * dirY;

         copy.removeOne(QPoint(posX, posY));
      }
   }

   return copy;
}



//-----------------------------------------------------------------------------
/*!
   \return \c true if kick is a good idea
*/
bool ProtoBot::isKickPossible()
{
   bool kick = false;

   if (mPlayerInfo->isKickEnabled())
   {
      // int enemiesHitCount = 0;

      // ok, we can kick - now what?

      // there are mostly 2 criteria:
      //
      // 1) the bomb must be within a certain distance, let's say 4-5 fields
      // 2) we must be able to reach that bomb before it explodes
      // 3) the bomb must be reachable
      // 4) there must be a bomb which will hit an enemy
      // 5) if it hits and explodes at the enemy position, it should not kill us

      // FOR EACH BOMB

      // checking the criteria
      //
      // 1) calculating the manhattan length to each bomb should be a good filter
      //    to avoid looking at those bombs totally out of our reach

      // 2) when will it explode? will we make it there in time?

      // 3) compute a path to that bomb

      // 4) will it collide with an enemy or will it hit an enemy at its
      //    destination position? the more enemies are hit the better
      //    => enemiesHitCount++

      // 5) will it do no harm to us

      // the score should be the distance to the bomb multiplied with the
      // number of enemies hit

      // => kick the best option, set "kick" flag to true
   }

   return kick;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if infection is active
*/
bool ProtoBot::isNoBombInfectionActive() const
{
   bool active = false;

   if (mPlayerInfo->getDisease())
   {
      if (mPlayerInfo->getDisease()->getType() == Constants::SkullNoBomb)
         active = true;
   }

   return active;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if infection is active
*/
bool ProtoBot::isInfectionActive(Constants::SkullType skullType) const
{
   bool active = false;

   if (mPlayerInfo->getDisease())
   {
      if (mPlayerInfo->getDisease()->getType() == skullType)
         active = true;
   }

   return active;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug paths is active
*/
bool ProtoBot::isDebugPathsEnabled() const
{
   return mDebugPaths;
}


//-----------------------------------------------------------------------------
/*!
   \param debug if debug paths is active
*/
void ProtoBot::setDebugPathsEnabled(bool debug)
{
   mDebugPaths = debug;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug for escape paths is active
*/
bool ProtoBot::isDebugEscapePathsEnabled() const
{
   return mDebugEscapePaths;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug for escape paths enabled
*/
void ProtoBot::setDebugEscapePathsEnabled(bool enabled)
{
   mDebugEscapePaths = enabled;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugMapItemsEnabled(bool enabled)
{
   mDebugMapItems = enabled;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugMapItemsEnabled() const
{
   return mDebugMapItems;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugScoresEnabled() const
{
   return mDebugScores;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugScoresEnabled(bool value)
{
   mDebugScores = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugKeysPressedEnabled() const
{
   return mDebugKeysPressed;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugKeysPressedEnabled(bool value)
{
   mDebugKeysPressed = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugPossibleActionsEnabled() const
{
   return mDebugKeysPressed;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugPossibleActionsEnabled(bool value)
{
   mDebugPossibleActions = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugExecutedActionsEnabled() const
{
   return mDebugExecutedActions;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugExecutedActionsEnabled(bool value)
{
   mDebugExecutedActions = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugCurrentHazardousEnabled() const
{
   return mDebugCurrentHazardous;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugCurrentHazardousEnabled(bool value)
{
   mDebugCurrentHazardous = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugWalkActionEnabled() const
{
   return mDebugWalkAction;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugWalkActionEnabled(bool value)
{
   mDebugWalkAction = value;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugBombDropEnabled() const
{
   return mDebugBombDrop;
}


//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugBombDropEnabled(bool value)
{
   mDebugBombDrop = value;
}

//-----------------------------------------------------------------------------
/*!
   \param enabled debug flag is enabled
*/
void ProtoBot::setDebugBreakpointEnabled(bool enabled)
{
   mDebugBreakpoint = enabled;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true debug flag is active
*/
bool ProtoBot::isDebugBreakpointEnabled() const
{
   return mDebugBreakpoint;
}


//-----------------------------------------------------------------------------
/*!
   \param x x to go
   \param y y to go
   \return \c true if we can get outta here
*/
bool ProtoBot::isKickEscapePossible(int &x, int &y)
{
   bool escapePossible = false;

   if (mPlayerInfo->isKickEnabled())
   {
      // check for bombs on up, down, left and right
      if (
            (y - 2 >= 0)
         && mBotMap->getItem(x, y - 1)
         && mBotMap->getItem(x, y - 1)->getType() == MapItem::Bomb
         && mBotMap->getItem(x, y - 2) == 0
      )
      {
         escapePossible = true;
         y--;
      }

      else if (
            (y + 2 < mBotMap->getHeight())
         && mBotMap->getItem(x, y + 1)
         && mBotMap->getItem(x, y + 1)->getType() == MapItem::Bomb
         && mBotMap->getItem(x, y + 2) == 0
      )
      {
         escapePossible = true;
         y++;
      }

      else if (
            (x - 2 >= 0)
         && mBotMap->getItem(x - 1, y)
         && mBotMap->getItem(x - 1, y)->getType() == MapItem::Bomb
         && mBotMap->getItem(x - 2, y) == 0
      )
      {
         escapePossible = true;
         x--;
      }

      else if (
            (x + 2 < mBotMap->getWidth())
         && mBotMap->getItem(x + 1, y)
         && mBotMap->getItem(x + 1, y)->getType() == MapItem::Bomb
         && mBotMap->getItem(x + 2, y) == 0
      )
      {
         escapePossible = true;
         x++;
      }
   }

   return escapePossible;
}


//-----------------------------------------------------------------------------
/*!
   \param nextXField next field x position to choose
   \param nextYField next field y position to choose
   \return \c true if another field should be chosen
*/
bool ProtoBot::evaluateLongDistance(int& nextXField, int& nextYField) const
{   
   /*

      we get into this function when there's nothing to do within
      our current manhattan distance range (mostly a radius of 5, 6 fields
      around the player).

      so we can assume the player stands freely, its movement is only
      limited by blocks. therefore we just need to shift left or right,
      up or down if there's any obstacle.

      [X] [X]           [X]P[X]           [X] [X]
         P                                     P
      [X] [X]           [X] [X]           [X] [X]

      all movement      shift left        shift up
      possible          and right         and down
                        blocked           blocked


                                 hint: the walls do not matter :)

                                 XXXXXXX                    X
                                                            X
                                 [X]P[X]           [X] [X]  X
                                                        P   X
                                 [X] [X]           [X] [X]  X
                                                            X
   */

   bool movePossible = false;

   // code only applies for large maps
   //
   // .. actually... why should it!?
   // if (mBotMap->getWidth() > 13)
   {
      /*

         manhattan distance from current point (0)

                     5
                   5 4 5
                 5 4 3 4 5
               5 4 3 2 3 4 5
             5 4 3 2 1 2 3 4 5
           5 4 3 2 1 0 1 2 3 4 5
             5 4 3 2 1 2 3 4 5
               5 4 3 2 3 4 5
                 5 4 3 4 5
                   5 4 5
                     5
      */

      // analyze the manhattan range used by the short distance functions
      // to locate all items that are NOT block items.. in case we find
      // one single iteam within the short distance range, there's no point
      // to continue with the long distance evaluations.
      int sX = mXField;
      int sY = mYField;

      int ml = 5;
      int diameter = (ml * 2) + 1;

      int fields = 1;
      int xOffset = 0;
      int yOffset = -ml;
      int testX = 0;
      int testY = 0;

      MapItem* item = 0;
      MapItem::ItemType type = MapItem::Unknown;

      for (int y = 0; y < diameter; y++)
      {
         testY = sY + yOffset;

         for (int x = 0; x < fields; x++)
         {
            testX = sX + x + xOffset;

            // check 1 point for the presence of anything interesting
            if (
                  testX >= 0 && testX < mBotMap->getWidth()
               && testY >= 0 && testY < mBotMap->getHeight()
            )
            {
               item = mBotMap->getItem(testX, testY);

               if (item)
               {
                  type = item->getType();

                  // a nice clean ragequit
                  // => there IS actually something to do within our
                  //    given manhattan distance.. so hold yer horses.
                  if (type != MapItem::Block)
                     return false;
               }
            }
         }

         // next line
         yOffset++;

         // increase width by two fields until center is reached.
         // also move to the left until the center is reached
         // then move to the right again.
         if (yOffset <= 0)
         {
            xOffset--;
            fields += 2;
         }
         else
         {
            xOffset++;
            fields -= 2;
         }
      }

      // if there's an enemy within short distance range, there's also no
      // point to evaluate any further than here. if there's an enemy in
      // short distance and we do not plan to attack him, we might have
      // good reasons to do so => abort.
      QList<QPoint> enemyPositions = getLivingEnemyPositions();
      foreach (const QPoint& p, enemyPositions)
      {
         if (
             BotMap::getManhattanLength(
               p.x(),
               p.y(),
               mXField,
               mYField
            ) < MANHATTAN_LENGTH_MAX_ATTACK
         )
         {
            // another ragequit (see reason above)
            return false;
         }
      }

      // start the search for a little action on the field
      int targetX = -1;
      int targetY = -1;

      // find extras located somewhere
      if (targetX < 0)
      {
         for (int y = 0; y < mBotMap->getHeight(); y++)
         {
            for (int x = 0; x < mBotMap->getWidth(); x++)
            {
               if (
                     mBotMap->getItem(x, y)
                  && mBotMap->getItem(x, y)->getType() == MapItem::Extra
               )
               {
                  targetX = x;
                  targetY = y;
                  break;
               }
            }
         }
      }

      // find stones located somewhere
      if (targetX < 0)
      {
         for (int y = 0; y < mBotMap->getHeight(); y++)
         {
            for (int x = 0; x < mBotMap->getWidth(); x++)
            {
               if (
                     mBotMap->getItem(x, y)
                  && mBotMap->getItem(x, y)->getType() == MapItem::Stone
               )
               {
                  targetX = x;
                  targetY = y;
                  break;
               }
            }
         }
      }

      // find stones located somewhere
      if (targetX < 0)
      {
         if (enemyPositions.size() > 0)
         {
            targetX = enemyPositions.at(0).x();
            targetY = enemyPositions.at(0).y();
         }
      }

      if (targetX >= 0)
      {
         movePossible = true;

         bool up = false;
         bool down = false;
         bool left = false;
         bool right = false;

         up    = (targetY < mYField);
         down  = (targetY > mYField);
         left  = (targetX < mXField);
         right = (targetX > mXField);

         // if we cannot go up or down
         // => shift to the side
         if (up && (nextYField - 1 >= 0))
         {
            if (mBotMap->isPositionBlocked(nextXField, nextYField - 1))
            {
               if (right)
                  nextXField++;
               else
                  nextXField--;
            }
            else
            {
               nextYField--;
            }
         }
         else if (down && (nextYField + 1 < mBotMap->getHeight()))
         {
            if (mBotMap->isPositionBlocked(nextXField, nextYField + 1))
            {
               if (right)
                  nextXField++;
               else
                  nextXField--;
            }
            else
            {
               nextYField++;
            }
         }

         // if we cannot go left or right
         // => shift up or down
         else if (left && (nextXField - 1 >= 0))
         {
            if (mBotMap->isPositionBlocked(nextXField - 1, nextYField))
            {
               if (down)
                  nextYField++;
               else
                  nextYField--;
            }
            else
            {
               nextXField--;
            }
         }
         else if (right && (nextXField + 1 < mBotMap->getWidth()))
         {
            if (mBotMap->isPositionBlocked(nextXField + 1, nextYField))
            {
               if (down)
                  nextYField++;
               else
                  nextYField--;
            }
            else
            {
               nextXField++;
            }
         }
      }
   }

   return movePossible;
}


//-----------------------------------------------------------------------------
/*!
   \param startX kick start x position
   \param startY kick start y position
   \param direction kick direction
   \param flames no of flames
*/
void ProtoBot::bombKicked(
   int startX,
   int startY,
   Constants::Direction direction,
   int flames
)
{
   bool hitSomething = false;
   int xi = startX;
   int yi = startY;
   int xiPrev = 0;
   int yiPrev = 0;
   int i = 1;
   MapItem* item = 0;

   // compute kick end position
   while (!hitSomething)
   {
      xiPrev = xi;
      yiPrev = yi;

      switch (direction)
      {
         case Constants::DirectionUp:
         {
            yi = startY - i;
            break;
         }

         case Constants::DirectionDown:
         {
            yi = startY + i;
            break;
         }

         case Constants::DirectionLeft:
         {
            xi = startX - i;
            break;
         }

         case Constants::DirectionRight:
         {
            xi = startX + i;
            break;
         }

         default:
         {
            break;
         }
      }

      if (
            xi >= 0 && xi < mBotMap->getWidth()
         && yi >= 0 && yi < mBotMap->getHeight()
      )
      {
         item = mBotMap->getItem(xi, yi);

         if (item && item->isBlocking())
         {
            hitSomething = true;
         }
         else
         {
            foreach (BotPlayerInfo* player, *mPlayerInfoMap)
            {
               if (!player->isKilled())
               {
                  int px = (int)floor(player->getX());
                  int py = (int)floor(player->getY());

                  if (px == xi && py == yi)
                  {
                     hitSomething = true;
                  }
               }
            }
         }
      }
      else
      {
         hitSomething = true;
      }

      i++;
   }

   if (
         xiPrev >= 0 && xiPrev < mBotMap->getWidth()
      && yiPrev >= 0 && yiPrev < mBotMap->getHeight()
   )
   {
      // qDebug("mark %d, %d with %d flames hazardous", xiPrev, yiPrev, flames);
      markHazardousTemporary(xiPrev, yiPrev, 1500, flames);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if a better field has been found
*/
bool ProtoBot::updateLeastHazardousField()
{
   bool foundSomething = false;

   // all reachable fields must suck
   QList<QPoint> reachable = mBotMap->getReachablePositions();

   // the number of reachable fields must be pretty limited
   int reachableCount = reachable.size();
   if (
         reachableCount <= 4
      && reachableCount >= 2
   )
   {
      bool proceed = true;
      foreach (const QPoint& p, reachable)
      {
         if (getScore( p.x(), p.y()) >= 0)
         {
            proceed = false;
            break;
         }
      }

      // if they all suck, choose the one that sucks least
      if (proceed)
      {
         QPoint best;
         int remaining = 0;
         int remainingTemp = 0;

         foreach (const QPoint& p, reachable)
         {
            remainingTemp = getRemainingBombTime(p.x(), p.y());

            if (remainingTemp > remaining)
            {
               best = p;
               remaining = remainingTemp;
            }
         }

         // find a path to the best field if it differs from our current field
         if (
               best.x() != mXField
            || best.y() != mYField
         )
         {
            if (remaining > 0)
            {
               findPath(best.x(), best.y());

               QList<AStarNode*> path = mPathFinding.getPath();

               if (path.length() > 0)
               {
                  foundSomething = true;

                  foreach (AStarNode* node, path)
                  {
                     if (
                           node->getX() != mXField
                        && node->getY() != mYField)
                     {
                        // make the bot choose the better field
                        setScore(node->getX(), node->getY(), 2);
                     }
                  }
               }

               clearPath();
            }
         }
      }
   }

   return foundSomething;
}


//-----------------------------------------------------------------------------
/*!
   \param start analysis at x
   \param start analysis at y
   \param recursionDepth current recursion depth
   \param end dead end's end point
   \param direction recursion direction
   \return list of points belonging to a "dead end"
*/
QList<QPoint> ProtoBot::analyzeDeadEnd(
   int x,
   int y,
   QPoint& end,
   int recursionDepth,
   const QPoint& direction
) const
{
   recursionDepth++;

   QList<QPoint> points;
   int reachableNeighbours = 4;
   QPoint openingDir1;
   QPoint openingDir2;
   MapItem* item = 0;
   int xi = 0;
   int yi = 0;

   /*

      default dead end situations

         [X][X]   [X][X]
         [X]         [X]
         [X][X]   [X][X]

         [X][X][X]
         [X]   [X]

         [X]   [X]
         [X][X][X]


      2 field dead ends

         [X][X][X]   [X][X][X]
         [X]               [X]
         [X][X][X]   [X][X][X]

         [X][X][X]
         [X]   [X]
         [X]   [X]

         [X]   [X]
         [X]   [X]
         [X][X][X]


      and so on...

   */

   // examine current position
   foreach (const QPoint& dir, mDirections)
   {
      xi = x + dir.x();
      yi = y + dir.y();

      if (
            xi >= 0 && xi < mBotMap->getWidth()
         && yi >= 0 && yi < mBotMap->getHeight()
      )
      {
         item = mBotMap->getItem(xi, yi);

         if (
               item
            && item->isBlocking()
         )
         {
            reachableNeighbours--;
         }
         else
         {
            // remember opening directions if field can be accessed
            if (openingDir1.isNull())
               openingDir1 = dir;
            else
               openingDir2 = dir;
         }
      }
      else
      {
         reachableNeighbours--;
      }
   }

   // there's just one opening
   if (reachableNeighbours == 1)
   {
      // we found the dead end's end :)
      // this end position is stored at the very end of the function
      // so it'll appear at the end of the list
      if (end.isNull())
      {
         end.setX(x);
         end.setY(y);
      }

      // dig deeper
      if (recursionDepth == 1)
      {
         // go into opposite direction
         points << analyzeDeadEnd(
                  x + openingDir1.x(),
                  y + openingDir1.y(),
                  end,
                  recursionDepth,
                  openingDir1
               );
      }
   }

   // there are two openings
   else if (reachableNeighbours == 2)
   {
      if (recursionDepth == 1)
      {
         // both direction must be the opposite of each other
         if (-openingDir1 == openingDir2)
         {
            // first go
            points << analyzeDeadEnd(
                     x + openingDir1.x(),
                     y + openingDir1.y(),
                     end,
                     recursionDepth,
                     openingDir1
                  );

            points << analyzeDeadEnd(
                     x + openingDir2.x(),
                     y + openingDir2.y(),
                     end,
                     recursionDepth,
                     openingDir2
                  );
         }
      }
      else
      {
         bool pointingIntoSameDir = false;
         bool pointingBack = false;

         // one must be the same direction we came from
         pointingIntoSameDir = (
               openingDir1 == direction
            || openingDir2 == direction
         );

         // one opening must be pointing into the direction we came from
         if (pointingIntoSameDir)
         {
            pointingBack = (
               (
                     -openingDir1.x() == direction.x()
                  && -openingDir1.y() == direction.y()
               )
               ||
               (
                     -openingDir2.x() == direction.x()
                  && -openingDir2.y() == direction.y()
               )
            );
         }

         // if that is the case, we can continue recursion into the direction
         if (pointingIntoSameDir && pointingBack)
         {
            points << QPoint(x, y);
            points << analyzeDeadEnd(
               x + direction.x(),
               y + direction.y(),
               end,
               recursionDepth,
               direction
            );
         }
      }
   }

   // we're done
   if (recursionDepth == 1 && !points.isEmpty())
   {
      // this is no dead end
      if (end.isNull())
      {
         points.clear();
      }
      else
      {
         // "end" goes to front
         points.push_front(end);

         int m = points.size();
         if (points.size() > 1)
         {
            // add dead end's opening
            QPoint other = points.at(1);

            openingDir1.setX(
               (other.x() > end.x())
                  ? 1
                  : (other.x() < end.x())
                    ? -1
                    : 0
            );

            openingDir1.setY(
               (other.y() > end.y())
                  ? 1
                  : (other.y() < end.y())
                    ? -1
                    : 0
            );
         }

         // "opening" goes to back
         points.push_back(
            QPoint(
               end.x() + m * openingDir1.x(),
               end.y() + m * openingDir1.y()
            )
         );
      }
   }

   return points;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position to check
   \param y y position to check
*/
bool ProtoBot::evaluateDeadEndSituation(int x, int y)
{
   bool safeToGoThere = true;

   QPoint end;
   QList<QPoint> points = analyzeDeadEnd(x, y, end);

   // the position is actually located within a dead end
   if (!points.isEmpty())
   {
      // check if there's an enemy in the area around opening
      QPoint opening = points.last();
      QList<BotPlayerInfo *> enemies = getEnemies();
      foreach (BotPlayerInfo* enemy, enemies)
      {
         if (!enemy->isKilled())
         {
            QPoint e(
               floor(enemy->getX()),
               floor(enemy->getY())
            );

            // yup. an enemy is around.
            if ((e - opening).manhattanLength() < 2)
            {
               safeToGoThere = false;
               break;
            }
         }
      }
   }

   return safeToGoThere;
}


//-----------------------------------------------------------------------------
/*!
*/
void ProtoBot::markHazardousDeadEnds()
{
   int x = getXField();
   int y = getYField();

   bool markHazardous = false;

   QPoint end;
   QList<QPoint> points = analyzeDeadEnd(x, y, end);

   // the position is actually located within a dead end
   if (!points.isEmpty())
   {
      // check if there's an enemy in the area around opening
      QPoint opening = points.last();
      QList<BotPlayerInfo *> enemies = getEnemies();

      // the dead end is small (just 1 or 2 fields)
      if (points.size() <= 3)
      {
         foreach (BotPlayerInfo* enemy, enemies)
         {
            if (!enemy->isKilled())
            {
               QPoint e(
                  floor(enemy->getX()),
                  floor(enemy->getY())
               );

               // yup. an enemy is around.
               if ((e - opening).manhattanLength() < 2)
               {
                  markHazardous = true;
               }
            }
         }
      }
   }

   if (markHazardous)
   {
      // we skip the opening
      for (int i = 0; i < points.size() - 1; i++)
      {
         setScore(
            points[i].x(),
            points[i].y(),
            -1
         );
      }
   }
}

