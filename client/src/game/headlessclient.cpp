#include "headlessclient.h"

// bomberman shared
#include "constants.h"

// client and shared
#include "countdownpacket.h"
#include "creategamerequestpacket.h"
#include "creategameresponsepacket.h"
#include "errorpacket.h"
#include "extramapitem.h"
#include "extramapitemcreatedpacket.h"
#include "extrashakepacket.h"
#include "gameeventpacket.h"
#include "gamesettings.h"
#include "gamestatemachine.h"
#include "gamestatspacket.h"
#include "joingamerequestpacket.h"
#include "joingameresponsepacket.h"
#include "keypacket.h"
#include "leavegamerequestpacket.h"
#include "leavegameresponsepacket.h"
#include "levels/level.h"
#include "listgamesrequestpacket.h"
#include "listgamesresponsepacket.h"
#include "positioninterpolation.h"
#include "positionpacket.h"
#include "mapitemcreatedpacket.h"
#include "mapitemdestroyedpacket.h"
#include "mapitemmovepacket.h"
#include "mapitemremovedpacket.h"
#include "messagepacket.h"
#include "detonationpacket.h"
#include "loginrequestpacket.h"
#include "loginresponsepacket.h"
#include "playerdisease.h"
#include "playerinfectedpacket.h"
#include "playerkilledpacket.h"
#include "playerstats.h"
#include "playersynchronizepacket.h"
#include "startgamerequestpacket.h"
#include "startgameresponsepacket.h"
#include "stopgamerequestpacket.h"
#include "stopgameresponsepacket.h"
#include "timepacket.h"
#include "tools/stream.h"
#include "math/vector.h"

// qt
#include <QDir>
#include <QHostAddress>
#include <QKeyEvent>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QThread>


// static variables
HeadlessClient* HeadlessClient::mInstance = 0;


