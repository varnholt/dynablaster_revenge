// header
#include "server.h"

// server
#include "game.h"
#include "serverversion.h"

// shared
#include "creategamerequestpacket.h"
#include "creategameresponsepacket.h"
#include "joingamerequestpacket.h"
#include "joingameresponsepacket.h"
#include "leavegamerequestpacket.h"
#include "leavegameresponsepacket.h"
#include "listgamesrequestpacket.h"
#include "listgamesresponsepacket.h"
#include "loginrequestpacket.h"
#include "loginresponsepacket.h"
#include "player.h"
#include "playersynchronizepacket.h"
#include "startgamerequestpacket.h"
#include "startgameresponsepacket.h"
#include "stopgamerequestpacket.h"
#include "stopgameresponsepacket.h"

// Qt
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>

// c
#include <math.h>
#include <cstdint>

// static variables
Server* Server::sInstance = nullptr;


//-----------------------------------------------------------------------------
/*!
   constructor
*/
Server::Server()
   : mTcpServer(nullptr),
     mPlayerId(0)
{
   sInstance = this;

   initServerConfiguration();

   qDebug(
      "Server::Server: Dynablaster Revenge Server - v%s, rev. %s",
      SERVER_VERSION,
      SERVER_REVISION
   );

   qDebug("Server::Server: binding to port %d..", SERVER_PORT);

   // create tcp server instance
   mTcpServer = new QTcpServer(this);

   // open server
   if (!mTcpServer->listen(QHostAddress::Any, SERVER_PORT))
   {
      qDebug(
         "Server::Server: Dynablaster Revenge Server: Unable to start the server: %s.",
         qPrintable(mTcpServer->errorString())
      );
   }
   else
   {
      qDebug("Server::Server: Dynablaster Revenge Server: server initialized");
   }

   // check for incoming connections
   connect(
      mTcpServer,
      SIGNAL(newConnection()),
      this,
      SLOT(newConnection())
   );
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
Server::~Server()
{
   qDebug("Server::~Server");

   delete mTcpServer;
}


//-----------------------------------------------------------------------------
/*!
   \return server instance
*/
Server *Server::getInstance()
{
   if (!sInstance)
      new Server();

   return sInstance;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if server is listening
*/
bool Server::isListening() const
{
   return mTcpServer->isListening();
}


//-----------------------------------------------------------------------------
/*!
   get socket for player id
   \param playerId player id
*/
QTcpSocket* Server::getPlayerSocket( int playerId )
{
   QMap<QTcpSocket*, Player*>::ConstIterator it;
   for (it= mPlayerSockets.constBegin(); it!= mPlayerSockets.constEnd(); it++)
   {
      Player* player= it.value();
      if (player && player->getId() == playerId)
         return it.key();
   }
   return nullptr;
}


//-----------------------------------------------------------------------------
/*!
   \return reference to server configuration
*/
const ServerConfiguration &Server::getServerConfiguration() const
{
   return mServerConfiguration;
}


//-----------------------------------------------------------------------------
/*!
*/
void Server::initServerConfiguration()
{
   // init config
   QSettings settings (
      SERVER_CONFIG_FILE_SERVER,
      QSettings::IniFormat
   );

   int bombTickTime =
      settings.value("tick_count", SERVER_BOMB_TICKTIME_DEFAULT).toInt();

   mServerConfiguration.setBombTickTime(bombTickTime);
}


//-----------------------------------------------------------------------------
/*!
   new connection to server opened
*/
void Server::newConnection()
{
   while (mTcpServer->hasPendingConnections())
   {
      QTcpSocket* socket = mTcpServer->nextPendingConnection();

      // disable the nagle's algorithm to provide low latency
      socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

      qDebug(
         "Server::newConnection: peer address: %s, socket ptr: %p",
         qPrintable(socket->peerAddress().toString()),
         socket
      );

      connect(
         socket,
         SIGNAL(readyRead()),
         this,
         SLOT(data())
      );

      connect(
         socket,
         SIGNAL(disconnected()),
         this,
         SLOT(disconnect())
      );

      connect(
         socket,
         SIGNAL(error(QAbstractSocket::SocketError)),
         this,
         SLOT(displayError(QAbstractSocket::SocketError))
      );

      // create new player
      Player* player = new Player(mPlayerId++);
      mPlayerSockets.insert(socket, player);
   }
}


//-----------------------------------------------------------------------------
/*!
   send single packet
*/
void Server::sendPacket(QTcpSocket* socket, Packet* packet)
{
   // init bytearray
   packet->serialize();

   /*
   qDebug(
      "Server::sendPacket: sending packet to player %p (%d bytes)",
      socket,
      packet->size()
   );
   */

   // send packet
   socket->write(*packet);

   // clean up
   delete packet;
}


//-----------------------------------------------------------------------------
/*!
   \param tcpSocket sender socket
   \param packet packet to process
*/
void Server::processStartGameRequest(QTcpSocket* tcpSocket, Packet* packet)
{
   StartGameRequestPacket* request = dynamic_cast<StartGameRequestPacket*>(packet);

   // TODO:
   // game needs "isRunning"
   // -> to be checked before restarting

   QMap<int, Game*>::const_iterator iter = mGames.constFind(request->getId());

   if (iter != mGames.constEnd())
   {
      Game* game = iter.value();

      if (
            mPlayerSockets[tcpSocket]->getId()
         == game->getCreator()->getId()
      )
      {
         // game->prepareGame();
         game->startSynchronization();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param tcpSocket sender socket
   \param packet packet to process
*/
void Server::processJoinGameRequest(QTcpSocket* tcpSocket, Packet* packet)
{
   JoinGameRequestPacket* request = dynamic_cast<JoinGameRequestPacket*>(packet);

   QMap<int, Game*>::const_iterator iter = mGames.constFind(request->getId());

   if (iter != mGames.constEnd())
   {
      Game* game = iter.value();

      QMap<QTcpSocket*, Player*>::const_iterator socketIter =
         mPlayerSockets.constFind(tcpSocket);

      if (socketIter != mPlayerSockets.constEnd())
      {
         Player* player = socketIter.value();

         if (game->joinGame(player, tcpSocket))
         {
            game->broadcastMessage(
               tr("%1 joined the game").arg(player->getNick())
            );

            mSocketGameMapping.insert(
               tcpSocket,
               game
            );

            // if the game is currently running, inform the player that the
            // game has been started
            game->processSpectator(tcpSocket);
         }
      }
      else
      {
         JoinGameResponsePacket* response = new JoinGameResponsePacket(
            false,
            game->getId(),
            -1,
            "fuck off",
            Constants::ColorWhite
         );

         sendPacket(tcpSocket, response);
      }
   }
}


void Server::processPlayerSynchronize(QTcpSocket* tcpSocket, Packet* packet)
{
   /*
      game level loading synchronizing workflow:

      1) player joins game
      2) client loads level data (in the background; via thread)
      3) client starts game
      4) server waits until all players will have sent a synch packet
      5) client finishes loading level data and sends synch-packet
      6) server delays until timeout (= 10 secs) or until all players sent a sync packet
      7) kill those players who didn't send a synch packet
      8) start game
   */

   PlayerSynchronizePacket* request = dynamic_cast<PlayerSynchronizePacket*>(packet);

   if (request->getSynchronizeProcess() == PlayerSynchronizePacket::LevelLoaded)
   {
      QMap<QTcpSocket*, Game*>::const_iterator iter =
         mSocketGameMapping.constFind(tcpSocket);

      if (iter != mSocketGameMapping.constEnd())
      {
         Game* game = iter.value();

         QMap<QTcpSocket*, Player*>::const_iterator socketIter =
            mPlayerSockets.constFind(tcpSocket);

         if (socketIter != mPlayerSockets.constEnd())
         {
            Player* player = socketIter.value();

            player->setLoadingSynchronized(true);

            qDebug("Server::processPlayerSynchronize: game: %d player: '%s'",
               game->getId(),
               qPrintable(player->getNick())
            );
         }
      }
   }
}



void Server::processLoginRequest(QTcpSocket* tcpSocket, Packet* packet)
{
   LoginRequestPacket* request = dynamic_cast<LoginRequestPacket*>(packet);

   qDebug("Server::processLoginRequest: login request packet received");

   // lookup player
   Player* player = mPlayerSockets[tcpSocket];

   if (player)
   {
      bool wasLoggedIn = player->isLoggedIn();

      QString nick = request->getNick();
      if (!wasLoggedIn)
         nick = correctDuplicatePlayerName(nick);

      // init player attributes
      player->setLoggedIn(true);
      player->setNick(nick);
      player->setBot(request->isBot());

      // send login acceptance directly back to sender
      LoginResponsePacket* singleResponse = new LoginResponsePacket(
         false,
         player->getId(),
         player->getNick(),
         getServerConfiguration()
      );

      sendPacket(tcpSocket, singleResponse);

      // signal new player to gui
      if (!wasLoggedIn)
      {
         emit newPlayer(player->getId(), player->getNick());
      }
   }
}


void Server::processListGamesRequest(QTcpSocket* tcpSocket)
{
   // ListGamesRequestPacket* request = (ListGamesRequestPacket*)packet;
   QList<GameInformation> games;

   foreach (Game* game, mGames)
   {

//      GameInformation(
//         game->getId(),
//         game->getPlayerCount(),
//         game->getMaximumPlayerCount(),
//         game->getName(),
//         game->getLevelName(),
//         game->getCreator()->getId(),
//         game->getMapDimension(),
//         game->getExtras(),
//         game->getDuration(),
//         game->getRoundsPlayed()
//      )

      games.append(game->getGameInformation());
   }

   ListGamesResponsePacket* response = new ListGamesResponsePacket(
      games
   );

   sendPacket(tcpSocket, response);
}


void Server::processCreateGameRequest(QTcpSocket* tcpSocket, Packet* packet)
{
   // the server current does not support a maximum game count.
   // if this ought to be implemented, we need to return a gameinformation
   // object countaining a gameid of -1.

   CreateGameRequestPacket* request = dynamic_cast<CreateGameRequestPacket*>(packet);

   // create game
   Game* game = new Game();
   Player* player = mPlayerSockets[tcpSocket];
   game->setCreateGameData(request->getData());
   game->setCreator(player);
   game->getGameRound()->setCount(request->getData().mRounds);

   connect(
      game,
      SIGNAL(forceLeaveGame(QTcpSocket*)),
      this,
      SLOT(processPlayerLeavesGame(QTcpSocket*))
   );

   // autocorrect duplicate game names
   correctDuplicateGameName(game);

   mGames.insert(game->getId(), game);
   emit newGame(game->getId(), game->getName());

   // initialize game
   game->initialize();

   CreateGameResponsePacket* response =
      new CreateGameResponsePacket(
         game->getGameInformation()
      );

   sendPacket(tcpSocket, response);
}


void Server::processGamePacket(QTcpSocket* tcpSocket, Packet* packet)
{
   QMap<QTcpSocket*, Game*>::const_iterator iter =
      mSocketGameMapping.constFind(tcpSocket);

   // if the socket already joined a game, let the
   // according game instance handle the communication
   if (iter != mSocketGameMapping.constEnd())
   {
      iter.value()->processPacket(
         tcpSocket,
         packet
      );
   }
}


//-----------------------------------------------------------------------------
/*!
   data received from client

   concept:

      Server
      |
      + QTcpServer*
      + newConnection() => new Player()
                               |
                               + TcpSocket*
      + data() => while(data is available)
                  1) read from QTcpSocket*
                  2) deserialize Packet*
                  3) process all server-related packets
                  4) process all game-related packets
*/
void Server::data()
{
   // qDebug("Server::data(): packet received");

   QTcpSocket* tcpSocket = dynamic_cast<QTcpSocket*>(sender());

   // check if player is known
   if (!mPlayerSockets.contains(tcpSocket))
   {
      qDebug("Server::data(): unknown player. fuck off");
      return;
   }

   QDataStream in(tcpSocket);
   in.setVersion(QDataStream::Qt_4_6);

   while (true)
   {
      quint16 blockSize = mPacketSizes[tcpSocket];

      // blocksize not initialized yet
      if (blockSize == 0)
      {
         if (tcpSocket->bytesAvailable() < static_cast<int32_t>(sizeof(quint16)))
         {
            // qDebug("Server::data(): cannot read packet size, packet too small");
            return;
         }

         in >> blockSize;
         mPacketSizes[tcpSocket]=blockSize;
      }

      // wait for more data
      if (tcpSocket->bytesAvailable() < blockSize)
      {
         qDebug(
            "Server::data: waiting for packet to complete [2] "
            "(actually: %d < expected: %d)",
            static_cast<int32_t>(tcpSocket->bytesAvailable()),
            blockSize
         );

         return;
      }

      // reset expected blocksize
      mPacketSizes[tcpSocket]=0;

      // block was read completely
      Packet* packet = Packet::deserialize(in);

      if (packet)
      {
         // packet->debug();

         switch(packet->getType())
         {
            case Packet::CREATEGAMEREQUEST:
            {
               processCreateGameRequest(tcpSocket, packet);
               break;
            }

            case Packet::LISTGAMESREQUEST:
            {
               processListGamesRequest(tcpSocket);
               break;
            }

            case Packet::LEAVEGAMEREQUEST:
            {
               processPlayerLeavesGame(tcpSocket);
               break;
            }

            case Packet::LOGINREQUEST:
            {
               processLoginRequest(tcpSocket, packet);
               break;
            }

            case Packet::JOINGAMEREQUEST:
            {
               processJoinGameRequest(tcpSocket, packet);
               break;
            }

            case Packet::STARTGAMEREQUEST:
            {
               processStartGameRequest(tcpSocket, packet);
               break;
            }

            case Packet::PLAYERMODIFIED:
            {
               break;
            }

            case Packet::PLAYERSYNCHRONIZEPACKET:
            {
               processPlayerSynchronize(tcpSocket, packet);
               break;
            }

            case Packet::INVALID:
            {
               qWarning("Server::data: Packet::INVALID received");
               break;
            }

            default:
            {
               processGamePacket(tcpSocket, packet);
               break;
            }
         }

         delete packet;
      }
   }
}


//----------------------------------------------------------------------------
/*!
   player disconnected from server
*/
void Server::disconnect()
{
   qDebug("Server::disconnect");

   // remove player from list of sockets
   QTcpSocket* tcpSocket = dynamic_cast<QTcpSocket*>(sender());
   // Player* player = mPlayerSockets[tcpSocket];

   // notify other players
   processPlayerLeavesGame(tcpSocket);

   // delete player
   Player* player= mPlayerSockets.take(tcpSocket);

   // inform server ui that player disconnected
   if (player)
      emit removePlayer(player->getId(), player->getNick());

   delete player;
}


//----------------------------------------------------------------------------
/*!
   \param player ptr to player
   \param game ptr to game
*/
void Server::processBroadcastLeaveGameResponse(Player* player, Game* game)
{
   QMap<QTcpSocket*, Player*>* players = game->getPlayerSockets();
   QMapIterator<QTcpSocket*, Player*> i(*players);
   while (i.hasNext())
   {
      i.next();

      qDebug(
         "Server::processBroadcastLeaveGameResponse: "
         "informing '%s' that '%s' left",
         qPrintable(i.value()->getNick()),
         qPrintable(player->getNick())
      );

      sendPacket(
         i.key(),
         new LeaveGameResponsePacket(
            game->getId(),
            player->getId()
         )
      );
   }
}


//----------------------------------------------------------------------------
/*!
   \param socket player's socket
*/
void Server::processPlayerLeavesGame(
   QTcpSocket* tcpSocket
)
{
   QMap<QTcpSocket*, Game*>::const_iterator iter =
      mSocketGameMapping.constFind(tcpSocket);

   // remove player from game
   if (iter != mSocketGameMapping.constEnd())
   {
      Player* player = mPlayerSockets[tcpSocket];
      Game* game = iter.value();

      // notify all players in the game that player left
      processBroadcastLeaveGameResponse(player, game);

      // remove player from game
      game->removePlayer(player, tcpSocket);

      // remove socket from socket<->game-mapping
      mSocketGameMapping.remove(tcpSocket);

      // if game is empty, delete game
      if (
            game->getPlayerCount() == 0
         || game->getPlayerCount() == game->getBotCount()
      )
      {
         processRemoveAllBots(game->getId());
         processRemoveGame(game->getId());
      }
      else
      {
         game->broadcastMessage(
            tr("%1 left the game").arg(player->getNick())
         );

         if (player == game->getCreator())
         {
            // 1) pass owner flag
            QList<Player*> players = game->getPlayers();

            foreach(Player* tmpPlayer, players)
            {
               if (!tmpPlayer->isBot())
               {
                  game->setCreator(tmpPlayer);
                  game->broadcastMessage(
                     tr("%1 is the new game owner").arg(tmpPlayer->getNick())
                  );
                  break;
               }
            }

            // 2) notify players about new owner
            foreach(Player* tmpPlayer, players)
            {
               processListGamesRequest(
                  game->getPlayerSockets()->key(tmpPlayer)
               );
            }
         }

         // end game if only 1 player left
         if (game->getPlayerCount() == 1)
         {
            // it is not desired to have a game over condition while
            // the game is not even running :)
            if (game->getState() != Constants::GameStopped)
               game->updateGameoverCondition();
         }
      }
   }
}


//----------------------------------------------------------------------------
/*!
   \param game game to remove
*/
void Server::processRemoveGame(
   int gameId
)
{
   Game* game = mGames.take(gameId);

   if (game)
   {
      emit removeGame(game->getId(), game->getName());
      game->deleteLater();
   }
}


//----------------------------------------------------------------------------
/*!
   \param affected game
*/
void Server::processRemoveAllBots(int gameId)
{
   Game* game = nullptr;
   QTcpSocket* tcpSocket = nullptr;

   QMap<int, Game*>::const_iterator it = mGames.find(gameId);

   if (it != mGames.end())
   {
      game = it.value();

      QList<Player*> players = game->getPlayers();
      foreach (Player* player, players)
      {
         tcpSocket = mPlayerSockets.key(player);

         // remove bot from game
         game->removePlayer(player, tcpSocket);

         if (tcpSocket)
         {
            // notify bot: "you're out"
            sendPacket(
               tcpSocket,
               new LeaveGameResponsePacket(
                  gameId,
                  player->getId()
               )
            );

            // remove socket from socket<->game-mapping
            mSocketGameMapping.remove(tcpSocket);
         }
      }
   }
}


//----------------------------------------------------------------------------
/*!
   \param game game to check for duplicate names
*/
void Server::correctDuplicateGameName(Game *newGame)
{
   bool changed = false;

   QString gameName = newGame->getName();
   QString correctedGameName = gameName;

   bool duplicate = false;
   int iteration = 0;

   do
   {
      duplicate = false;

      foreach(Game* game, mGames)
      {
         if (game->getName() == correctedGameName)
         {
            duplicate = true;
            break;
         }
      }

      if (duplicate)
      {
         changed = true;

         correctedGameName = QString("%1 #%2")
            .arg(gameName)
            .arg(iteration + 1);

         iteration++;
      }
   } while (duplicate);

   // set corrected game name
   if (changed)
   {
      newGame->setName(correctedGameName);
   }
}


//----------------------------------------------------------------------------
/*!
   \param nick player nick
   \return corrected player nick
*/
QString Server::correctDuplicatePlayerName(const QString &nick)
{
   QString correctedNick = nick;

   bool duplicate = false;
   int iteration = 0;

   do
   {
      duplicate = false;

      QMapIterator<QTcpSocket*, Player*> i(mPlayerSockets);
      while (i.hasNext())
      {
         i.next();

         if (correctedNick == i.value()->getNick())
         {
            duplicate = true;
            break;
         }
      }

      if (duplicate)
      {
         correctedNick = QString("%1%2")
            .arg(nick)
            .arg(iteration + 1);

         iteration++;
      }
   }
   while (duplicate);

   return correctedNick;
}


//----------------------------------------------------------------------------
/*!
*/
void Server::displayError(QAbstractSocket::SocketError)
{
   qDebug("Server::displayError: %s", qPrintable(mTcpServer->errorString()));
}
