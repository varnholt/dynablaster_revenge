// header
#include "botclient.h"

// shared
#include "botbombmapitem.h"
#include "extramapitem.h"
#include "mapitem.h"

// Qt
#include <QApplication>
#include <QTcpSocket>

// math
#include <math.h>

// ai
#include "bot.h"
#include "botconstants.h"
#include "botmap.h"
#include "botplayerinfo.h"

// packets
#include "countdownpacket.h"
#include "extramapitemcreatedpacket.h"
#include "extrashakepacket.h"
#include "gameeventpacket.h"
#include "joingamerequestpacket.h"
#include "joingameresponsepacket.h"
#include "keypacket.h"
#include "leavegameresponsepacket.h"
#include "listgamesrequestpacket.h"
#include "listgamesresponsepacket.h"
#include "loginrequestpacket.h"
#include "loginresponsepacket.h"
#include "mapitemcreatedpacket.h"
#include "mapitemdestroyedpacket.h"
#include "mapitemmovepacket.h"
#include "mapitemremovedpacket.h"
#include "messagepacket.h"
#include "playerdisease.h"
#include "playerinfectedpacket.h"
#include "playerkilledpacket.h"
#include "playersynchronizepacket.h"
#include "positionpacket.h"
#include "startgamerequestpacket.h"
#include "startgameresponsepacket.h"
#include "stopgameresponsepacket.h"

// std
#include <stdio.h>
#include <stdlib.h>

void botDebugHandler(QtMsgType type, const char *msg)
{
   switch (type)
   {
      case QtInfoMsg:
         fprintf(stdout, "Info: %s\n", msg);
         abort();
      case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
      case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
      case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
      case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
   }
}



//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
BotClient::BotClient(QObject *parent) :
   QObject(parent),
   mSocket(0),
   mConnected(false),
   mBlockSize(0),
   mBot(0),
   mBotMap(0),
   mGameId(0),
   mAutoJoin(true),
   mAutoStart(false),
   mPlayerId(0),
   mKeysPressed(0),
   mGameJoined(false),
   mSpeed(0.0f),
   mDeltaX(0.0f),
   mDeltaY(0.0f)
{
   // qInstallMsgHandler(botDebugHandler);
   mSocket = new QTcpSocket(this);
}


