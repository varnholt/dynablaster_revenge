#ifndef PROTOBOT_H
#define PROTOBOT_H

// base
#include "bot.h"

// shared
#include "constants.h"

// astar
#include "astarpathfinding.h"

// Qt
#include <QPoint>

// forward declarations
class BombChainReaction;
class BotCharacter;
class BotWalkAction;
class ProtoBotInsults;
class ProtoBotMemory;

class ProtoBot : public Bot
{
   public:

      //! constructor
      ProtoBot();

      //! destructor
      virtual ~ProtoBot();

      //! debug bot's path in map
      void debugPathWithMap();

      //! setter for botmap
      virtual void setBotMap(BotMap* botmap);

      //! create a map
      virtual BotMap* createMap(int width, int height);

      //! setter for player info map
      void setPlayerInfoMap(QMap<int, BotPlayerInfo *> *map);

      //! getter for protobot insult message handler
      ProtoBotInsults* getInsults();

      //! getter for extra shake ids
      const QList<int>& getExtraShakeIds() const;

      //! getter for debug paths flag
      bool isDebugPathsEnabled() const;

      //! setter for debug paths flag
      void setDebugPathsEnabled(bool debug);

      //! getter for debug escape paths flag
      bool isDebugEscapePathsEnabled() const;

      //! setter for debug escape paths flag
      void setDebugEscapePathsEnabled(bool value);

      //! getter for debug mapitems flag
      bool isDebugMapItemsEnabled() const;

      //! setter for debug mapitems flag
      void setDebugMapItemsEnabled(bool value);

      //! getter for debug flag
      bool isDebugScoresEnabled() const;

      //! setter for debug flag
      void setDebugScoresEnabled(bool value);

      //! getter for debug flag
      bool isDebugKeysPressedEnabled() const;

      //! setter for debug flag
      void setDebugKeysPressedEnabled(bool value);

      //! getter for debug flag
      bool isDebugPossibleActionsEnabled() const;

      //! setter for debug flag
      void setDebugPossibleActionsEnabled(bool value);

      //! getter for debug flag
      bool isDebugExecutedActionsEnabled() const;

      //! setter for debug flag
      void setDebugExecutedActionsEnabled(bool value);

      //! getter for debug flag
      bool isDebugCurrentHazardousEnabled() const;

      //! setter for debug flag
      void setDebugCurrentHazardousEnabled(bool value);

      //! getter for debug flag
      bool isDebugWalkActionEnabled() const;

      //! setter for debug flag
      void setDebugWalkActionEnabled(bool value);

      //! getter for debug flag
      bool isDebugBombDropEnabled() const;

      //! setter for debug flag
      void setDebugBombDropEnabled(bool value);

      //! setter for debug breakpoint flag
      void setDebugBreakpointEnabled(bool enabled);

      //! getter for debug breakpoint flag
      bool isDebugBreakpointEnabled() const;

      //! check if we can kick ourselves out of here
      bool isKickEscapePossible(int& x, int& y);

      //! getter for idle counter
      int getIdleCounter() const;

      //! setter for idle counter
      void setIdleCounter(int value);

      //! reset idle counter
      void resetIdleCounter();

      //! increase idle counter
      void increaseIdleCounter();


   public slots:

      //! setter for current position (overwritten)
      virtual void updatePlayerPosition(int id, float x, float y, float ang);

      //! overwrite extra shake handler
      virtual void extraShake(int itemId);

      //! wake up bot
      virtual void wakeUp();

      //! setter for hazardous temp milliseconds
      virtual void markHazardousTemporary(int x, int y, int ms, int fieldCount = 0);

      //! mark dead end hazardous in case we need to leave NOW
      virtual void markHazardousDeadEnds();

      //! make hazardous temp for bomb kicks
      virtual void bombKicked(
         int startX,
         int startY,
         Constants::Direction,
         int flames
      );


   protected:

      // inherited from bot

      //! compute new options
      virtual void think();

      //! base overwritten
      virtual void decide();

      //! base overwritten
      virtual void act();

      //! reset bot
      virtual void reset();

      //! cleanup stuff after destructor call
      virtual void cleanUpBot();


      // custom stuff

      //! random
      int randomize(int min, int max);

      //! find path to target
      void findPath(int targetX, int targetY);

      //! find path to target
      void findPathFromTo(int startX, int startY, int targetX, int targetY);

      //! clear path when done
      void clearPath();

      //! calculate score for reachable positions
      void scoreFields();

      //! score for kick
      bool isKickPossible();

      //! score for escape path
      bool updateEscapeScore();

      //! score for extras
      bool updateExtraScore();

      //! score for bomb dropping
      bool updateBombStoneScore();

      //! score for attack
      bool updateAttackScore();

      //! choose the next field to go to
      void chooseNextField();

      //! setter for remaining bomb time
      void setRemainingBombTime(int x, int y, int time);

      //! getter for remaining bomb time
      int getRemainingBombTime(int x, int y) const;

      //! setter for score at x,y
      void setScore(int x, int y, int score);

      //! multiply score by factor
      void multiplyScore(int x, int y, int factor);

      //! get score for field at x,y
      int getScore(int x, int y) const;

      //! getter for hazardous temp milliseconds
      int getHazardousTemporary(int x, int y) const;

