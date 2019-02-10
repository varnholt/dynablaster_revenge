#ifndef GAME_H
#define GAME_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QTimer>

// shared
#include "constants.h"
#include "creategamedata.h"
#include "gameinformation.h"
#include "gameround.h"
#include "packet.h"

// forward declarations
class BombMapItem;
class CollisionDetection;
class ExtraMapItem;
class ExtraShakePacketHandler;
class ExtraSpawn;
class Map;
class MapItem;
class MapItemCreatedPacket;
class Player;
class PlayerDisease;
class QTcpSocket;

class Game : public QObject
{

   Q_OBJECT

   public:

      //! constructor
      Game();

      //! destructor
      ~Game();

      //! initialize everything
      void initialize();

      //! getter for the game's id
      int getId() const;

      //! getter for the player count
      int getPlayerCount() const;

      //! getter for the maximum player count
      int getMaximumPlayerCount() const;

      //! setter for create game data
      void setCreateGameData(const CreateGameData& data);

      //! setter for game name
      void setName(const QString& name);

      //! getter for the game's name
      const QString& getName() const;

      //! getter for the level's name
      const QString& getLevelName() const;

      //! process a packet
      void processPacket(
         QTcpSocket* tcpSocket,
         Packet* packet
      );

      //! getter for map socket <-> player
      QMap<QTcpSocket*, Player*>* getPlayerSockets();

      //! setter for the game's creator
      void setCreator(Player*);

      //! getter for the game's creator
      Player* getCreator() const;

      //! getter for list of players
      QList<Player*> getPlayers() const;

      //! setter for the game's duration
      void setDuration(int);

      //! getter for the game's map
      Map* getMap() const;

      //! getter for map dimension
      Constants::Dimension getMapDimension() const;

      //! get combined extra enum
      int getExtras() const;

      //! getter for game duration
      int getDuration() const;

      //! broadcast a message to all players in the game
      void broadcastMessage(const QString&);

      //! broadcast start game
      void broadcastStartGame();

      //! check gameover condition
      void updateGameoverCondition();

      //! getter for rounds played
      int getGamesPlayed() const;

      //! check if game is only populated by bots
      bool isGamePopulatedByBots() const;

      //! getter for free player color
      Constants::Color getColorForNextPlayer() const;

      //! getter for game information object
      GameInformation getGameInformation();

      //! getter for the current game state
      Constants::GameState getState() const;

      //! set synchronization active
      void setSynchronizationActive(bool active);

      //! return synchronization flag
      bool isSynchronizationActive() const;

      //! player joins a game
      bool joinGame(Player* player, QTcpSocket* socket);

      //! getter for position skip count
      int getPositionSkipCount() const;

      //! get the actual number of bots in this game
      int getBotCount() const;

      //! getter for gameround ptr
      GameRound* getGameRound();

      //! getter for time left
      int getTimeLeft();

      //! check if start positions are initialized
      bool isStartPositionInitialized() const;

      //! set start positions to initialized
      void setStartPositionsInitialized(bool value);

      //! getter for message shown flag
      bool isGameOnlyPopulatedByBotsMessageShown() const;

      //! setter for message shown flag
      void setGameOnlyPopulatedByBotsMessageShown(bool value);

      //! process spectator client
      void processSpectator(QTcpSocket* tcpSocket);


   public slots:

      //! start new game
      void startGame();

      //! (force) stop the game
      void stopGame();

      //! start game synchronization
      void startSynchronization();

      //! game synchronization step
      void synchronize();

      //! prepare game
      void prepareGame();

      //! game finished
      void finishGame();

      //! player left the game
      void removePlayer(Player* player, QTcpSocket* playerSocket);

      //! add packet to list of outgoing packets
      void addOutgoingPacket(Packet* packet);


   signals:

      //! player was killed
      void playerKilled(int id);

      //! player leaves game
      void playerLeaves(int id);

      //! player is forced to leave game
      void forceLeaveGame(QTcpSocket* socket);

      //! state changed
      void stateChanged(Constants::GameState);


   private slots:

      //! update everything
      void update();

      //! bomb exploded (newschool, iterative detonations)
      void bombExploded(BombMapItem *bomb, bool);

      //! send game time packets
      void processGameTime();

      //! update prepare game countdown
      void updatePrepareGame();

      //! bomb kick animation triggered
      void bombKickedAnimation(Constants::Direction, float speed);

      //! send an idle packet
      void playerIdle(qint8 directions, Player* player);

      //! move player
      void playerMove(
         Player* player,
         float assignedXPos,
         float assignedYPos,
         qint8 directions
      );

      //! player kicks a bomb
      void playerKicksBomb(
         Player* player,
         MapItem* item,
         bool verticallyKicked,
         int keysPressed
      );

      //! player disease stopped
      void playerDiseaseStopped();

      //! send a message to the spectator that the game is currently running
      void processSpectatorMessage();

      //! spawn an extra
      void spawn();


   private:

      //! initialize skull setup
      void initSkullSetup();

      //! initialize the map
      void initializeMap();

      //! initialize timers
      void initializeTimers();

      //! initialize players
      void initializePlayerStartPositions();

      //! initialize extra spawn
      void initializeExtraSpawn();

      //! broadcast map
      void broadcastCreateMapItems();

