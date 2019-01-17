#include "dummyplayer.h"
#include "loginrequestpacket.h"
#include "joingamerequestpacket.h"
#include "constants.h"
#include <QHostAddress>

DummyPlayer::DummyPlayer(const QString& nickName, int gameId)
 : QTcpSocket()
 , mNick(nickName)
 , mGameId(gameId)
{
   connect(this, SIGNAL(connected()), this, SLOT(onConnect()));
   connect(this, SIGNAL(disconnected()), this, SLOT(onDisonnect()));
   connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onDisonnect()));
   connect(this, SIGNAL(readyRead()), this, SLOT(onRead()));
   connectToHost(QHostAddress("127.0.0.1"), SERVER_PORT);
}

DummyPlayer::~DummyPlayer()
{
}

void DummyPlayer::send(Packet& pack)
{
   pack.serialize();
   write(pack);
   flush();
}

void DummyPlayer::onConnect()
{
   LoginRequestPacket login(mNick, false);
   send(login);

   JoinGameRequestPacket join(mGameId);
   send(join);
}

void DummyPlayer::onDisconnect()
{
   deleteLater();
}

void DummyPlayer::onRead()
{
   QByteArray data= readAll();
   Q_UNUSED(data);
}

/*
void Server::unitTestAddPlayer()
{
   if (!mGames.isEmpty())
   {
      QTcpSocket* tcpSocket = new QTcpSocket();

      // create new player
      Player* player = new Player(mPlayerId++);
      mPlayerSockets.insert(tcpSocket, player);

      Game* game = mGames[1];

      // init player attributes
      player->setLoggedIn(true);
      player->setNick(QString::number(qrand() % 99999999));

      // assign player color
      player->setColor(
         game->getColorForNextPlayer()
         );

      mSocketGameMapping.insert(
         tcpSocket,
         game
         );

      game->joinGame(player, tcpSocket);

      // signal new plyaer to gui
      emit newPlayer(player->getId(), player->getNick());
   }
}
*/