//-----------------------------------------------------------------------------
/*!
*/
BotClient::~BotClient()
{
   delete mBot;

   // bot accesses player info ptrs, so delete them later
   foreach (PlayerInfo *playerInfo, mPlayerInfo)
      delete playerInfo;

   mPlayerInfo.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::initialize()
{
   // data connections
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
      SIGNAL(disconnected()),
      this,
      SLOT(clientDisconnect())
   );

   connect(
      mSocket,
      SIGNAL(error(QAbstractSocket::SocketError)),
      this,
      SLOT(clientDisconnect())
   );

   // init auto join/start
   initializeAutoJoinStart();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::connectToServer()
{
   mSocket->connectToHost(
      mHost,
      6300
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::reconnect()
{

}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::clientConnect()
{
   mConnected = true;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::clientDisconnect()
{
   mConnected = false;
   qApp->exit();
}


//-----------------------------------------------------------------------------
/*!
   \param host host to set
*/
void BotClient::setHost(const QString& host)
{
   mHost = host;
}


//-----------------------------------------------------------------------------
/*!
   \param player's nick
*/
void BotClient::setNick(const QString& nick)
{
   mNick = nick;
}


//-----------------------------------------------------------------------------
/*!
   \param botmap bot map
*/
void BotClient::setBotMap(BotMap* botmap)
{
   mBotMap = botmap;
}


//----------------------------------------------------------------------------
/*!
*/
void BotClient::initializeAutoJoinStart()
{
   if (mAutoJoin)
   {
      connect(
         mSocket,
         SIGNAL(connected()),
         this,
         SLOT(login())
      );

      connect(
         this,
         SIGNAL(updatePlayerId(int)),
         this,
         SLOT(selectGame())
      );

      connect(
         this,
         SIGNAL(gameSelected()),
         this,
         SLOT(joinGame())
      );
   }

   if (mAutoStart)
   {
      connect(
         this,
         SIGNAL(joinGameResponse(bool)),
         this,
         SLOT(startGame())
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::login()
{
   // reset player id
   mPlayerId = -1;

   // send login packet
   LoginRequestPacket login(mNick, true);
   send(&login);
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::selectGame()
{
   ListGamesRequestPacket packet;
   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to send
*/
void BotClient::send(Packet* packet)
{
   packet->serialize();
   mSocket->write(*packet);
   mSocket->flush();
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream
   \return true if sufficient data was received
*/
bool BotClient::packetAvailable(QDataStream& in)
{
   // blocksize not initialized yet

   if (mBlockSize == 0)
   {
      // not enough data to read blocksize?
      if (mSocket->bytesAvailable() < (int)sizeof(quint16))
        return false;

     // read blocksize
      in >> mBlockSize;

      // qDebug("BotClient::packetAvailable: %d bytes", mBlockSize);
   }

   // enough data?
   return (mSocket->bytesAvailable() >= mBlockSize);
}


//-----------------------------------------------------------------------------
/*!
   \param height map height
   \param width map width
*/
void BotClient::initBotMap(int width, int height)
{
   // delete mBotMap;
   mBotMap = mBot->createMap(width, height);
   mBot->setBotMap(mBotMap);

   // connect botmap
   connect(
      this,
      SIGNAL(mapItemCreated(MapItem*)),
      mBotMap,
      SLOT(createMapItem(MapItem*))
   );

   connect(
      this,
      SIGNAL(mapItemRemoved(MapItem*)),
      mBotMap,
      SLOT(removeMapItem(MapItem*)),
      Qt::DirectConnection
   );
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to bot
*/
Bot *BotClient::getBot() const
{
   return mBot;
}


//-----------------------------------------------------------------------------
/*!
   \param dimensions dimensions to use
*/
void BotClient::createMap(Constants::Dimension dimensions)
{
   int width = 13;
   int height = 11;

   switch (dimensions)
   {
      case Constants::Dimension13x11:
         width = 13;
         height = 11;
         break;
      case Constants::Dimension19x17:
         width = 19;
         height = 17;
         break;
      case Constants::Dimension25x21:
         width = 25;
         height = 21;
         break;
      default:
         break;
   }

   initBotMap(width, height);
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::readData()
{
   QDataStream in(mSocket);
   in.setVersion(QDataStream::Qt_4_8);

   while (packetAvailable(in))
   {
      // block was read completely
      Packet* packet = Packet::deserialize(in);

      if (packet)
      {
         switch(packet->getType())
         {
            case Packet::CREATEGAMERESPONSE:
            {
               // processCreateGameResponse(packet);
               break;
            }

            case Packet::JOINGAMERESPONSE:
            {
               processJoinGameResponse(packet);
               break;
            }

            case Packet::LEAVEGAMERESPONSE:
            {
               processLeaveGameResponse(packet);
               break;
            }

            case Packet::LISTGAMESRESPONSE:
            {
                processListGameResponse(packet);
                break;
            }

            case Packet::LOGINRESPONSE:
            {
               processLoginResponse(packet);
               break;
            }

            case Packet::PLAYERINFECTEDPACKET:
            {
               processPlayerInfected(packet);
               break;
            }

            case Packet::PLAYERKILLED:
            {
               processPlayerKilled(packet);
               break;
            }

            case Packet::DETONATION:
            {
               // processDetonation(packet);
               break;
            }

            case Packet::ERROR:
            {
               break;
            }

            case Packet::POSITION:
            {
               processPosition(packet);
               break;
            }

            case Packet::MAPITEMCREATED:
            {
               processMapItemCreated(packet);
               break;
            }

            case Packet::EXTRAMAPITEMCREATED:
            {
               processExtraMapItemCreated(packet);
               break;
            }

            case Packet::MAPITEMDESTROYED:
            {
               processMapItemDestroyed(packet);
               break;
            }

            case Packet::MAPITEMREMOVED:
            {
               processMapItemRemoved(packet);
               break;
            }

            case Packet::MAPITEMMOVE:
            {
               processMapItemMove(packet);
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

            case Packet::GAMEEVENT:
            {
               processGameEvent(packet);
               break;
            }

            case Packet::MESSAGE:
            {
               // processMessage(packet);
               break;
            }

            case Packet::TIME:
            {
               // processTime(packet);
               break;
            }

            case Packet::COUNTDOWN:
            {
               processCountdown(packet);
               break;
            }

            case Packet::EXTRASHAKE:
            {
               processExtraShake(packet);
               break;
            }

            case Packet::INVALID:
            {
               qWarning("BotClient::data: Packet::INVALID received");
               break;
            }

            default:
               break;
         }

         delete packet;
      }

      mBlockSize = 0;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param bot bot instance
*/
void BotClient::setBot(Bot* bot)
{
   mBot = bot;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::joinGame()
{
   if (mPlayerId != -1)
   {
      if (!isGameJoined())
      {
         if (!mGames.isEmpty())
         {
            // qDebug("BotClient::joinGame");
            JoinGameRequestPacket joinPacket(getGameId());
            send(&joinPacket);

            PlayerSynchronizePacket syncPacket(PlayerSynchronizePacket::LevelLoaded);
            send(&syncPacket);
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::startGame()
{
   StartGameRequestPacket packet(mGameId);
   send(&packet);
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::setGameId(int id)
{
   mGameId = id;
}


//-----------------------------------------------------------------------------
/*!
*/
int BotClient::getGameId() const
{
   return mGameId;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::setAutoJoin(bool enabled)
{
   mAutoJoin = enabled;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::setAutoStart(bool enabled)
{
   mAutoStart = enabled;
}


//-----------------------------------------------------------------------------
/*!
   \param p packet to process
*/
void BotClient::processLoginResponse(Packet* p)
{
   LoginResponsePacket* lrp = (LoginResponsePacket*)p;

   // qDebug("BotClient::processLoginResponse: id: %d", lrp->getId());

   if (lrp->getId() >= 0)
   {
      mPlayerId = lrp->getId();

      // store server configuration data
      setServerConfiguration(lrp->getServerConfiguration());
      getBot()->setServerConfiguration(lrp->getServerConfiguration());
   }

   emit updatePlayerId(mPlayerId);
}


//-----------------------------------------------------------------------------
/*!
   \param p packet to process
*/
void BotClient::processJoinGameResponse(Packet* p)
{
   // qDebug("BotClient::processJoinGameResponse");

   JoinGameResponsePacket* response = (JoinGameResponsePacket*)p;

   BotPlayerInfo* info = 0;
   int id = response->getPlayerId();

   if (!mPlayerInfo.contains(id))
   {
      info = new BotPlayerInfo();
      info->setId(id);
      info->setNick(response->getNick());
      info->setColor(Constants::ColorWhite); // TODO

      mPlayerInfo.insert(id, info);

      // send infomap to all instances interested in them
      // playerInfoMapRequest();
   }

   // ensure it was us who joined as join game responses are
   // broadcasted to all players
   if (id == mPlayerId)
   {
      setGameJoined(true);

      if (info)
         mBot->setPlayerInfo(info);

      mGameId = response->getGameId();

      // select just the first one in the list
      GameInformation gameInfo = mGames[0];
      createMap(gameInfo.getMapDimensions());
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processLeaveGameResponse(Packet *p)
{
   LeaveGameResponsePacket* response = dynamic_cast<LeaveGameResponsePacket*>(p);

   if (response)
   {
      if (response->getPlayerId() == mPlayerId)
      {
         emit remove();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processMapItemCreated(Packet* packet)
{
   MapItem* item = 0;
   MapItemCreatedPacket* micp = (MapItemCreatedPacket*)packet;

   switch (micp->getItemType())
   {
      case MapItem::Bomb:
      {
         int x = micp->getX();
         int y = micp->getY();
         BotPlayerInfo* player = 0;

         item = new BotBombMapItem(
            -1, // we don't know the player id yet
            -1, // we don't know the number of flames yet
            micp->getAppearance(),
            x,
            y
         );

         item->setUniqueId(micp->getUniqueId());

         player = getPlayerInfo(micp->getPlayerId());

         if (player)
         {
            ((BotBombMapItem*)item)->setPlayerId(player->getId());
            ((BotBombMapItem*)item)->setFlameCount(player->getFlameCount());

            /*
            if (DEBUG_BOMB_DROP)
            {
               qDebug(
                  "BotClient::processMapItemCreated: bomb dropped at (%d, %d), "
                  "flames: %d, estimated detonation time: %s",
                  item->getX(),
                  item->getY(),
                  player->getFlameCount(),
                  qPrintable(QTime::currentTime().addSecs(5).toString())
               );
            }
            */
         }

         break;
      }

      default:
      {
         item = new MapItem( (MapItemCreatedPacket*)packet );
         break;
      }
   }

   addMapItem(item);
   emit mapItemCreated(item);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processExtraMapItemCreated(Packet* packet)
{
   ExtraMapItem *extra = new ExtraMapItem( (ExtraMapItemCreatedPacket*)packet );
   addMapItem(extra);
   emit mapItemCreated(extra);
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processMapItemDestroyed(Packet* packet)
{
   MapItemDestroyedPacket* destroyedPacket = (MapItemDestroyedPacket*)packet;
   MapItem* item = getMapItem(destroyedPacket->getUniqueId());

   if (item)
   {
//      qDebug(
//         "BotClient::processMapItemDestroyed: player: %d, intensity: %f",
//         destroyedPacket->getPlayerId(),
//         destroyedPacket->getIntensity()
//      );

      queueObsoleteItem(item);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processMapItemRemoved(Packet* packet)
{
   MapItemRemovedPacket* removedPacket = (MapItemRemovedPacket*)packet;
   MapItem *item = getMapItem(removedPacket->getUniqueId());

   if (item)
   {
      queueObsoleteItem(item);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processMapItemMove(Packet *packet)
{
   MapItemMovePacket* movePacket = (MapItemMovePacket*)packet;
   MapItem *item = getMapItem(movePacket->getMapItemId());

   if (item)
   {
      // in case the move direction is unknown, the item stopped its movement
      // and can be relocated at its nominal position
      if (movePacket->getDirection() == Constants::DirectionUnknown)
      {
         // relocate the bomb on the map
         int x = movePacket->getNominalX();
         int y = movePacket->getNominalY();

         // obsolete as done in the else section
         MapItem* existingItem = mBotMap->getItem(item->getX(), item->getY());
         if (
               existingItem
            && existingItem->getUniqueId() == item->getUniqueId()
         )
         {
            mBotMap->setItem(item->getX(), item->getY(), 0);
         }

         // remap item
         mBotMap->setItem(x, y, item);

         item->setX(x);
         item->setY(y);
      }
      else
      {
         // notify bot about initiated kick
         BotBombMapItem* bomb = dynamic_cast<BotBombMapItem*>(item);

         if (bomb)
         {
            emit bombKicked(
               item->getX(),
               item->getY(),
               movePacket->getDirection(),
               bomb->getFlames()
            );
         }

         // this case will make the bot react as soon the bomb kick animation
         // has been started. by just taking care about the remapping case (see above)
         // the bot is only notified about the result of the bomb kick animation.

         // take bomb out of the map
         /*
         MapItem* existingItem = mBotMap->getItem(item->getX(), item->getY());

         if (
               existingItem
            && existingItem->getUniqueId() == item->getUniqueId()
         )
         {
            mBotMap->setItem(item->getX(), item->getY(), 0);
         }
         */
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param id id of the map id
   \param mapitem map item ptr
*/
MapItem* BotClient::getMapItem(int id) const
{
   mMutex.lock();

   MapItem* item = 0;

   QMap<int,MapItem*>::ConstIterator it= mMapItems.constFind(id);
   if (it != mMapItems.constEnd())
      item = it.value();

   mMutex.unlock();

   return item;
}


//-----------------------------------------------------------------------------
/*!
   \param mapitem item to add
*/
void BotClient::addMapItem(MapItem* mapItem)
{
   mMutex.lock();
   mMapItems.insert(mapItem->getUniqueId(), mapItem);
   mMutex.unlock();
}


//-----------------------------------------------------------------------------
/*!
   \param mapitem item to remove
*/
void BotClient::removeMapItem(MapItem *mapItem)
{
   // delete item and take it from the mapitem-map
   mMutex.lock();
   mMapItems.remove(mapItem->getUniqueId());
   mapItem->deleteLater();
   mMutex.unlock();
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
   \return player info object
*/
BotPlayerInfo* BotClient::getPlayerInfo(int id) const
{
   QMap<int, BotPlayerInfo*>::ConstIterator it = mPlayerInfo.constFind(id);

   if (it != mPlayerInfo.constEnd())
      return it.value();
   else
      return 0;
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
   \return player info object list
*/
QList<BotPlayerInfo *> BotClient::getPlayerInfoList() const
{
   return mPlayerInfo.values();
}


//-----------------------------------------------------------------------------
/*!
   \return player info map
*/
QMap<int, BotPlayerInfo *> *BotClient::getPlayerInfoMap()
{
   return &mPlayerInfo;
}


//-----------------------------------------------------------------------------
/*!
   \param config reference to server configuration
*/
void BotClient::setServerConfiguration(const ServerConfiguration & config)
{
   mServerConfiguration = config;
}


//-----------------------------------------------------------------------------
/*!
   \return reference to server configuration
*/
const ServerConfiguration &BotClient::getServerConfiguration() const
{
   return mServerConfiguration;
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processPosition(Packet* packet)
{
   PositionPacket* posPacket = (PositionPacket*)packet;

   PlayerInfo* playerInfo = getPlayerInfo(posPacket->getPlayerId());

   if (playerInfo)
   {
      playerInfo->setPosition(
         posPacket->getX(),
         posPacket->getY(),
         posPacket->getAngle()
      );

      playerInfo->setDirections(posPacket->getDirections());

      emit updatePlayerPosition(
         posPacket->getPlayerId(),
         posPacket->getX(),
         posPacket->getY(),
         posPacket->getAngle()
      );

      if (playerInfo->getId() == getPlayerId())
      {
         setSpeed(
              posPacket->getDeltaX()
            + posPacket->getDeltaY()
         );

         setDeltaX(posPacket->getDeltaX());
         setDeltaY(posPacket->getDeltaY());
      }

//      emit updatePlayerSpeed(
//         posPacket->getPlayerId(),
//         posPacket->getDeltaX(),
//         posPacket->getDeltaY(),
//         posPacket->getAngle()
//      );
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processStartGameResponse(Packet* packet)
{
   StartGameResponsePacket* response = (StartGameResponsePacket*)packet;

   if (response->isStarted())
      emit gameStarted();
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processGameEvent(Packet* packet)
{
   GameEventPacket* gameEventPacket = (GameEventPacket*)packet;
   int playerId = gameEventPacket->getPlayerId();

   if (playerId != -1)
   {
//      qDebug(
//         "processGameEvent: player: %d, picked extra: %d",
//         playerId,
//         gameEventPacket->getExtraType()
//      );

      if (mPlayerInfo.contains(playerId))
      {
         BotPlayerInfo* playerInfo = mPlayerInfo[playerId];

         switch (gameEventPacket->getExtraType())
         {
            case Constants::ExtraBomb:
               playerInfo->addBomb();
               break;

            case Constants::ExtraFlame:
               playerInfo->addFlame();
               break;

            case Constants::ExtraSpeedup:
               playerInfo->addSpeed();
               break;

            case Constants::ExtraKick:
               playerInfo->setKickEnabled(true);
               break;

            default:
               break;
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processPlayerInfected(Packet *packet)
{
   PlayerInfectedPacket* infectedPacket = (PlayerInfectedPacket*)packet;
   int id = infectedPacket->getPlayerId();

   QMap<int, BotPlayerInfo*>::const_iterator it = mPlayerInfo.find(id);

   if (it != mPlayerInfo.end())
   {
      switch (infectedPacket->getSkullType())
      {
         // infection aborted
         case Constants::SkullReset:
         {
            it.value()->getDisease()->deleteLater();
            break;
         }

         // infection started
         default:
         {
            // create guarded pointer with disease
            QPointer<PlayerDisease> disease = new PlayerDisease();
            disease.data()->setType(infectedPacket->getSkullType());

            // infect player
            it.value()->infect(disease);
            break;
         }
      }
   }
}



//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processPlayerKilled(Packet *packet)
{
   PlayerKilledPacket* killedPacket = (PlayerKilledPacket*)packet;
   int id = killedPacket->getPlayerId();

   QMap<int, BotPlayerInfo*>::const_iterator it = mPlayerInfo.find(id);

   if (it != mPlayerInfo.end())
   {
      it.value()->setKilled(true);
   }

   if (id == mPlayerId)
      mBot->die();
}


//-----------------------------------------------------------------------------
/*!
   \param packet stop game response packet
*/
void BotClient::processStopGameResponse(Packet *packet)
{
   StopGameResponsePacket* stopGamePacket = (StopGameResponsePacket*)packet;

   if (stopGamePacket->getId() == getGameId())
      mBot->idle();
}


//-----------------------------------------------------------------------------
/*!
   \param keysPressed walk direction and bomb drop
*/
void BotClient::walk(qint8 keysPressed)
{
   // keysPressed and mKeysPressed had the same value (up) but the
   // bot was not moving at all.. no further key packets were sent
   // to the server since keysPressed and mKeysPressed have been
   bool sendAgain = false;

   if (keysPressed & Constants::KeyUp)
   {
      if (getDeltaY() >= 0.0f)
         sendAgain = true;
   }
   if (keysPressed & Constants::KeyDown)
   {
      if (getDeltaY() <= 0.0f)
         sendAgain = true;
   }
   if (keysPressed & Constants::KeyLeft)
   {
      if (getDeltaX() >= 0.0f)
         sendAgain = true;
   }
   if (keysPressed & Constants::KeyRight)
   {
      if (getDeltaX() <= 0.0f)
         sendAgain = true;
   }

   if (
         keysPressed != mKeysPressed
      || sendAgain
   )
   {
      mKeysPressed = keysPressed;

      // cool bots don't get their keyboards flipped
      PlayerDisease* disease = getBot()->getPlayerInfo()->getDisease();
      if (disease)
      {
         if (disease->getType() == Constants::SkullKeyboardInvert)
         {
            disease->applyKeyboardInvert(keysPressed);
         }
      }

      // build keypacket
      KeyPacket kPacket(mPlayerId, keysPressed);
      send(&kPacket);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::bomb()
{
   // only place bombs within some field center
   // because the server might be placing the bomb into
   // an undesired field otherwise
   mKeysPressed = 0;
   qint8 keys = 0;
   int x = mBot->getXField();
   int y = mBot->getYField();
//   float relX = mBot->getX() - x;
//   float relY = mBot->getY() - y;
//
//   if (
//         relX > 0.1f && relX < 0.9f
//      && relY > 0.1f && relY < 0.9f
//   )
//   {
      keys = Constants::KeyBomb;

      // mark the field hazardous until the bot re-communicated the
      // field status
      emit markHazardousTemporary(
         x,
         y,
         500,
         mBot->getPlayerInfo()->getFlameCount()
      );
//   }
//   else
//   {
//      // keys = mBot->getBotKeysPressed();
//      keys = mBot->computeWalkKeys();
//   }

   KeyPacket kPacket(mPlayerId, keys);
   send(&kPacket);
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::processListGameResponse(Packet* packet)
{
   ListGamesResponsePacket* list = (ListGamesResponsePacket*)packet;
   mGames = list->getGames();

   emit gameSelected();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::setGameJoined(bool joined)
{
   mGameJoined = joined;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if player joined game
*/
bool BotClient::isGameJoined() const
{
   return mGameJoined;
}


//-----------------------------------------------------------------------------
/*!
   \param speed player speed
*/
void BotClient::setSpeed(float speed)
{
   mSpeed = speed;
}


//-----------------------------------------------------------------------------
/*!
   \return player speed
*/
float BotClient::getSpeed() const
{
   return mSpeed;
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
int BotClient::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param message message to send
   \param receiverId id of the message receiver
*/
void BotClient::sendMessage(
   const QString& message,
   bool finishedTyping,
   int receiverId
)
{
   if (isGameJoined())
   {
      MessagePacket packet(
         -1,
         message,
         finishedTyping,
         receiverId
      );

      send(&packet);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::resetBot()
{
   setDeltaX(0.0f);
   setDeltaY(0.0f);

   // reset keys pressed
   mKeysPressed = 0;

   // reset killed flag for all players
   foreach (BotPlayerInfo *playerInfo, mPlayerInfo)
   {
      playerInfo->setKilled(false);
      playerInfo->reset();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::bugTrack1()
{
   if (mBombTime.elapsed() < 500)
   {
      qDebug("bug");
   }
}


//-----------------------------------------------------------------------------
/*!
*/
int BotClient::getWalkCount() const
{
   return mWalkCount;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::increaseWalkCount()
{
   mWalkCount++;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::resetWalkCount()
{
   mWalkCount = 0;
}


//-----------------------------------------------------------------------------
/*!
   \return delta y
*/
float BotClient::getDeltaY() const
{
   return mDeltaY;
}


//-----------------------------------------------------------------------------
/*!
   \param value delta y
*/
void BotClient::setDeltaY(float value)
{
   mDeltaY = value;
}


//-----------------------------------------------------------------------------
/*!
   \return delta x
*/
float BotClient::getDeltaX() const
{
   return mDeltaX;
}


//-----------------------------------------------------------------------------
/*!
   \param value delta x
*/
void BotClient::setDeltaX(float value)
{
   mDeltaX = value;
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processCountdown(Packet* packet)
{
   CountdownPacket* countdownPacket = (CountdownPacket*)packet;

   int timeLeft = countdownPacket->getTimeLeft();

   /*
   qDebug(
      "BotClient::processCountdown: left: %d",
      timeLeft
   );
   */

   // sync tick
   if (timeLeft == SERVER_PREPARATION_TIME + SERVER_PREPARATION_SYNC_TIME - 1)
   {
      resetBot();
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BotClient::processExtraShake(Packet *packet)
{
   ExtraShakePacket* extraShakePacket = (ExtraShakePacket*)packet;

   int itemId = extraShakePacket->getMapItemUniqueId();

   /*
   qDebug(
      "BotClient::processExtraShake: item %d contains an extra",
      itemId
   );
   */

   emit extraShake(itemId);
}


//-----------------------------------------------------------------------------
/*!
   \param item item that is queued to be deleted later
*/
void BotClient::queueObsoleteItem(MapItem *item)
{
   mMutex.lock();
   mObsoleteMapItems << item;
   mMutex.unlock();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotClient::clearObsoleteItems()
{
   mMutex.lock();
   mObsoleteMapItems.clear();
   mMutex.unlock();
}



/*
   deleteObsoleteMapItems is accessed by another thread, so every member
   used in there is protected by a mutex in order to avoid race conditions

*/


//-----------------------------------------------------------------------------
/*!
   \param item item that is queued to be deleted later
*/
void BotClient::deleteObsoleteMapItems()
{
   mMutex.lock();
   QQueue<MapItem*> items = mObsoleteMapItems;
   mMutex.unlock();

   foreach (MapItem* item, items)
   {
      emit mapItemRemoved(item);
      removeMapItem(item);
   }

   clearObsoleteItems();
}

