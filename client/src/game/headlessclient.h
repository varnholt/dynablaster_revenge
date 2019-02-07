#pragma once

// qt
#include <QColor>
#include <QMap>
#include <QObject>
#include <QTcpSocket>

// game
#include "gameinformation.h"
#include "playerinfo.h"

// foward declarations
class MapItem;
class Packet;
class Playlist;
class Server;

class QKeyEvent;
class QTcpSocket;

class HeadlessClient : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      HeadlessClient(/*const QString& host, const QString& nick*/);

      //! destructor
      virtual ~HeadlessClient();

      //! static getter for singleton instance
      static HeadlessClient* getInstance();

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
      QList<GameInformation>* getGames() const;

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

      //! getter for game mode
      Constants::GameMode getGameMode() const;

      //! setter for game mode
      void setGameMode(const Constants::GameMode &mode);

signals:

      // visualization

      //! signal game list update
      void gamesListUpdated(const QList<GameInformation>&);

      //! signal our player id
      void playerId(int id);

      //! signal level loading
      void loadLevel(const QString& level);

      //! play rumble effect if possible
      void rumble(float intensity, int ms);


      // game workflow and menus

      //! socket error
      void socketError();

      //! connected to server
      void connected();

      //! disconnected from server
      void disconnected();

      //! login response
      void loginResponse(bool);

      //! create game response
      void createGameResponse(
         bool success,
         int gameId,
         bool owner
      );

      //! join game response
      void joinGameResponse(bool);

      //! game start response
      void gameStarted();

      //! game stopped response
      void gameStopped();

      //! message received
      void messageReceived(
         int senderId,
         const QString& message,
         bool finished
      );

      //! time left during coutdown
      void countdown(int left);

      //! show the main menu
      void showMainMenu();

      //! extra has been removed
      void extraRemoved(
         int x,
         int y,
         bool destroyed,
         Constants::ExtraType extra,
         int playerId = -1
      );


      // game stats

      //! server time changed
      void timeChanged(int, int);

      //! player's score changed
      void scoreChanged(int);


      // server

      void hosting(bool);


   public slots:

      // key event handlers

      //! process key pressed event handed from gui
      void keyPressed(QKeyEvent*);

      //! process key released event handed from gui
      void keyReleased(QKeyEvent*);

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
      void sendMessage(
         const QString& message,
         bool finishedTyping,
         int receiverId = -1
      );


      // functions for communication from or to the menu

      //! process login request
      void loginRequest(
         const QString& host,
         const QString& nick
      );

      //! game list request
      void gameListRequest();

      //! request leave game
      void leaveGameRequest();

      //! level loaded
      void levelLoaded(const QString& path);


   private slots:

      //! data received on socket
      void readData();

      // //! reconnect
      // void reconnect();

      //! connect client
      void clientConnect();

      //! disconnect client
      void clientDisconnect();

      //! process a packet
      void processPacket(Packet* packet);

      //! game state changed
      void gameStateChanged();

      //! socket error
      void socketError(QAbstractSocket::SocketError);


   private:

      void processCreateGameResponse(Packet* packet);
      void processJoinGameResponse(Packet* packet);
      void processLoginResponse(Packet* packet);
      void processPlayerKilled(Packet* packet);
      void processDetonation(Packet* packet);
      void processError(Packet* packet);
      void processPosition(Packet* packet);
      void processGameStats(Packet* packet);
      void processGameEvent(Packet* packet);
      void processStartGameResponse(Packet* packet);
      void processStopGameResponse(Packet* packet);
      void processMessage(Packet* packet);
      void processTime(Packet* packet);
      void processCountdown(Packet *packet);
      void processExtraShake(Packet* packet);

      //! send a packet
      void send(Packet *packet);

      //! check for packets
      bool packetAvailable(QDataStream& source);

      //! get mapitem by mapitem id
      MapItem* getMapItem(int id) const;

      //! broadcast data about added players
      void broadcastAddPlayerData();

      //! send current keys pressed
      void sendKeysPressedPacket();

      //! remove bomb key flag from key pressed combination
      void removeBombKeyFlag();

      //! debug keyboard input
      void debugKeyboardInput();

      //! reset client after disonnect or error
      void resetGameData();

      //! reset client state
      void resetClientState();

      //! init playback
      void initializePlayback();

      //! list network devices
      QList<QString> getLocalIps() const;


      // members

      //! keys currently pressed
      int mKeysPressed;

      //! flag indicating bomb key was released
      bool mBombReleased;

      //! tcp socket to server
      QTcpSocket *mSocket;

      //! blocksize of packet which is received from server
      quint16 mBlockSize;

      //! player id
      int mId;

      //! game id
      int mGameId;

      //! player alive
      bool mDead;

      //! map items
      QMap<int,MapItem*> mMapItems;

      //! host name
      QString mHost;

      //! nick name
      QString mNick;

      //! list of games available
      mutable QList<GameInformation> mGames;

      //! connected flag
      bool mConnected;

      //! login is requested after connect to host
      bool mLoginAfterConnect;

      //! ingame message to send
      QString mMessage;

      //! current player info
      QPointer<PlayerInfo> mCurrentPlayerInfo;

      //! getter for client singleton
      static HeadlessClient* mInstance;

      //! typing activated
      bool mIngameMessagingActive;

      //! main menu active
      bool mMainMenuActive;

      //! game mode
      Constants::GameMode mGameMode;
};