//-----------------------------------------------------------------------------
/*!
*/
HeadlessClient::HeadlessClient(/*const QString& host, const QString& nick*/)
   : mKeysPressed(0),
     mBombReleased(true),
     mSocket(0),
     mBlockSize(0),
     mId(-1),
     mGameId(-1),
     mDead(true),
     mConnected(false),
     mLoginAfterConnect(false),
     mIngameMessagingActive(false)
{
   mInstance = this;   

   connect(
      GameStateMachine::getInstance(),
      SIGNAL(stateChanged()),
      this,
      SLOT(gameStateChanged())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::initialize()
{
   mSocket = new QTcpSocket(this);

   connect(
      mSocket,
      SIGNAL(error(QAbstractSocket::SocketError)),
      this,
      SLOT(socketError(QAbstractSocket::SocketError))
   );

   connect(
      mSocket,
      SIGNAL(readyRead()),
      this,
      SLOT(readData())
   );

   connect(
      mSocket,
      SIGNAL(connected()),
      this,
      SLOT(clientConnect())
   );

   connect(
      mSocket,
      SIGNAL(connected()),
      this,
      SIGNAL(connected())
   );

   connect(
      mSocket,
      SIGNAL(disconnected()),
      this,
      SLOT(clientDisconnect())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
HeadlessClient::~HeadlessClient()
{
   mInstance= 0;
}


//-----------------------------------------------------------------------------
/*!
   \return singleton instance of client
*/
HeadlessClient *HeadlessClient::getInstance()
{
   return mInstance;
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to list of games
*/
QList<GameInformation>* HeadlessClient::getGames() const
{
   return &mGames;
}


//-----------------------------------------------------------------------------
/*!
   \param id game id
*/
void HeadlessClient::setGameId(int id)
{
   mGameId = id;
}


//-----------------------------------------------------------------------------
/*!
   \return game id
*/
int HeadlessClient::getGameId() const
{
   return mGameId;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if game is valid
*/
bool HeadlessClient::isGameIdValid() const
{
   return getGameId() != -1;
}


//-----------------------------------------------------------------------------
/*!
   \return game information
*/
GameInformation *HeadlessClient::getGameInformation(int id) const
{
   GameInformation* gameInfo = 0;

   for (int i = 0; i < mGames.size(); i++)
   {
      if (mGames.at(i).getId() == id)
      {
         gameInfo = &mGames[i];
         break;
      }
   }

   return gameInfo;
}


//-----------------------------------------------------------------------------
/*!
   \return game information
*/
GameInformation* HeadlessClient::getCurrentGameInformation() const
{
   return getGameInformation(getGameId());
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
*/
void HeadlessClient::setPlayerId(int id)
{
   mId = id;
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
int HeadlessClient::getPlayerId() const
{
   return mId;
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if player is owner
*/
bool HeadlessClient::isPlayerOwner() const
{
   bool owner = false;
   GameInformation* info = getCurrentGameInformation();

   if (info)
   {
      owner = info->getCreatorId() == getPlayerId();
   }

   return owner;
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to send
*/
void HeadlessClient::send(Packet *packet)
{
   packet->serialize();
   mSocket->write(*packet);
   mSocket->flush();
}


//-----------------------------------------------------------------------------
/*!
   \param host host name
*/
void HeadlessClient::setHost(const QString& host)
{
   mHost = host;
}


//-----------------------------------------------------------------------------
/*!
   \return host name
*/
const QString &HeadlessClient::getHost() const
{
   return mHost;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::connectToServer()
{
   mSocket->connectToHost(
      getHost(),
      6300
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::clientConnect()
{
   qDebug("HeadlessClient::clientConnect()");

   setConnected(true);

   if (mLoginAfterConnect)
   {
      login(getNick());
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::clientDisconnect()
{
   if (isConnected())
   {
      // 1) show menu
      // 2) open main screen
      // 3) display disconnected message

      // we were disconnected
      emit disconnected();
   }
   else
   {
      // we had to disconnect first before reconnecting to
      // another server
      loginRequest(getHost(), getNick());
   }

   resetGameData();
   resetClientState();

   qDebug() << QString("disconnected");

   QMap<int,MapItem*>::Iterator it= mMapItems.begin();
   while (it!= mMapItems.end())
   {
      MapItem *item= *it;
      it= mMapItems.erase(it);
      delete item;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream
   \return true if sufficient data was received
*/
bool HeadlessClient::packetAvailable(QDataStream& in)
{
   // blocksize not initialized yet
   if (mBlockSize == 0)
   {
      // not enough data to read blocksize?
      if (mSocket->bytesAvailable() < (int)sizeof(quint16))
        return false;

     // read blocksize
      in >> mBlockSize;
   }

   // enough data?
   return (mSocket->bytesAvailable() >= mBlockSize);
}


//-----------------------------------------------------------------------------
/*!
   \param id id of the map id
   \param mapitem map item ptr
*/
MapItem* HeadlessClient::getMapItem(int id) const
{
   MapItem* item = 0;

   QMap<int,MapItem*>::ConstIterator it= mMapItems.constFind(id);
   if (it != mMapItems.constEnd())
      item = it.value();

   return item;
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processCreateGameResponse(Packet* packet)
{
   CreateGameResponsePacket* response = (CreateGameResponsePacket*)packet;

   // extract game information
   GameInformation gameInformation = response->getGameInformation();
   int gameId = gameInformation.getId();
   int playerId = gameInformation.getCreatorId();
   bool success = false;

   if (gameId > -1)
   {
      qDebug(
         "BombermanClient::data(): game %d created",
         gameId
      );

      // update game information
      GameInformation* oldGameInformation = getGameInformation(gameId);
      if (oldGameInformation)
      {
         *oldGameInformation = gameInformation;
      }
      else
      {
         mGames << gameInformation;
      }

      success = true;
   }
   else
   {
      qDebug("BombermanClient::data(): game create request failed");
   }

   emit createGameResponse(
      success,
      gameId,
      playerId == getPlayerId()
   );
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processJoinGameResponse(Packet* packet)
{
   JoinGameResponsePacket* response = (JoinGameResponsePacket*)packet;

   if (response->isSuccessful())
   {
      PlayerInfo* info = 0;

      // ensure it was us who joined as join game responses are
      // broadcasted to all players
      if (response->getPlayerId() == getPlayerId())
      {
         // store current player info
         setCurrentPlayerInfo(info);

         // store game id
         setGameId(response->getGameId());
      }

      // fake level loaded
      levelLoaded("meh");
   }
   else if (response->getPlayerId() == getPlayerId())
   {
      // boo :(
      emit socketError();
   }
}


//-----------------------------------------------------------------------------
/*!
   \param path path of the level that has been loaded
*/
void HeadlessClient::levelLoaded(const QString& /*path*/)
{
   PlayerSynchronizePacket packet(PlayerSynchronizePacket::LevelLoaded);
   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processLoginResponse(Packet* packet)
{
   LoginResponsePacket *login= (LoginResponsePacket*)packet;
   setPlayerId(login->getId());
   emit loginResponse(true);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processPlayerKilled(Packet* packet)
{
   PlayerKilledPacket *kill= (PlayerKilledPacket*)packet;

   // check if own player was killed
   if (kill->getPlayerId() == getPlayerId())
      emit rumble(0.5, 2000);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processError(Packet* packet)
{
   ErrorPacket* errorPacket= (ErrorPacket*)packet;

   switch (errorPacket->getErrorType())
   {
      // if sync fails, the player is kicked out of the game
      case Constants::ErrorSyncTimeout:
      {
         // resetGameData();
         break;
      }

      default:
      {
         break;
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processGameEvent(Packet *packet)
{
   GameEventPacket* response = (GameEventPacket*)packet;

   switch (response->getGameEvent())
   {
      case GameEventPacket::ExtraCollected:
      {
         // rumble just a little on extra collect
         if (response->getPlayerId() == getPlayerId())
            emit rumble(0.2f, 500);

         break;
      }

      default:
         break;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param info current player info
*/
void HeadlessClient::setCurrentPlayerInfo(PlayerInfo* info)
{
   mCurrentPlayerInfo = info;
}


//-----------------------------------------------------------------------------
/*!
   \return player info ptr
*/
PlayerInfo *HeadlessClient::getCurrentPlayerInfo() const
{
   return mCurrentPlayerInfo;
}


//-----------------------------------------------------------------------------
/*!
   \return current message
*/
const QString &HeadlessClient::getMessage() const
{
   return mMessage;
}


//-----------------------------------------------------------------------------
/*!
   \param message message to set
*/
void HeadlessClient::setMessage(const QString &message)
{
   mMessage = message;
}


//-----------------------------------------------------------------------------
/*!
   \param connected connected state
*/
void HeadlessClient::setConnected(bool connected)
{
   mConnected = connected;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if client is connected
*/
bool HeadlessClient::isConnected() const
{
   return mConnected;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::processStartGameResponse(Packet* packet)
{
   StartGameResponsePacket* response = (StartGameResponsePacket*)packet;

   qDebug(
      "BombermanClient::data: game %d started",
      response->getId()
   );

   GameStateMachine::getInstance()->setState(Constants::GameActive);

   mDead = false;

   emit gameStarted();

   // resend keys pressed on start game
   //	fix for 0000032: Player movement at start of each round
   //    If you press a movement button during the countdown at the start of a
   //    round and hold it, the player does not move when the round begins.
   sendKeysPressedPacket();
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processStopGameResponse(Packet* packet)
{
   StopGameResponsePacket* response = (StopGameResponsePacket*)packet;

   qDebug(
      "BombermanClient::data: game %d stopped, all rounds finished: %d",
      response->getId(),
      response->isFinished()
   );

   GameStateMachine::getInstance()->setState(Constants::GameStopped);

   mDead = true;
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processMessage(Packet* packet)
{
   MessagePacket* messagePacket = (MessagePacket*)packet;

   emit messageReceived(
      messagePacket->getSenderId(),
      messagePacket->getMessage(),
      messagePacket->isTypingFinished()
   );

   if (messagePacket->isTypingFinished())
   {
      // omit broadcast messages
      if (
            messagePacket->getSenderId() != -1
         && messagePacket->getSenderId() != getPlayerId()
      )
      {
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processCountdown(Packet* /*packet*/)
{
   GameStateMachine::getInstance()->setState(Constants::GamePreparing);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::processPacket(Packet* packet)
{
   if (packet)
   {
      switch(packet->getType())
      {
         case Packet::COUNTDOWN:
         {
            processCountdown(packet);
            break;
         }

         case Packet::CREATEGAMERESPONSE:
         {
            processCreateGameResponse(packet);
            break;
         }

         case Packet::DETONATION:
         {
            // processDetonation(packet);
            break;
         }

         case Packet::ERROR:
         {
            processError(packet);
            break;
         }

         case Packet::EXTRAMAPITEMCREATED:
         {
            // processExtraMapItemCreated(packet);
            break;
         }

         case Packet::GAMESTATS:
         {
            // processGameStats(packet);
            break;
         }

         case Packet::GAMEEVENT:
         {
            processGameEvent(packet);
            break;
         }

         case Packet::JOINGAMERESPONSE:
         {
            processJoinGameResponse(packet);
            break;
         }

         case Packet::KEY:
         {
            qWarning("BombermanClient::data(): wrong direction!");
            break;
         }

         case Packet::LISTGAMESRESPONSE:
         {
             // processListGameResponse(packet);
             break;
         }

         case Packet::LOGINRESPONSE:
         {
            processLoginResponse(packet);
            break;
         }

         case Packet::PLAYERINFECTEDPACKET:
         {
            // processPlayerInfected(packet);
            break;
         }

         case Packet::PLAYERKILLED:
         {
            processPlayerKilled(packet);
            break;
         }

         case Packet::POSITION:
         {
            // processPosition(packet);
            break;
         }

         case Packet::LEAVEGAMERESPONSE:
         {
            // processLeaveGameResponse(packet);
            break;
         }

         case Packet::MAPITEMCREATED:
         {
            // processMapItemCreated(packet);
            break;
         }

         case Packet::MAPITEMDESTROYED:
         {
            // processExtraMapItemDestroyed(packet);
            break;
         }

         case Packet::MAPITEMMOVE:
         {
            // processMapItemMove(packet);
            break;
         }

         case Packet::MAPITEMREMOVED:
         {
            // processMapItemRemoved(packet);
            break;
         }

         case Packet::MESSAGE:
         {
            processMessage(packet);
            break;
         }

         case Packet::STARTGAMERESPONSE:
         {
            processStartGameResponse(packet);
            break;
         }

         case Packet::STOPGAMERESPONSE:
         {
            processStopGameResponse(packet);
            break;
         }

         case Packet::TIME:
         {
            // processTime(packet);
            break;
         }

         case Packet::EXTRASHAKE:
         {
            // processExtraShake(packet);
            break;
         }

         case Packet::INVALID:
         {
            qWarning("BombermanClient::data: Packet::INVALID received");
            break;
         }

         default:
            break;
      }

      delete packet;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void HeadlessClient::readData()
{
   QDataStream in(mSocket);
   in.setVersion(QDataStream::Qt_4_6);

   while (packetAvailable(in))
   {
      // block was read completely
      Packet* packet = Packet::deserialize(in);

      processPacket(packet);

      mBlockSize = 0;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param event keypressed event
*/
void HeadlessClient::keyPressed(QKeyEvent* event)
{
   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   bool controlKey =
         event->key() == controllerSettings->getUpKey()
      || event->key() == controllerSettings->getDownKey()
      || event->key() == controllerSettings->getLeftKey()
      || event->key() == controllerSettings->getRightKey()
      || event->key() == controllerSettings->getBombKey();

   if ((controlKey && !event->isAutoRepeat()) || !controlKey)
   {
      processKeyPressed(event->key());
   }
}


//-----------------------------------------------------------------------------
/*!
   \param event keyreleased event
*/
void HeadlessClient::keyReleased(QKeyEvent* event)
{
   if (!event->isAutoRepeat())
      processKeyReleased(event->key());
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::sendKeysPressedPacket()
{
   // build keypacket
   // qDebug("BombermanClient::keyPressed: sending keypacket");
   KeyPacket kPacket(0, mKeysPressed);
   send(&kPacket);
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::removeBombKeyFlag()
{
   // in any case remove the bomb key from the key combination
   // to avoid unwanted bomb dropping
   if ((mKeysPressed & Constants::KeyBomb) == Constants::KeyBomb)
      mKeysPressed &= ~Constants::KeyBomb;
}


//-----------------------------------------------------------------------------
/*!
   \param key key to process
   \param text text to type
*/
void HeadlessClient::processKeyPressed(int key)
{
   // care about movement
   bool moved = false;

   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   if (key == controllerSettings->getUpKey())
   {
      mKeysPressed |= Constants::KeyUp;
      moved = true;
   }
   else if (key == controllerSettings->getDownKey())
   {
      mKeysPressed |= Constants::KeyDown;
      moved = true;
   }
   else if (key == controllerSettings->getLeftKey())
   {
      mKeysPressed |= Constants::KeyLeft;
      moved = true;
   }
   else if (key == controllerSettings->getRightKey())
   {
      mKeysPressed |= Constants::KeyRight;
      moved = true;
   }
   else if (key == controllerSettings->getBombKey())
   {
      mBombReleased = false;
      mKeysPressed |= Constants::KeyBomb;
      moved = true;
   }

   if (moved)
   {
      if (!mDead)
      {
         sendKeysPressedPacket();
         removeBombKeyFlag();
      }
   }
}


void HeadlessClient::processKeyReleased(int key)
{
   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   if (key == controllerSettings->getUpKey())
   {
      mKeysPressed &= ~Constants::KeyUp;
   }
   else if (key == controllerSettings->getDownKey())
   {
      mKeysPressed &= ~Constants::KeyDown;
   }
   else if (key == controllerSettings->getLeftKey())
   {
      mKeysPressed &= ~Constants::KeyLeft;
   }
   else if (key == controllerSettings->getRightKey())
   {
      mKeysPressed &= ~Constants::KeyRight;
   }
   else if (
         key == controllerSettings->getBombKey()
      // || key == Qt::Key_Space
      // || key == Qt::Key_Control
   )
   {
      // next bomb may be dropped
      mBombReleased = true;
      mKeysPressed &= ~Constants::KeyBomb;
   }

      /*
         switch (key)
         {
            case Qt::Key_Up:
               mKeysPressed &= ~Constants::KeyUp;
               break;

            case Qt::Key_Down:
               mKeysPressed &= ~Constants::KeyDown;
               break;

            case Qt::Key_Left:
               mKeysPressed &= ~Constants::KeyLeft;
               break;

            case Qt::Key_Right:
               mKeysPressed &= ~Constants::KeyRight;
               break;

            case Qt::Key_Control:
            {
               // next bomb may be dropped
               mBombReleased = true;
               mKeysPressed &= ~Constants::KeyBomb;
            }

            default:
               break;
         }
      */

   if (!mDead)
   {
      // build keypacket
      // qDebug("BombermanClient::keyPressed: sending keypacket");
      KeyPacket kPacket(0, mKeysPressed);
      send(&kPacket);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::debugKeyboardInput()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::resetClientState()
{
   setConnected(false);
   mDead = true;

   // we're disconnected
   emit loginResponse(false);
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::resetGameData()
{
   mGames.clear();
   setGameId(-1);
   GameStateMachine::getInstance()->setState(Constants::GameStopped);
}


/*
   server workflow

   1) LoginRequest, wait for
      LoginResponse

   2) ListGameRequest, wait for
      ListGameResponse

   3) CreateGameRequest if required, wait for
      CreateGameResponse

   4) JoinGameRequest, wait for
      JoinGameResponse (for each player in the game)

   5) StartGameRequest, wait for
      StartGameResponse

*/


//-----------------------------------------------------------------------------
/*!
   \param nick nick to use
   \param color color to use
*/
void HeadlessClient::login(const QString& nick)
{
   setNick(nick);
   LoginRequestPacket login(nick, false);
   send(&login);
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::stopGame()
{
   stopGame(getGameId());
}


//-----------------------------------------------------------------------------
/*!
   \param player's nick
*/
void HeadlessClient::setNick(const QString& nick)
{
   mNick = nick;
}


//-----------------------------------------------------------------------------
/*!
   \return player's nick
*/
const QString &HeadlessClient::getNick() const
{
   return mNick;
}


//-----------------------------------------------------------------------------
/*!
   \param message message to send
   \param receiverId id of the message receiver
*/
void HeadlessClient::sendMessage(
   const QString& message,
   bool finishedTyping,
   int receiverId
)
{
   if (!finishedTyping)
      setMessage(message);

   MessagePacket packet(
      -1,
      message,
      finishedTyping,
      receiverId
   );

   send(&packet);

   if (finishedTyping)
   {
      setMessage("");
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::listGames()
{
   if (
         isConnected()
      && (GameStateMachine::getInstance()->getState() != Constants::GameActive)
   )
   {
      ListGamesRequestPacket packet;
      send(&packet);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::createGameAutomatic()
{
   createGame(
      "coding",
      Level::getLevelDirectoryName(Level::LevelCastle),
      1,
      1800,
      5,
      !true,
      !true,
      !true,
      !true,
      true,
      Constants::Dimension13x11
   );
}


//-----------------------------------------------------------------------------
/*!
   \param name game's name
*/
void HeadlessClient::createGame(
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
)
{
   bool dryRun =
      GameSettings::getInstance()->getDevelopmentSettings()->isDryRunEnabled();

   if (dryRun)
      rounds = 999;

   CreateGameRequestPacket packet(
      name,
      level,
      rounds,
      duration,
      maxPlayers,
      extraBombEnabled,
      extraFlameEnabled,
      extraSpeedupEnabled,
      extraKickEnabled,
      extraSkullsEnabled,
      dimension
   );

   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::joinGame(int game)
{
   JoinGameRequestPacket packet(game);
   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
   \param game game to start
*/
void HeadlessClient::startGame(int game)
{
   GameStateMachine::getInstance()->setState(Constants::GamePreparing);

   StartGameRequestPacket packet(game);
   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
   \param game game to stop
*/
void HeadlessClient::stopGame(int game)
{
   StopGameRequestPacket request(game);
   send(&request);
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::setLoginAfterConnect(bool loginAfterConnect)
{
   mLoginAfterConnect = loginAfterConnect;
}


//-----------------------------------------------------------------------------
/*!
   \param host host to use
   \param nick nick to use
*/
void HeadlessClient::loginRequest(
   const QString& host,
   const QString& nick
)
{
   setHost(host);
   setNick(nick);

   // we first have to disconnect from the current server
   // because its hostname obviously differs from ours
   if (
         mSocket->state() >= QAbstractSocket::ConnectedState
      && mSocket->peerName().toLower() != host.toLower()
   )
   {
      qDebug(
         "BombermanClient::loginRequest: connect to another host: %s -> %s",
         qPrintable(mSocket->peerName().toLower()),
         qPrintable(host.toLower())
      );

      setConnected(false);
      mSocket->disconnectFromHost();
   }
   else
   {
      if (mSocket->state() < QAbstractSocket::ConnectedState)
      {
         setConnected(false);
         connectToServer();
         mLoginAfterConnect = true;
      }
      else
      {
         // we are already connected, so we just need to login
         login(nick);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::leaveGameRequest()
{
   LeaveGameRequestPacket packet(getGameId(), getPlayerId());
   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::gameListRequest()
{
   listGames();
}


//-----------------------------------------------------------------------------
/*!
   \return game mode
*/
Constants::GameMode HeadlessClient::getGameMode() const
{
   return mGameMode;
}


//-----------------------------------------------------------------------------
/*!
   \param mode game mode
*/
void HeadlessClient::setGameMode(const Constants::GameMode &mode)
{
   mGameMode = mode;
}


//-----------------------------------------------------------------------------
/*!
*/
void HeadlessClient::gameStateChanged()
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      //
   }
}


//-----------------------------------------------------------------------------
/*!
   \param error socket error
*/
void HeadlessClient::socketError(QAbstractSocket::SocketError /*error*/)
{
   qDebug("socket error: %s",
      qPrintable(mSocket->errorString())
   );

   emit socketError();
}

