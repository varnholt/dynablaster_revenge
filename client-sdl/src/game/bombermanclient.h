#ifndef BOMBERMANCLIENT_H
#define BOMBERMANCLIENT_H

// qt
#include <QColor>
#include <QObject>
#include <QTimer>

// shared
#include "packetstreambuffer.h"
#include "signal.h"

#include <map>
#include <unordered_map>
#include <vector>

// game
#include "gameinformation.h"
#include "playerinfo.h"

// foward declarations
class BotFactory;
class MapItem;
class Packet;
class Playlist;
class PositionInterpolation;
class Server;

class QKeyEvent;
struct NET_Address;
struct NET_StreamSocket;

class BombermanClient : public QObject
{
   Q_OBJECT

public:
   //! constructor
   BombermanClient(/*const QString& host, const QString& nick*/);

   //! destructor
   virtual ~BombermanClient();

   //! static getter for singleton instance
   static BombermanClient* getInstance();

   //! initialize client
   void initialize();

   //! setter for hostname to connect to
   void setHost(const QString& host);

   //! getter for hostname to connect to
   const QString& getHost() const;

   //! setter for nickname to use in login
   void setNick(const QString& nick);

   //! getter for nickname to use in login
   const QString& getNick() const;

   //! connect to server
   void connectToServer();

   //! immediate login after connecting to server
   void setLoginAfterConnect(bool loginAfterConnect);

   //! getter for list of games
   std::vector<GameInformation>* getGames() const;

   //! setter for game id
   void setGameId(int id);

   //! getter for game id
   int getGameId() const;

   //! check if game id is valid
   bool isGameIdValid() const;

   //! getter for game information
   GameInformation* getGameInformation(int id) const;

   //! getter for current game information
   GameInformation* getCurrentGameInformation() const;

   //! setter for player id
   void setPlayerId(int);

   //! getter for player id
   int getPlayerId() const;

   //! is player game owner
   bool isPlayerOwner() const;

   //! getter for player color by player id
   Constants::Color getColor(int playerId) const;

   //! get list of players
   std::vector<PlayerInfo*> getPlayerInfoList() const;

   //! get map of player
   std::map<int, PlayerInfo*>* getPlayerInfoMap() const;

   //! add player info to map
   void addPlayerInfo(int id, PlayerInfo* info);

   //! remove player info
   void removePlayerInfo(int id);

   //! get info object for given player id
   PlayerInfo* getPlayerInfo(int id) const;

   //! getter for position interpolation
   PositionInterpolation* getPositionInterpolation() const;

   //! setter for current player info
   void setCurrentPlayerInfo(PlayerInfo* info);

   //! getter for current player info
   PlayerInfo* getCurrentPlayerInfo() const;

   //! getter for message
   const QString& getMessage() const;

   //! setter for message
   void setMessage(const QString& message);

   //! setter for connected state
   void setConnected(bool connected);

   //! check if client is connected
   bool isConnected() const;

   //! check if we're hosting games
   bool isHosting() const;

   //! getter for single/multi player mode
   bool isSinglePlayer() const;

   //! getter for game mode
   Constants::GameMode getGameMode() const;

   //! setter for game mode
   void setGameMode(const Constants::GameMode& mode);

   // communicate with menus

   //! check if main menu is active
   bool isMainMenuActive() const;

public:
   // Signal<> replacements for BombermanClient's former Qt signals (see
   // project_full_qt_removal_scope memory) - every external connect() site now uses these.

   Signal<int, float, float, float> setPlayerPositionSignal;
   Signal<int, float, float, float> setPlayerSpeedSignal;
   Signal<MapItem*> createMapItemSignal;
   Signal<MapItem*> removeMapItemSignal;
   Signal<MapItem*, float> destroyMapItemSignal;
   Signal<int, int, int, int, int, int, float> detonationSignal;
   Signal<int, const QString&, Constants::Color> addPlayerSignal;
   Signal<int> removePlayerSignal;
   Signal<int> playerIdSignal;
   Signal<float, float> playfieldScaleSignal;
   Signal<int, int> playfieldSizeSignal;
   Signal<const QString&> loadLevelSignal;
   Signal<MapItem*> shakeBlockSignal;
   Signal<int, Constants::SkullType, int, int, int> playerInfectedSignal;
   Signal<> connectedSignal;
   Signal<> disconnectedSignal;
   Signal<bool> loginResponseSignal;
   Signal<bool, int, bool> createGameResponseSignal;
   Signal<bool> joinGameResponseSignal;
   Signal<> gameStartedSignal;
   Signal<> gameStoppedSignal;
   Signal<int, const QString&, bool> messageReceivedSignal;
   Signal<int> countdownSignal;
   Signal<std::map<int, PlayerInfo*>*> playerInfoMapUpdatedSignal;
   Signal<> showGameSignal;
   Signal<> showMenuSignal;
   Signal<> showMainMenuSignal;
   Signal<int, int, bool, Constants::ExtraType, int> extraRemovedSignal;

   Signal<bool> zoomInSignal;
   Signal<bool> zoomOutSignal;
   Signal<MapItem*, Constants::Direction, float, int, int> moveMapItemSignal;
   Signal<const std::vector<GameInformation>&> gamesListUpdatedSignal;
   Signal<float, int> rumbleSignal;
   Signal<int, int> timeChangedSignal;
   Signal<bool> hostingSignal;

public slots:

   // key event handlers

   //! process key pressed event handed from gui
   void keyPressed(QKeyEvent*);

   //! process key released event handed from gui
   void keyReleased(QKeyEvent*);

   //! release all keys
   void releaseAllKeys();

   //! process key pressed (not given as key event)
   void processKeyPressed(int key);

