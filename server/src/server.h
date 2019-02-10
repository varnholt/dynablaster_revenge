#ifndef SERVER_H
#define SERVER_H

#include <QAbstractSocket>
#include <QObject>
#include <QTimer>
#include <QMap>
#include <QList>

// shared
#include "constants.h"
#include "packet.h"
#include "serverconfiguration.h"

// forward declarations
class Game;
class Player;
class QTcpServer;
class QTcpSocket;

class Server : public QObject
{

Q_OBJECT

public:

   //! constructor
   Server();

   //! destructor
   ~Server();

   //! get server instance
   static Server* getInstance();

   //! check if server is listening
   bool isListening() const;

   //! get socket for player id
   QTcpSocket* getPlayerSocket( int playerId );

   //! getter for server configuration
   const ServerConfiguration& getServerConfiguration() const;


signals:

   //! new player connected to server
   void newPlayer(int playerId, const QString& nickName);

   //! player disconnected
   void removePlayer(int playerId, const QString& nickName);

   //! new game created
   void newGame(int gameId, const QString& gameName);

   //! game removed
   void removeGame(int gameId, const QString& gameName);


protected:

   //! initialize server configuration
   void initServerConfiguration();


private slots:

   //! new connection opened
   void newConnection();

   //! data received
   void data();

   //! player disconnected
   void disconnect();

   //! display error
   void displayError(QAbstractSocket::SocketError);

   void processStartGameRequest(QTcpSocket* tcpSocket, Packet* packet);
   void processJoinGameRequest(QTcpSocket* tcpSocket, Packet* packet);
   void processLoginRequest(QTcpSocket* tcpSocket, Packet* packet);
   void processListGamesRequest(QTcpSocket* tcpSocket);
   void processCreateGameRequest(QTcpSocket* tcpSocket, Packet* packet);
   void processGamePacket(QTcpSocket* tcpSocket, Packet* packet);
   void processPlayerLeavesGame(QTcpSocket* socket);
   void processPlayerSynchronize(QTcpSocket* tcpSocket, Packet* packet);
   void processRemoveGame(int gameId);
   void processRemoveAllBots(int gameId);
   void processBroadcastLeaveGameResponse(Player* player, Game* game);

   //! fix duplicate game names
   void correctDuplicateGameName(Game* game);

   //! send single packet
   void sendPacket(QTcpSocket* socket, Packet* packet);


private:

   //! fix duplicate player names
   QString correctDuplicatePlayerName(const QString& nick);

   //! tcp server
   QTcpServer* mTcpServer;

   //! map of expected packet sizes
   QMap<QTcpSocket*, quint16> mPacketSizes;

   //! map socket <-> player
   QMap<QTcpSocket*, Player*> mPlayerSockets;

   //! map socket <-> game
   QMap<QTcpSocket*, Game*> mSocketGameMapping;

   //! map of active games
   QMap<int, Game*> mGames;

   //! next player id
   int mPlayerId;

   //! static server instance
   static Server* sInstance;

   //! server configuration data
   ServerConfiguration mServerConfiguration;
};


#endif
