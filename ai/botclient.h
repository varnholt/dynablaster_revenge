#ifndef BOTCLIENT_H
#define BOTCLIENT_H

// Qt
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QTime>

// shared
#include "gameinformation.h"
#include "serverconfiguration.h"

// forward declarations
class Bot;
class BotMap;
class MapItem;
class Packet;
class BotPlayerInfo;
class QTcpSocket;


class BotClient : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      BotClient(QObject *parent = 0);

      //! destructor
      virtual ~BotClient();

      //! initialize bot client
      void initialize();

      //! connect to server
      void connectToServer();

      void initializeAutoJoinStart();

      //! setter for host name
      void setHost(const QString& host);

      //! setter for nickname to use in login
      void setNick(const QString& nick);

      //! setter for bot
      void setBot(Bot* bot);

      //! setter for bot map
      void setBotMap(BotMap* map);

      //! setter for autojoin
      void setAutoJoin(bool enabled);

      //! setter for autostart
      void setAutoStart(bool enabled);

      //! setter for game id
      void setGameId(int id);

      //! getter for game id
      int getGameId() const;

      //! get info object for given player id
      BotPlayerInfo* getPlayerInfo(int id) const;

      //! get a list of players
      QList<BotPlayerInfo*> getPlayerInfoList() const;

      //! getter for the map of players
      QMap<int, BotPlayerInfo *> *getPlayerInfoMap();

      //! setter for server configuration
      void setServerConfiguration(const ServerConfiguration&);

      //! getter for server configuration
      const ServerConfiguration& getServerConfiguration() const;

      //! getter for delta x
      float getDeltaX() const;

      //! setter for delta x
      void setDeltaX(float value);

      //! getter for delta y
      float getDeltaY() const;

      //! setter for delta y
      void setDeltaY(float value);


signals:

      //! player logged in
      void updatePlayerId(int id);

      //! a map item has been created
      void mapItemCreated(MapItem *item);

      //! a map item has been removed
      void mapItemRemoved(MapItem *item);

      //! bomb placed
      // void bombPlaced(int playerId, int x, int y, int flames);

      //! game selected
      void gameSelected();

      //! game started
      void gameStarted();

      //! update player position
      void updatePlayerPosition(int id, float x, float y, float ang);

      //! update player speed
      void updatePlayerSpeed(int id, float x, float y, float ang);

      //! bot shall be removed
      void remove();

      //! extra shake
      void extraShake(int id);

      //! mark temporary hazardous
      void markHazardousTemporary(int x, int y, int millis, int fieldCount = 0);

      //! make hazardous temp for bomb kicks
      void bombKicked(
         int startX,
         int startY,
         Constants::Direction,
         int flames
      );


   public slots:

      //! send walk packet
      void walk(qint8);

      //! drop a bomb
      void bomb();

      //! send a message to others
      void sendMessage(
         const QString& message,
         bool finishedTyping = true,
         int receiverId = -1
      );

      //! delete obsolete items
      void deleteObsoleteMapItems();


   private slots:

      //! data received on socket
      void readData();

      //! reconnect
      void reconnect();

      //! connect client
      void clientConnect();

      //! disconnect client
      void clientDisconnect();

      //! login
      void login();

      //! select game
      void selectGame();

      //! join a game
      void joinGame();

      //! start game
      void startGame();


      // packet handlers

      //! login response
      void processLoginResponse(Packet *p);

      //! join game response
      void processJoinGameResponse(Packet *p);

      //! leave game response
      void processLeaveGameResponse(Packet* p);

      //! process position packet
      void processPosition(Packet *packet);

      //! a map item has been created
      void processMapItemCreated(Packet* packet);

      //! a map item has been destroyed
      void processMapItemRemoved(Packet* packet);

      //! a map item is moved (kicked)
      void processMapItemMove(Packet* packet);

      //! an extra map item has been created
      void processExtraMapItemCreated(Packet* packet);

      //! an extra map item has been destroyed
      void processMapItemDestroyed(Packet* packet);

      //! game was started
      void processStartGameResponse(Packet *packet);

      //! game event received
      void processGameEvent(Packet *packet);

      //! player infected packet received
      void processPlayerInfected(Packet* packet);

      //! player killed packet received
      void processPlayerKilled(Packet* packet);

      //! game stopped
      void processStopGameResponse(Packet* packet);

      //! list games
      void processListGameResponse(Packet* packet);

      //! process countdown
      void processCountdown(Packet* packet);

      //! process extra shake
      void processExtraShake(Packet* packet);

      //! setter for game joined flag
      void setGameJoined(bool joined);

      //! getter for game joined flag
      bool isGameJoined() const;

      //! setter for player speed
      void setSpeed(float speed);

      //! getter for player speed
      float getSpeed() const;

      //! getter for player id
      int getPlayerId() const;


   private:

      //! queue delete item
      void queueObsoleteItem(MapItem* item);

      //! clear obsolete items
      void clearObsoleteItems();

      //! getter for map item by id
      MapItem* getMapItem(int id) const;

      //! add a map item
      void addMapItem(MapItem* mapItem);

      //! remove a map item
      void removeMapItem(MapItem* mapItem);

      //! send a packet
      void send(Packet *packet);

      //! check for packets
      bool packetAvailable(QDataStream& source);

      //! create a new map
      void createMap(Constants::Dimension dimensions);

      //! initialize a bot map
      void initBotMap(int width, int height);

      //! getter for bot ptr
      Bot* getBot() const;

      //! reset bot
      void resetBot();

      //! track a bug
      void bugTrack1();

      //! getter for walk count
      int getWalkCount() const;

      //! increase walk count
      void increaseWalkCount();

      //! reset walk count
      void resetWalkCount();


      //! mapitem mutex
      mutable QMutex mMutex;

      //! tcp socket to server
      QTcpSocket* mSocket;

      //! host name
      QString mHost;

      //! nick name
      QString mNick;

      //! connected flag
      bool mConnected;

      //! expected block size of current packet
      quint16 mBlockSize;

      //! bot
      Bot* mBot;

      //! botmap
      BotMap* mBotMap;

      //! game id to join
      int mGameId;

      //! automatically join game
      bool mAutoJoin;

      //! automatically start game
      bool mAutoStart;

      //! player id
      int mPlayerId;

      //! map items
      QMap<int,MapItem*> mMapItems;

      //! map id <-> player info object
      QMap<int, BotPlayerInfo*> mPlayerInfo;

      //! bot's keyboard keys pressed
      int mKeysPressed;

      //! list of games available
      mutable QList<GameInformation> mGames;

      //! true if game was succesfully joined
      bool mGameJoined;

      //! current speed
      float mSpeed;

      //! queue of items to be deleted later
      QQueue<MapItem*> mObsoleteMapItems;

      //! server configuration
      ServerConfiguration mServerConfiguration;

      //! time elapsed since last bomb
      QTime mBombTime;

      //! walk counter
      int mWalkCount;

      //! delta x
      float mDeltaX;

      //! delta y
      float mDeltaY;
};

#endif // BOTCLIENT_H