   //! process key released
   void processKeyReleased(int key);

   // game workflow

   //! try to log in
   void login(const QString& nick = tr("developer"));

   //! ask the server to stop the game
   void stopGame();

   //! list games
   void listGames();

   //! create a game
   void createGame(
      const QString& name,
      const QString& level,
      int rounds,
      int duration,
      int maxPlayers,
      bool extraBombEnabled,
      bool extraFlameEnabled,
      bool extraSpeedupEnabled,
      bool extraKickEnabled,
      bool extraSkullsEnabled,
      Constants::Dimension dimension
   );

   //! automatically create a game
   void createGameAutomatic();

   //! join a game
   void joinGame(int game = 1);

   //! start a game
   void startGame(int game = 1);

   //! stop a game
   void stopGame(int game);

   //! send a message to others
   void sendMessage(const QString& message, bool finishedTyping, int receiverId = -1);

   //! host a game
   void host();

   //! initialize bots
   void initializeBots();

   // functions for communication from or to the menu

   //! process login request
   void loginRequest(const QString& host, const QString& nick);

   //! game list request
   void gameListRequest();

   //! request leave game
   void leaveGameRequest();

   //! player is idle
   void idle(bool idle);

   //! level loaded
   void levelLoaded(const QString& path);

   //! menu page changed
   void setMainMenuActive(bool active);

   //! show ip addresses
   void showIps();

private slots:

   //! poll for connection progress and incoming data, once per tick
   void poll();

   //! connect client
   void clientConnect();

   //! disconnect client
   void clientDisconnect();

   //! process a packet
   void processPacket(Packet* packet);

   //! demo mode is finished
   void playbackFinished();

   //! game state changed
   void gameStateChanged();

   //! clear list with player info
   void clearPlayerInfoMap();

private:
   void processCreateGameResponse(Packet* packet);
   void processJoinGameResponse(Packet* packet);
   void processListGameResponse(Packet* packet);
   void processLoginResponse(Packet* packet);
   void processPlayerKilled(Packet* packet);
   void processPlayerInfected(Packet* packet);
   void processDetonation(Packet* packet);
   void processError(Packet* packet);
   void processPosition(Packet* packet);
   void processMapItemCreated(Packet* packet);
   void processMapItemMove(Packet* packet);
   void processExtraMapItemCreated(Packet* packet);
   void processGameStats(Packet* packet);
   void processExtraMapItemDestroyed(Packet* packet);
   void processMapItemRemoved(Packet* packet);
   void processStartGameResponse(Packet* packet);
   void processStopGameResponse(Packet* packet);
   void processGameEvent(Packet* packet);
   void processMessage(Packet* packet);
   void processTime(Packet* packet);
   void processCountdown(Packet* packet);
   void processLeaveGameResponse(Packet* packet);
   void processExtraShake(Packet* packet);

   //! send a packet
   void send(Packet* packet);

   //! check for packets
   bool packetAvailable();

   //! read and dispatch all available data from the socket
   void readData();

   //! tear down whatever connection attempt or connection is in progress
   void disconnectFromServer();

   //! show a generic connection-failure message to the user
   void reportConnectionError(const char* reason);

   //! get mapitem by mapitem id
   MapItem* getMapItem(int id) const;

   //! broadcast data about added players
   void broadcastAddPlayerData();

   //! broadcast player start positions
   void broadcastPlayerStartPositions();

   //! send current keys pressed
   void sendKeysPressedPacket();

   //! remove bomb key flag from key pressed combination
   void removeBombKeyFlag();

   // ingame messaging

   //! getter for ingame messaging flag
   bool isIngameMessagingActive() const;

   //! setter for ingame messaging flag
   void setIngameMessagingActive(bool active);

   //! toggle ingame messaging
   void toggleIngameMessaging();

   //! debug keyboard input
   void debugKeyboardInput();

   //! reset client after disonnect or error
   void resetGameData();

   //! reset client state
   void resetClientState();

   //! init playback
   void initializePlayback();

   //! list network devices
   std::vector<QString> getLocalIps() const;

   // members

   //! keys currently pressed
   int mKeysPressed;

   //! flag indicating bomb key was released
   bool mBombReleased;

   //! stream socket to server, null unless connected or connecting
   NET_StreamSocket* mSocket;

   //! host address pending resolution, null once resolved (or if not resolving)
   NET_Address* mAddress;

   //! drives poll() once per tick
   QTimer* mPollTimer;

   //! incoming byte buffer
   PacketStreamBuffer mBuffer;

   //! blocksize of packet which is received from server
   uint16_t mBlockSize;

   //! player id
   int mId;

   //! game id
   int mGameId;

   //! player alive
   bool mDead;

   //! map items
   std::unordered_map<int, MapItem*> mMapItems;

   //! host name
   QString mHost;

   //! nick name
   QString mNick;

   //! list of games available
   mutable std::vector<GameInformation> mGames;

   //! connected flag
   bool mConnected;

   //! login is requested after connect to host
   bool mLoginAfterConnect;

   //! server
   Server* mServer;

   //! ingame message to send
   QString mMessage;

   //! map id <-> player info object
   mutable std::map<int, PlayerInfo*> mPlayerInfo;

   //! current player info
   QPointer<PlayerInfo> mCurrentPlayerInfo;

   //! getter for client singleton
   static BombermanClient* mInstance;

   //! position interpolation
   PositionInterpolation* mPositionInterpolation;

   //! typing activated
   bool mIngameMessagingActive;

   //! main menu active
   bool mMainMenuActive;

   //! bot factory
   BotFactory* mBotFactory;

   //! game mode
   Constants::GameMode mGameMode;
};

#endif