      //! update hazardous temporary array
      void updateHazardousTemporary(int ms);

      //! reset hazardous temporary array
      void resetHazardousTemporary();

      //! check if a path is dangerous
      bool isPathHazardous(const QList<AStarNode*>& path) const;

      //! get the number of hazardous fields per path
      int getHazardousFieldCount(const QList<AStarNode*>& path) const;

      //! player is currently unable to drop bombs
      bool isNoBombInfectionActive() const;

      //! check if player infection is active
      bool isInfectionActive(Constants::SkullType skullType) const;

      //! get a list of enemy positions
      QList<QPoint> getLivingEnemyPositions() const;

      //! get a list of future enemy positions
      QList<QPoint> getLivingEnemyFuturePositions() const;

      //! get a list of enemies
      QList<BotPlayerInfo *> getEnemies() const;

      //! check if a bomb drop is safe for ourself
      QList<QPoint> reachablePositionsLeft(
         int x,
         int y,
         int flames,
         const QList<QPoint>& reachablePositions
      ) const;

      // transiteration functionality

      //! create walk direction from next field
      BotWalkAction* getWalkActionInstance();

      //! debug output scores
      void debugScores();

      //! check if attack is now possible
      bool isAttackPossible();

      //! check if bomb position reached
      bool isBombStonePossible();

      //! reset scores
      void resetScores();

      //! mark hazardous fields
      void markHazardousFields();

      //! mark reachable fields
      void markReachableFields();

      //! update remaining bomb times
      void updateRemainingBombTimes();

      //! pick next field to go to
      // void pickNextField();

      //! transiteration busy flag
      // bool mTransiterateBusy;

      //! setter for bomb stone position
      void setBombStonePosition(const QPoint &weightedPoint);

      //! getter for bomb stone position
      QPoint getBombStonePosition() const;

      //! setter for bomb stone position
      void setBombStonePositionPrevious(const QPoint &previous);

      //! getter for bomb stone position
      QPoint getBombStonePositionPrevious() const;

      //! reset bomb stone position
      void resetBombStonePosition();

      //! reset score flags
      void resetScoringFlags();

      //! evaluate long distance
      bool evaluateLongDistance(int &nextXField, int &nextYField) const;

      //! check if a safe escape is possible from this point
      bool isSafeEscapePossible();

      //! mark the best field in case we're in a fucked up situation
      bool updateLeastHazardousField();

      //! check if a field is a dead end
      QList<QPoint> analyzeDeadEnd(
         int x,
         int y,
         QPoint& end,
         int recursionDepth = 0,
         const QPoint& direction = QPoint()
      ) const;

      //! evaluate dead end situation
      bool evaluateDeadEndSituation(int x, int y);

      //! reset field bomb times
      void resetFieldBombTimes();

      //! check if field score should be overriden
      bool overrideFieldScore();


      // bug spotting

      //! track bug issue 1
      void bugTrack1();

      //! track bug issue 2
      void bugTrack2();

      //! track bug issue 3
      void bugTrack3();


      //! a star path finding
      AStarPathFinding mPathFinding;

      //! best path found
      QList<QPoint> mBestEscapePath;

      //! best attack path
      QList<QPoint> mBestAttackPath;

      //! score field
      int* mFieldScores; // [13 * 11];

      //! remaining bomb times
      int* mFieldBombTimes;

      //! temporary hazardous
      int* mHazardousTemorary;

      //! bomb drop position
      QPoint mBombStonePosition;

      //! previous bomb drop position
      QPoint mBombStonePositionPrevious;

      //! attack bomb drop position
      // QPoint mAttackBombDropPosition;

      //! list of possible attack bomb drop positions
      // QList<QPoint> mAttackBombDropPositions;

      //! every bot has a character that defines the action's scores
      BotCharacter* mBotCharacter;

      //! player info map
      QMap<int, BotPlayerInfo *>* mPlayerInfoMap;

      //! protobot insults
      ProtoBotInsults* mInsults;

      //! list of enemy positions (updated by scoreForAttack)
      QList<QPoint> mEnemyPositions;

      //! list of shake extras
      QList<int> mExtraShakeIds;

      //! direction vectors
      QList<QPoint> mDirections;

      //! last target position
      QPoint mLastTarget;

      //! bomb chain reaction evaluation
      BombChainReaction* mBombChainReaction;

      //! bot has a memory to remember good positions and such
      ProtoBotMemory* mMemory;

      //! idle counter
      int mIdle;

      QQueue<QPoint> mLastPositions;


      // scoring
      bool mScoringCurrentHazardous;
      bool mScoringPrepareAttack;
      bool mScoringEscape;
      bool mScoringExtra;
      bool mScoringPrepareBombStone;
      bool mScoringBomb;
      bool mScoringAttackPossible;
      bool mScoringBombStonePossible;

      // debugging
      bool mDebugBreakpoint;
      bool mDebugPaths;
      bool mDebugEscapePaths;
      bool mDebugMapItems;
      bool mDebugScores;
      bool mDebugKeysPressed;
      bool mDebugPossibleActions;
      bool mDebugExecutedActions;
      bool mDebugCurrentHazardous;
      bool mDebugWalkAction;
      bool mDebugBombDrop;
};

#endif // PROTOBOT_H
