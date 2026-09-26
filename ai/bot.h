#ifndef BOT_H
#define BOT_H

// Qt
#include <QObject>
#include <QPoint>
#include <QQueue>
#include <QTimer>

// ai
#include "botoption.h"

// shared
#include "constants.h"
#include "serverconfiguration.h"
#include "signal.h"

// forward declarations
class BotMap;
class BotPlayerInfo;

class Bot : public QObject
{
   Q_OBJECT

public:
   //! bot state
   enum BotState
   {
      BotStateIdle,
      BotStateActive,
      BotStateDead
   };

   //! constructor
   Bot(QObject* parent = 0);

   //! destructor
   virtual ~Bot();

   //! setter for bot map
   virtual void setBotMap(BotMap* botmap);

   //! setter for player info ptr
   void setPlayerInfo(BotPlayerInfo* info);

   //! getter for player info ptr
   BotPlayerInfo* getPlayerInfo() const;

   //! getter for current position x
   float getX() const;

   //! getter for current position y
   float getY() const;

   //! getter for field x
   int getXField();

   //! getter for field y
   int getYField();

   //! create a map
   virtual BotMap* createMap(int width, int height);

   //! setter for server configuration
   void setServerConfiguration(const ServerConfiguration&);

   //! getter for server configuration
   const ServerConfiguration& getServerConfiguration() const;

   //! start ticking the bot's think/decide/act loop, once per game round
   void startTicking();

   //! setter for bot keys pressed
   void setBotKeysPressed(int8_t keysPressed);

   //! getter for bot keys pressed
   int8_t getBotKeysPressed() const;

   //! compute walk keys
   int8_t computeWalkKeys() const;

   // Signal<> replacements for Bot's former Qt signals - the only two connect() sites for
   // these (ai/botfactory.cpp) always target a BotClient instance, never BombermanClient (see
   // project_full_qt_removal_scope memory - a previous session's claim that this reached into
   // BombermanClient's own signal surface was wrong, it doesn't).

   //! drop bomb
   Signal<> bombSignal;

   //! send walk keys
   Signal<int8_t> walkSignal;

   //! time to sync
   Signal<> syncSignal;

public slots:

   //! setter for current position
   virtual void updatePlayerPosition(int id, float x, float y, float ang);

   //! setter for player id
   void updatePlayerId(int id);

   //! wake up bot
   virtual void wakeUp();

   //! go to idle
   virtual void idle();

   //! die
   virtual void die();

   //! extra shake packet spotted
   virtual void extraShake(int);

   //! mark hazardous temporary
   virtual void markHazardousTemporary(int x, int y, int ms, int fieldCount = 0);

   //! make hazardous temp for bomb kicks
   virtual void bombKicked(int startX, int startY, Constants::Direction, int flames);

protected:
   // bot base functionality

   //! compute new options
   virtual void think();

   //! decide what's to do next
   virtual void decide();

   //! do some action
   virtual void act();

   //! check if an action is required
   virtual bool isActionRequired();

   //! reset bot states
   virtual void reset();

   //! delete stuff in destructor
   virtual void cleanUpBot();

   //! think/decide/act once, called every mTickTimer interval while active
   virtual void tick();

   // game state transitions

   //! check if bot is active
   bool isActive();

   //! setter for bot's state
   void setState(BotState state);

   // navigation

   //! check if field is already reached
   bool isFieldReached();

   // bot information

   //! invalidate
   void invalidate();

   //! check if player position is valid
   bool isValid() const;

   //! setter for valid flag
   void setPlayerPositionValid(bool valid);

   //! getter for valid flag
   bool isPlayerPositionValid() const;

   //! update position queue
   void updatePositionQueue();

   //! check if player's brain seems to be fused
   bool isPositionQueueRecurrent() const;

   // members

   //! bot state
   BotState mBotState;

   //! bot map
   BotMap* mBotMap;

   //! player info ptr
   BotPlayerInfo* mPlayerInfo;

   //! bot's options
   QList<BotOption*> mOptions;

   //! bot's next action
   QList<BotAction*> mActions;

   //! x position
   float mX;

   //! y position
   float mY;

   //! x field
   int mXField;

   //! y field
   int mYField;

   //! bot id
   int mId;

   //! ticks tick() at the same ~100ms cadence the old QThread loop's msleep(100) had
   QTimer* mTickTimer;

   //! current walk direction
   int mBotKeysPressed;

   //! decision required flag
   bool mDecisionRequired;

   //! action required flag
   bool mActionRequired;

   // navigation

   //! transiteration target x
   int mTransiterateTargetX;

   //! transiteration target y
   int mTransiterateTargetY;

   //! player position is valid
   bool mPlayerPositionValid;

   //! store last few player positions
   QQueue<QPoint> mPositionQueue;

   // server related

   //! server configuration
   ServerConfiguration mServerConfiguration;
};

#endif  // BOT_H