      //! broadcast and clear map
      void broadcastClearMapItems();

      //! broadcast start positions
      void broadcastStartPositions();

      //! update positions
      void updatePositions();

      //! update player positions
      void updatePlayerPositions();

      //! update bombs
      void updateBombs();

      //! check if player collects an extra
      void updateExtras();

      //! check if infected players collide
      void updateInfections();

      //! send all packets from the outgoing-vector
      void sendBroadcastPackets();

      //! send single packet
      void sendPacket(QTcpSocket* socket, Packet* packet);

      //! get directions for given player
      int getPlayerDirections(Player* p);

      //! setter for the current game state
      void setState(Constants::GameState);

      //! update stats on player kill event
      void updateStatsPlayerKilled(Player* killer, Player* victim);

      //! update stats on player won event
      void processPlayerWon(Player*);

      //! only bots left
      void processOnlyBotsLeft();

      //! broadcast game stats
      void broadcastGameStats();

      //! increase the number of rounds played
      void increaseGamesPlayed();

      //! init all map related items on prepare-game-phase
      void initMapRelatedItems();

      //! check if kicking is possible
      bool isKickPossible(
         int x,
         int y,
         Constants::Direction kickDir
      );

      //! next game round
      void nextRound();

      //! reset round stats
      void resetRoundStats();

      //! broadcast game information
      void broadcastGameInformation();

      //! create extra skull
      void createInfection(
         Player* infectingPlayer,
         Player* infectedPlayer = 0,
         ExtraMapItem *extra = 0,
         const QList<Constants::SkullType>& faces = QList<Constants::SkullType>()
      );

      //! create kick animation
      void createKickAnimation(
         BombMapItem* kickedBomb,
         Constants::Direction kickDir
      );

      //! decrease immune times
      void updateImmuneTimes();

      //! make field immune
      void makeFieldImmune(int x, int y);

      //! check if field is immune
      bool isFieldImmune(int x, int y) const;

      //! a counter checking how many players left the game during the running round
      void increasePlayersLeftTheGameCount();

      //! reset the player left game counter
      void resetPlayersLeftTheGameCount();

      //! read players left game counter
      int getPlayersLeftTheGameCount() const;

      //! send a message to game owner
      void sendMessageToOwner(const QString& message);

      //! do not rotate dead player into stones
      void rotateDeadPlayerTowardsBomb(
         Constants::Direction direction,
         Player* player,
         int x,
         int y
      );

      //! check if extra spawning is enabled
      bool isSpawnExtrasEnabled() const;


      // members

      //! calls the update loop
      QTimer* mUpdateTimer;

      //! map socket <-> player
      QMap<QTcpSocket*, Player*> mPlayerSockets;

      //! map id <-> player
      QMap<qint8, Player*> mPlayers;

      //! map of expected packet sizes
      QMap<QTcpSocket*, quint16> mPacketSizes;

      //! outgoing packages
      QList<Packet*> mOutgoingPackets;

      //! playfield
      Map* mMap;

      //! map items to delete after destruction
      QSet<MapItem*> mDestroyedMapItems;

      //! one direction to check
      QList<Constants::Direction> mDirectionCheckCenter;

      //! all 4 directions to check
      QList<Constants::Direction> mDirectionCheckAll;

      //! game id
      int mGameId;

      //! static game id counter
      static int sGameId;

      //! game create data
      CreateGameData mCreateGameData;

      //! flag to indicate game is running
      bool mRunning;

      //! idle packet sent flag
      bool mIdlePacketSent;

      //! server time update timer
      QTimer* mGameTimeUpdateTimer;

      //! game time
      QTime mGameTime;

      //! game duration
      int mDuration;

      //! game owner
      Player* mCreator;

      //! game state
      Constants::GameState mState;

      //! preparation timer
      QTimer* mPreparationTimer;

      //! prepration time
      QTime mPreparationTime;

      //! preparation counter
      int mPreparationCounter;

      //! idle packet map
      QSet<Player*> mIdlePacketSentSet;

      //! game over condition needs to be checked in (recursive) detonation
      bool mCheckGameOver;

      //! skip countdown
      bool mSkipCountdown;

      //! position skips
      int mPositionSkipCount;

      //! number of rounds played
      int mGamesPlayed;

      //! game synchronization time
      QTime mSynchronizationTime;

      //! synchronization is active or not
      bool mSynchronizationActive;

      //! shake packet handler
      ExtraShakePacketHandler* mShakePacketHandler;

      //! maximum player speed
      float mMaxSpeed;

      //! sync max time
      int mSyncMaxTime;

      //! collision detection
      CollisionDetection* mCollisionDetection;

      //! game round instance
      GameRound mGameRound;

      //! immune times
      int* mImmuneTimes;

      //! counter of players left per round
      int mPlayerLeftTheGameCount;

      //! start positions are initialized
      bool mStartPositionsInitialized;

      //! only populated message shown flag
      bool mGameOnlyPopulatedByBotsMessageShown;

      //! list of spectators
      QList< QPointer<QTcpSocket> > mSpectators;

      //! extra spawning
      ExtraSpawn* mExtraSpawn;

      //! extra spawning enabled
      bool mExtraSpawnEnabled;
};

#endif
