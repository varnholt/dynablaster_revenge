#include "bombermanclient.h"

// bomberman shared
#include "constants.h"

// client and shared
#include "countdownpacket.h"
#include "creategamerequestpacket.h"
#include "creategameresponsepacket.h"
#include "detonationpacket.h"
#include "errorpacket.h"
#include "extramapitem.h"
#include "extramapitemcreatedpacket.h"
#include "extrashakepacket.h"
#include "extrashakepackethandler.h"
#include "gameeventpacket.h"
#include "gamehelptexts.h"
#include "gameplayback.h"
#include "gamesettings.h"
#include "gamestatemachine.h"
#include "gamestatspacket.h"
#include "helpmanager.h"
#include "joingamerequestpacket.h"
#include "joingameresponsepacket.h"
#include "keypacket.h"
#include "leavegamerequestpacket.h"
#include "leavegameresponsepacket.h"
#include "levels/level.h"
#include "listgamesrequestpacket.h"
#include "listgamesresponsepacket.h"
#include "loginrequestpacket.h"
#include "loginresponsepacket.h"
#include "mapitemcreatedpacket.h"
#include "mapitemdestroyedpacket.h"
#include "mapitemmovepacket.h"
#include "mapitemremovedpacket.h"
#include "math/vector.h"
#include "messagepacket.h"
#include "playerdisease.h"
#include "playerinfectedpacket.h"
#include "playerkilledpacket.h"
#include "playerstats.h"
#include "playersynchronizepacket.h"
#include "positioninterpolation.h"
#include "positionpacket.h"
#include "soundmanager.h"
#include "startgamerequestpacket.h"
#include "startgameresponsepacket.h"
#include "stopgamerequestpacket.h"
#include "stopgameresponsepacket.h"
#include "stringutils.h"
#include "timepacket.h"
#include "tools/stream.h"

#include <format>
#include <string>

// ai
#include "botfactory.h"

// server
#include "server.h"

// qt
#include <QDir>
#include <QKeyEvent>
// SDL
#include <SDL3_net/SDL_net.h>

#include <cstdint>

// static variables
BombermanClient* BombermanClient::mInstance = nullptr;

//-----------------------------------------------------------------------------
/*!
 */
BombermanClient::BombermanClient(/*const std::string& host, const std::string& nick*/)
    : mKeysPressed(0),
      mBombReleased(true),
      mSocket(nullptr),
      mAddress(nullptr),
      mBlockSize(0),
      mId(-1),
      mGameId(-1),
      mDead(true),
      mConnected(false),
      mLoginAfterConnect(false),
      mServer(nullptr),
      mPositionInterpolation(nullptr),
      mIngameMessagingActive(false),
      mMainMenuActive(false),
      mBotFactory(nullptr)
{
   mInstance = this;

   mPositionInterpolation = new PositionInterpolation(this);

   connect(mPositionInterpolation, SIGNAL(bounce()), SoundManager::getInstance(), SLOT(playSoundBombBounce()));

   connect(GameStateMachine::getInstance(), SIGNAL(stateChanged()), this, SLOT(gameStateChanged()));

   mBotFactory = new BotFactory(this);

   // enable game playback
   initializePlayback();
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::initialize()
{
   mPollTimer.timeoutSignal.connect([this]() { poll(); });
   mPollTimer.start(16);

   // interpolation
   connect(
      this,
      SIGNAL(moveMapItem(MapItem*, Constants::Direction, float, int, int)),
      mPositionInterpolation,
      SLOT(moveMapItem(MapItem*, Constants::Direction, float, int, int))
   );
}

//-----------------------------------------------------------------------------
/*!
 */
BombermanClient::~BombermanClient()
{
   mInstance = nullptr;
   clearPlayerInfoMap();

   delete mServer;
}

//-----------------------------------------------------------------------------
/*!
   \return singleton instance of client
*/
BombermanClient* BombermanClient::getInstance()
{
   return mInstance;
}

//-----------------------------------------------------------------------------
/*!
   \return ptr to list of games
*/
std::vector<GameInformation>* BombermanClient::getGames() const
{
   return &mGames;
}

//-----------------------------------------------------------------------------
/*!
   \param id game id
*/
void BombermanClient::setGameId(int id)
{
   mGameId = id;
}

//-----------------------------------------------------------------------------
/*!
   \return game id
*/
int BombermanClient::getGameId() const
{
   return mGameId;
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if game is valid
*/
bool BombermanClient::isGameIdValid() const
{
   return getGameId() != -1;
}

//-----------------------------------------------------------------------------
/*!
   \return game information
*/
GameInformation* BombermanClient::getGameInformation(int id) const
{
   GameInformation* gameInfo = nullptr;

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
GameInformation* BombermanClient::getCurrentGameInformation() const
{
   return getGameInformation(getGameId());
}

//-----------------------------------------------------------------------------
/*!
   \param id player id
*/
void BombermanClient::setPlayerId(int id)
{
   mId = id;
}

//-----------------------------------------------------------------------------
/*!
   \return player id
*/
int BombermanClient::getPlayerId() const
{
   return mId;
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if player is owner
*/
bool BombermanClient::isPlayerOwner() const
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
   \param playerId player id
   \return player color
*/
Constants::Color BombermanClient::getColor(int playerId) const
{
   Constants::Color color = Constants::ColorWhite;

   PlayerInfo* info = getPlayerInfo(playerId);

   if (info)
      color = info->getColor();

   return color;
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to send
*/
void BombermanClient::send(Packet* packet)
{
   packet->serialize();

   if (mSocket)
   {
      NET_WriteToStreamSocket(mSocket, packet->constData(), static_cast<int>(packet->size()));
   }
}

//-----------------------------------------------------------------------------
/*!
   \param host host name
*/
void BombermanClient::setHost(const std::string& host)
{
   mHost = host;
}

//-----------------------------------------------------------------------------
/*!
   \return host name
*/
const std::string& BombermanClient::getHost() const
{
   return mHost;
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::connectToServer()
{
   mAddress = NET_ResolveHostname(getHost().c_str());

   if (!mAddress)
   {
      reportConnectionError(SDL_GetError());
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::clientConnect()
{
   qDebug("BombermanClient::clientConnect()");

   setConnected(true);

   if (mLoginAfterConnect)
   {
      login(getNick());
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::clientDisconnect()
{
   if (isConnected())
   {
      // 1) show menu
      // 2) open main screen
      // 3) display disconnected message

      // we were disconnected
      disconnectedSignal();
   }
   else
   {
      // we had to disconnect first before reconnecting to
      // another server
      loginRequest(getHost(), getNick());
   }

   resetGameData();
   resetClientState();

   qDebug("disconnected");

   auto it = mMapItems.begin();
   while (it != mMapItems.end())
   {
      MapItem* item = it->second;
      it = mMapItems.erase(it);
      delete item;
   }
}

//-----------------------------------------------------------------------------
/*!
   tear down whatever connection attempt or connection is in progress
*/
void BombermanClient::disconnectFromServer()
{
   if (mAddress)
   {
      NET_UnrefAddress(mAddress);
      mAddress = nullptr;
   }

   if (mSocket)
   {
      NET_DestroyStreamSocket(mSocket);
      mSocket = nullptr;
   }

   clientDisconnect();
}

//-----------------------------------------------------------------------------
/*!
  \param reason low-level failure reason, from SDL_GetError()
*/
void BombermanClient::reportConnectionError(const char* reason)
{
   std::string message = TEXT_ERROR_NETWORK_GENERAL;

   if (reason && *reason)
   {
      message += std::format(" ({})", reason);
   }

   HelpManager::getInstance()->addMessage("", message, Constants::HelpSeverityError);
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream
   \return true if sufficient data was received
*/
bool BombermanClient::packetAvailable()
{
   // blocksize not initialized yet
   if (mBlockSize == 0)
   {
      // not enough data to read blocksize?
      if (mBuffer.bytesAvailable() < sizeof(uint16_t))
         return false;

      // read blocksize
      BinaryReader sizeReader = mBuffer.reader();
      sizeReader >> mBlockSize;
      mBuffer.consume(sizeReader.pos());
   }

   // enough data?
   return (mBuffer.bytesAvailable() >= mBlockSize);
}

//-----------------------------------------------------------------------------
/*!
   \return ingame messaging active flag
*/
bool BombermanClient::isIngameMessagingActive() const
{
   return mIngameMessagingActive;
}

//-----------------------------------------------------------------------------
/*!
   \param active ingame messaging active flag
*/
void BombermanClient::setIngameMessagingActive(bool active)
{
   mIngameMessagingActive = active;
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::toggleIngameMessaging()
{
   setIngameMessagingActive(!isIngameMessagingActive());
}

//-----------------------------------------------------------------------------
/*!
   \param id id of the map id
   \param mapitem map item ptr
*/
MapItem* BombermanClient::getMapItem(int id) const
{
   MapItem* item = nullptr;

   auto it = mMapItems.find(id);
   if (it != mMapItems.end())
      item = it->second;

   return item;
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processCreateGameResponse(Packet* packet)
{
   CreateGameResponsePacket* response = dynamic_cast<CreateGameResponsePacket*>(packet);

   // extract game information
   GameInformation gameInformation = response->getGameInformation();
   int gameId = gameInformation.getId();
   int playerId = gameInformation.getCreatorId();
   bool success = false;

   if (gameId > -1)
   {
      qDebug("BombermanClient::data(): game %d created", gameId);

      // update game information
      GameInformation* oldGameInformation = getGameInformation(gameId);
      if (oldGameInformation)
      {
         *oldGameInformation = gameInformation;
      }
      else
      {
         mGames.push_back(gameInformation);
      }

      success = true;
   }
   else
   {
      qDebug("BombermanClient::data(): game create request failed");
   }

   createGameResponseSignal(success, gameId, playerId == getPlayerId());
}

//-----------------------------------------------------------------------------
/*!
   \param id key
   \param info value
*/
void BombermanClient::addPlayerInfo(int id, PlayerInfo* info)
{
   mPlayerInfo[id] = info;
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processJoinGameResponse(Packet* packet)
{
   JoinGameResponsePacket* response = dynamic_cast<JoinGameResponsePacket*>(packet);

   if (response->isSuccessful())
   {
      PlayerInfo* info = nullptr;

      int id = response->getPlayerId();

      if (!mPlayerInfo.contains(id))
      {
         info = new PlayerInfo();
         info->setId(id);
         info->setNick(response->getNick());
         info->setColor(response->getColor());
         info->setKilled(true);

         addPlayerInfo(id, info);

         // send infomap to all instances interested in them
         playerInfoMapUpdatedSignal(&mPlayerInfo);

         // play "player joined sample"
         if (!GamePlayback::getInstance()->isReplaying())
            SoundManager::getInstance()->playSoundPlayerJoined();
      }

      // ensure it was us who joined as join game responses are
      // broadcasted to all players
      if (response->getPlayerId() == getPlayerId())
      {
         // store current player info
         setCurrentPlayerInfo(info);

         // store game id
         setGameId(response->getGameId());

         GameInformation* gameInformation = getGameInformation(getGameId());

         int width = 0;
         int height = 0;
         switch (gameInformation->getMapDimensions())
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

         playfieldSizeSignal(width, height);
         playfieldScaleSignal(gameInformation->getMapScaleX(), gameInformation->getMapScaleY());
         loadLevelSignal(gameInformation->getLevelName());
         joinGameResponseSignal(true);
      }
   }
   else if (response->getPlayerId() == getPlayerId())
   {
      // the player which did not successfully login was me!
      HelpManager::getInstance()->addMessage("", TEXT_ERROR_UNABLE_TO_JOIN, Constants::HelpSeverityError);
   }
}

//-----------------------------------------------------------------------------
/*!
   \param path path of the level that has been loaded
*/
void BombermanClient::levelLoaded(const QString& /*path*/)
{
   // game successfully joined
   // emit joinGameResponse(true);

   PlayerSynchronizePacket packet(PlayerSynchronizePacket::LevelLoaded);
   send(&packet);
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::processListGameResponse(Packet* packet)
{
   ListGamesResponsePacket* list = dynamic_cast<ListGamesResponsePacket*>(packet);

   // only update game(s) included in list
   if (list->isUpdate())
   {
      foreach (GameInformation info, list->getGames())
      {
         *(getGameInformation(info.getId())) = info;
      }
   }
   else
   {
      mGames = list->getGames();
   }

   // qDebug("BombermanClient::data(): %d games received", mGames.size());

   gamesListUpdatedSignal(mGames);
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processLoginResponse(Packet* packet)
{
   LoginResponsePacket* login = dynamic_cast<LoginResponsePacket*>(packet);

   setPlayerId(login->getId());

   // login is always supposed to work
   playerIdSignal(getPlayerId());
   loginResponseSignal(true);
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processPlayerKilled(Packet* packet)
{
   PlayerKilledPacket* kill = dynamic_cast<PlayerKilledPacket*>(packet);

   // inform game drawable about death
   removePlayerSignal(kill->getPlayerId());

   // check if own player was killed
   if (kill->getPlayerId() == getPlayerId())
      rumbleSignal(0.5f, 2000);

   // update player info instance
   PlayerInfo* killedPlayer = getPlayerInfo(kill->getPlayerId());
   if (killedPlayer)
   {
      killedPlayer->setKilled(true);
   }

   // play killed sample
   SoundManager::getInstance()->playSoundKilled();
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processPlayerInfected(Packet* packet)
{
   PlayerInfectedPacket* infectedPacket = dynamic_cast<PlayerInfectedPacket*>(packet);

   PlayerInfo* player = getPlayerInfo(infectedPacket->getPlayerId());

   if (player)
   {
      /*
      // create guarded pointer with disease
      QPointer<PlayerDisease> disease = new PlayerDisease();
      disease.data()->setType(infectedPacket->getSkullType());
      disease.data()->setDuration(infectedPacket->getDuration());
      disease->activate();

      // infect player
      player->infect(disease);
      */

      playerInfectedSignal(
         infectedPacket->getPlayerId(),
         infectedPacket->getSkullType(),
         infectedPacket->getInfectorId(),
         infectedPacket->getExtraPosX(),
         infectedPacket->getExtraPosY()
      );

      SoundManager::getInstance()->playSkullSound(infectedPacket->getSkullType());
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processDetonation(Packet* packet)
{
   DetonationPacket* det = dynamic_cast<DetonationPacket*>(packet);

   // Vector pos = Vector(det->getX(), det->getY(), 0.0);
   //
   // TODO: pos needs to be rotated and scaled properly
   //       right now 0,0 is closest to the listener

   SoundManager::getInstance()->playSoundBomb();

   int intense = qMax<int>(det->getUp(), det->getDown());
   intense = qMax<int>(intense, det->getLeft());
   intense = qMax<int>(intense, det->getRight());

   detonationSignal(det->getX(), det->getY(), det->getUp(), det->getDown(), det->getLeft(), det->getRight(), static_cast<float>(intense));
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processError(Packet* packet)
{
   ErrorPacket* errorPacket = dynamic_cast<ErrorPacket*>(packet);

   // show error received from server
   HelpManager::getInstance()->addMessage("", errorPacket->getErrorMessage(), Constants::HelpSeverityError);

   switch (errorPacket->getErrorType())
   {
      // if sync fails, the player is kicked out of the game
      case Constants::ErrorSyncTimeout:
      {
         resetGameData();

         showMenuSignal();
         showMainMenuSignal();
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
   \param id player id
   \return player info ptr
*/
PlayerInfo* BombermanClient::getPlayerInfo(int id) const
{
   PlayerInfo* pInfo = nullptr;

   auto iter = mPlayerInfo.find(id);

   if (iter != mPlayerInfo.end())
      pInfo = iter->second;

   return pInfo;
}

//-----------------------------------------------------------------------------
/*!
   \param info current player info
*/
void BombermanClient::setCurrentPlayerInfo(PlayerInfo* info)
{
   mCurrentPlayerInfo = info;
}

//-----------------------------------------------------------------------------
/*!
   \return player info ptr
*/
PlayerInfo* BombermanClient::getCurrentPlayerInfo() const
{
   return mCurrentPlayerInfo;
}

//-----------------------------------------------------------------------------
/*!
   \return list of players
*/
std::vector<PlayerInfo*> BombermanClient::getPlayerInfoList() const
{
   std::vector<PlayerInfo*> list;
   list.reserve(mPlayerInfo.size());
   for (const auto& [id, info] : mPlayerInfo)
      list.push_back(info);
   return list;
}

//-----------------------------------------------------------------------------
/*!
   \return map of players
*/
std::map<int, PlayerInfo*>* BombermanClient::getPlayerInfoMap() const
{
   return &mPlayerInfo;
}

//-----------------------------------------------------------------------------
/*!
   \return ptr to position interpolation
*/
PositionInterpolation* BombermanClient::getPositionInterpolation() const
{
   return mPositionInterpolation;
}

//-----------------------------------------------------------------------------
/*!
   \return current message
*/
const std::string& BombermanClient::getMessage() const
{
   return mMessage;
}

//-----------------------------------------------------------------------------
/*!
   \param message message to set
*/
void BombermanClient::setMessage(const std::string& message)
{
   mMessage = message;
}

//-----------------------------------------------------------------------------
/*!
   \param connected connected state
*/
void BombermanClient::setConnected(bool connected)
{
   mConnected = connected;
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if client is connected
*/
bool BombermanClient::isConnected() const
{
   return mConnected;
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if we're hosting games
*/
bool BombermanClient::isHosting() const
{
   return mServer;
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processPosition(Packet* packet)
{
   PositionPacket* posPacket = dynamic_cast<PositionPacket*>(packet);

   PlayerInfo* playerInfo = getPlayerInfo(posPacket->getPlayerId());

   if (playerInfo)
   {
      playerInfo->setPosition(posPacket->getX(), posPacket->getY(), posPacket->getAngle());

      playerInfo->setPositionDelta(posPacket->getDeltaX(), posPacket->getDeltaY(), posPacket->getAngleDelta());

      playerInfo->setDirections(posPacket->getDirections());

      setPlayerPositionSignal(posPacket->getPlayerId(), posPacket->getX(), posPacket->getY(), posPacket->getAngle());

      setPlayerSpeedSignal(posPacket->getPlayerId(), posPacket->getDeltaX(), posPacket->getDeltaY(), posPacket->getAngleDelta());
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processMapItemCreated(Packet* packet)
{
   MapItem* item = new MapItem(dynamic_cast<MapItemCreatedPacket*>(packet));
   mMapItems[item->getUniqueId()] = item;
   createMapItemSignal(item);
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processMapItemMove(Packet* packet)
{
   MapItemMovePacket* movePacket = dynamic_cast<MapItemMovePacket*>(packet);
   MapItem* item = getMapItem(movePacket->getMapItemId());

   if (item)
   {
      moveMapItemSignal(item, movePacket->getDirection(), movePacket->getSpeed(), movePacket->getNominalX(), movePacket->getNominalY());

      // play kick sound
      if (movePacket->getSpeed() > 0.0f)
         SoundManager::getInstance()->playSoundKick();
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processExtraMapItemCreated(Packet* packet)
{
   ExtraMapItem* extra = new ExtraMapItem(dynamic_cast<ExtraMapItemCreatedPacket*>(packet));
   mMapItems[extra->getUniqueId()] = extra;
   createMapItemSignal(extra);

   SoundManager::getInstance()->playSoundExtraRevealed();
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processGameStats(Packet* packet)
{
   GameStatsPacket* statsPacket = dynamic_cast<GameStatsPacket*>(packet);

   // stats are copied
   QList<int> ids = statsPacket->getPlayerIds();
   QList<PlayerStats> overallStats = statsPacket->getOverallStats();
   QList<PlayerStats> roundStats = statsPacket->getRoundStats();

   PlayerInfo* info = nullptr;

   int i = 0;
   foreach (int id, ids)
   {
      auto iter = mPlayerInfo.find(id);

      if (iter != mPlayerInfo.end())
      {
         info = iter->second;

         /*
         qDebug(
            "BombermanClient::processGameStats: "
            "player %s: wins: %d, kills: %d, deaths: %d",
            qPrintable(info->getNick()),
            stats[i].getWins(),
            stats[i].getKills(),
            stats[i].getDeaths()
         );
         */

         info->setOverallStats(overallStats[i]);
         info->setRoundStats(roundStats[i]);
      }

      i++;
   }

   playerInfoMapUpdatedSignal(&mPlayerInfo);
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processExtraMapItemDestroyed(Packet* packet)
{
   MapItemDestroyedPacket* remove = dynamic_cast<MapItemDestroyedPacket*>(packet);
   MapItem* item = getMapItem(remove->getUniqueId());

   if (item)
   {
      item->setDestroyDirection(remove->getDirection());

      destroyMapItemSignal(item, remove->getIntensity());
      mMapItems.erase(item->getUniqueId());
      delete item;
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processMapItemRemoved(Packet* packet)
{
   MapItemRemovedPacket* remove = dynamic_cast<MapItemRemovedPacket*>(packet);
   MapItem* item = getMapItem(remove->getUniqueId());

   if (item)
   {
      removeMapItemSignal(item);
      mMapItems.erase(item->getUniqueId());
      delete item;
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::broadcastAddPlayerData()
{
   for (const auto& [id, playerInfo] : mPlayerInfo)
   {
      // reset killed flag
      playerInfo->setKilled(false);

      // tell game drawable about players in the game
      addPlayerSignal(playerInfo->getId(), playerInfo->getNick(), playerInfo->getColor());
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::broadcastPlayerStartPositions()
{
   for (const auto& [id, playerInfo] : mPlayerInfo)
   {
      setPlayerPositionSignal(playerInfo->getId(), playerInfo->getX(), playerInfo->getY(), playerInfo->getAngle());
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::processStartGameResponse(Packet* packet)
{
   StartGameResponsePacket* response = dynamic_cast<StartGameResponsePacket*>(packet);

   qDebug("BombermanClient::data: game %d started", response->getId());

   setIngameMessagingActive(false);
   GameStateMachine::getInstance()->setState(Constants::GameActive);

   mDead = false;

   gameStartedSignal();

   SoundManager* sm = SoundManager::getInstance();
   sm->playSoundStart();
   sm->restartPlayListAfterFadeOut(1000);

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
void BombermanClient::processStopGameResponse(Packet* packet)
{
   StopGameResponsePacket* response = dynamic_cast<StopGameResponsePacket*>(packet);

   qDebug("BombermanClient::data: game %d stopped, all rounds finished: %d", response->getId(), response->isFinished());

   GameStateMachine::getInstance()->setState(Constants::GameStopped);

   mDead = true;

   if (response->isFinished())
   {
      gameStoppedSignal();

      SoundManager* sm = SoundManager::getInstance();
      sm->playSoundStart();
      sm->restartPlayListAfterFadeOut(SHOW_WINNER_TIME_SUM);
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processGameEvent(Packet* packet)
{
   GameEventPacket* response = dynamic_cast<GameEventPacket*>(packet);

   switch (response->getGameEvent())
   {
      case GameEventPacket::ExtraCollected:
      {
         SoundManager::getInstance()->playSoundExtra();

         extraRemovedSignal(response->getX(), response->getY(), false, response->getExtraType(), response->getPlayerId());

         // rumble just a little on extra collect
         if (response->getPlayerId() == getPlayerId())
            rumbleSignal(0.2f, 500);

         break;
      }

      case GameEventPacket::ExtraDestroyed:
      {
         extraRemovedSignal(response->getX(), response->getY(), true, response->getExtraType(), -1);

         break;
      }

      case GameEventPacket::BombExploded:
      default:
         break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processMessage(Packet* packet)
{
   MessagePacket* messagePacket = dynamic_cast<MessagePacket*>(packet);

   messageReceivedSignal(messagePacket->getSenderId(), messagePacket->getMessage(), messagePacket->isTypingFinished());

   if (messagePacket->isTypingFinished())
   {
      // omit broadcast messages
      if (messagePacket->getSenderId() != -1 && messagePacket->getSenderId() != getPlayerId())
      {
         SoundManager::getInstance()->playSoundMessageReceived();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processTime(Packet* packet)
{
   TimePacket* timePacket = dynamic_cast<TimePacket*>(packet);

   /*
      qDebug(
         "BombermanClient::data: server time: %s, left: %d",
         qPrintable(packet->getTimestamp().toString()),
         timePacket->getTimeLeft()
      );
   */

   GameInformation* gameInformation = getCurrentGameInformation();

   if (gameInformation)
   {
      int timeLeft = timePacket->getTimeLeft();
      int duration = gameInformation->getDuration();

      if (timeLeft == (duration * 3 / 20))
      {
         SoundManager::getInstance()->playSoundHurryUp();
      }

      timeChangedSignal(timeLeft, duration);
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processCountdown(Packet* packet)
{
   GameStateMachine::getInstance()->setState(Constants::GamePreparing);

   CountdownPacket* countdownPacket = dynamic_cast<CountdownPacket*>(packet);

   int timeLeft = countdownPacket->getTimeLeft();

   qDebug("BombermanClient::processCountdown: left: %d", timeLeft);

   // sync tick
   if (timeLeft == SERVER_PREPARATION_TIME + SERVER_PREPARATION_SYNC_TIME - 1)
   {
      // use first tick to mute the music while the
      // countdown samples are played
      SoundManager::getInstance()->fadeOut(1000);

      showGameSignal();

      broadcastAddPlayerData();
      broadcastPlayerStartPositions();
   }

   // next ticks
   if (timeLeft <= SERVER_PREPARATION_TIME - 1)
   {
      countdownSignal(timeLeft);

      // play sample
      SoundManager::getInstance()->playSoundTime(timeLeft);
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processPacket(Packet* packet)
{
   if (packet)
   {
      switch (packet->getType())
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
            processDetonation(packet);
            break;
         }

         case Packet::ERROR:
         {
            processError(packet);
            break;
         }

         case Packet::EXTRAMAPITEMCREATED:
         {
            processExtraMapItemCreated(packet);
            break;
         }

         case Packet::GAMESTATS:
         {
            processGameStats(packet);
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

         case Packet::POSITION:
         {
            /*
            qDebug(
               "pos received: %s",
               qPrintable(QTime::currentTime().toString("hh:mm:ss.zzz"))
            );
            */

            processPosition(packet);
            break;
         }

         case Packet::LEAVEGAMERESPONSE:
         {
            processLeaveGameResponse(packet);
            break;
         }

         case Packet::MAPITEMCREATED:
         {
            processMapItemCreated(packet);
            break;
         }

         case Packet::MAPITEMDESTROYED:
         {
            processExtraMapItemDestroyed(packet);
            break;
         }

         case Packet::MAPITEMMOVE:
         {
            processMapItemMove(packet);
            break;
         }

         case Packet::MAPITEMREMOVED:
         {
            processMapItemRemoved(packet);
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
            processTime(packet);
            break;
         }

         case Packet::EXTRASHAKE:
         {
            processExtraShake(packet);
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

      // if playback is activated, record packets
      if (GamePlayback::getInstance()->isRecording())
      {
         GamePlayback::getInstance()->record(packet);
      }
      else
      {
         delete packet;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::readData()
{
   char chunk[4096];
   int bytesRead;

   while ((bytesRead = NET_ReadFromStreamSocket(mSocket, chunk, sizeof(chunk))) > 0)
   {
      mBuffer.append(chunk, bytesRead);
   }

   if (bytesRead < 0)
   {
      NET_DestroyStreamSocket(mSocket);
      mSocket = nullptr;
      clientDisconnect();
      return;
   }

   while (packetAvailable())
   {
      // block was read completely
      BinaryReader in = mBuffer.reader();
      Packet* packet = Packet::deserialize(in);
      mBuffer.consume(in.pos());

      processPacket(packet);

      mBlockSize = 0;
   }

   mBuffer.compact();
}

//-----------------------------------------------------------------------------
/*!
   poll for connection progress and incoming data, once per tick
*/
void BombermanClient::poll()
{
   if (mAddress)
   {
      NET_Status status = NET_GetAddressStatus(mAddress);

      if (status == NET_SUCCESS)
      {
         NET_Address* address = mAddress;
         mAddress = nullptr;

         mSocket = NET_CreateClient(address, 6300, 0);
         NET_UnrefAddress(address);

         if (!mSocket)
         {
            reportConnectionError(SDL_GetError());
         }
      }
      else if (status == NET_FAILURE)
      {
         reportConnectionError(SDL_GetError());
         NET_UnrefAddress(mAddress);
         mAddress = nullptr;
      }

      return;
   }

   if (mSocket && !isConnected())
   {
      NET_Status status = NET_GetConnectionStatus(mSocket);

      if (status == NET_SUCCESS)
      {
         clientConnect();
         connectedSignal();
      }
      else if (status == NET_FAILURE)
      {
         reportConnectionError(SDL_GetError());
         NET_DestroyStreamSocket(mSocket);
         mSocket = nullptr;
      }

      return;
   }

   if (mSocket && isConnected())
   {
      readData();
   }
}

//-----------------------------------------------------------------------------
/*!
   \param event keypressed event
*/
void BombermanClient::keyPressed(QKeyEvent* event)
{
   GameSettings::ControllerSettings* controllerSettings = GameSettings::getInstance()->getControllerSettings();

   bool controlKey = event->key() == controllerSettings->getUpKey() || event->key() == controllerSettings->getDownKey() ||
                     event->key() == controllerSettings->getLeftKey() || event->key() == controllerSettings->getRightKey() ||
                     event->key() == controllerSettings->getBombKey() || event->key() == controllerSettings->getZoomInKey() ||
                     event->key() == controllerSettings->getZoomOutKey() || event->key() == controllerSettings->getStartKey();

   if ((controlKey && !event->isAutoRepeat()) || !controlKey)
   {
      processKeyPressed(event->key());
   }
}

//-----------------------------------------------------------------------------
/*!
   \param event keyreleased event
*/
void BombermanClient::keyReleased(QKeyEvent* event)
{
   if (!event->isAutoRepeat())
      processKeyReleased(event->key());
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::releaseAllKeys()
{
   if (mKeysPressed != 0)
   {
      // next bomb may be dropped
      mBombReleased = true;
      mKeysPressed = 0;

      // send key changes
      processKeyReleased(0);
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::clearPlayerInfoMap()
{
   for (const auto& [id, info] : mPlayerInfo)
      delete info;
   mPlayerInfo.clear();
}

//-----------------------------------------------------------------------------
/*!
   \param id player id
*/
void BombermanClient::removePlayerInfo(int id)
{
   auto it = mPlayerInfo.find(id);
   if (it != mPlayerInfo.end())
   {
      delete it->second;
      mPlayerInfo.erase(it);
   }
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processLeaveGameResponse(Packet* packet)
{
   LeaveGameResponsePacket* leavePacket = dynamic_cast<LeaveGameResponsePacket*>(packet);

   // either we left the game, so we have to clear the player info map
   // completely, or it was just one player, so the player map keeps intact
   if (leavePacket->getPlayerId() == getPlayerId())
   {
      clearPlayerInfoMap();
   }
   else
   {
      // remove player from playerinfo-map
      removePlayerInfo(leavePacket->getPlayerId());
   }

   // send infomap to all instances interested in them
   playerInfoMapUpdatedSignal(&mPlayerInfo);

   // remove player
   removePlayerSignal(leavePacket->getPlayerId());

   // play "player left sample"
   if (!GamePlayback::getInstance()->isReplaying())
      SoundManager::getInstance()->playSoundPlayerLeft();
}

//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void BombermanClient::processExtraShake(Packet* packet)
{
   ExtraShakePacket* shakePacket = dynamic_cast<ExtraShakePacket*>(packet);

   MapItem* item = getMapItem(shakePacket->getMapItemUniqueId());

   if (item)
   {
      shakeBlockSignal(item);
      SoundManager::getInstance()->playSoundBoxShake();
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::sendKeysPressedPacket()
{
   // build keypacket
   // qDebug("BombermanClient::keyPressed: sending keypacket");
   KeyPacket kPacket(0, mKeysPressed);
   send(&kPacket);
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::removeBombKeyFlag()
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
void BombermanClient::processKeyPressed(int key)
{
   if (key == Qt::Key_Return || key == Qt::Key_Enter)
   {
      if (GameStateMachine::getInstance()->getState() == Constants::GameActive)
      {
         toggleIngameMessaging();
      }
   }

   // write text
   if (isIngameMessagingActive())
   {
      if (GameStateMachine::getInstance()->getState() == Constants::GameActive)
      {
         if (key == Qt::Key_Escape)
         {
            toggleIngameMessaging();
         }
      }
   }

   // leave game if requested
   else if (key == Qt::Key_Escape)
   {
      // restart music if escape was hit during countdown
      if (GameStateMachine::getInstance()->getState() == Constants::GamePreparing)
      {
         SoundManager* sm = SoundManager::getInstance();
         sm->restartPlayListAfterFadeOut(1000);
      }

      leaveGameRequest();
      mBotFactory->removeAll();

      // reset game data is called before showMenu/showMainMenu
      // as the win drawable sets itself visible when the game state
      // is changed to 'stopped' (that happens in resetGameData)
      resetGameData();

      showMenuSignal();
      showMainMenuSignal();
   }

   // zoom camera
   else if (key == Qt::Key_BracketLeft)
   {
      zoomOutSignal(true);
   }
   else if (key == Qt::Key_BracketRight)
   {
      zoomInSignal(true);
   }

   // abort game (F10/Start)
   else if (key == Qt::Key_F10)
   {
      stopGame();
   }

   // care about movement
   else
   {
      bool moved = false;

      GameSettings::ControllerSettings* controllerSettings = GameSettings::getInstance()->getControllerSettings();

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
      else if (key == controllerSettings->getBombKey()
               // || key == Qt::Key_Space
               // || key == Qt::Key_Control
      )
      {
         // TODO: verify if omitting the bombrelease flag is alright
         //       in the past keyrelease events were dropped and the
         //       flag was not reset properly, therefore bombkey-presses
         //       were left out.. duh.
         //
         if (true /*mBombReleased*/)
         {
            // in any case remove the bomb key in order to avoid dropping
            // too many bombs (i.e. autofire is disabled for bombs)
            mBombReleased = false;
            mKeysPressed |= Constants::KeyBomb;
            moved = true;
         }
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
}

void BombermanClient::processKeyReleased(int key)
{
   GameSettings::ControllerSettings* controllerSettings = GameSettings::getInstance()->getControllerSettings();

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
   // zoom camera
   else if (key == Qt::Key_BracketLeft)
   {
      zoomOutSignal(false);
   }
   else if (key == Qt::Key_BracketRight)
   {
      zoomInSignal(false);
   }
   else if (key == controllerSettings->getBombKey()
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
void BombermanClient::debugKeyboardInput()
{
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::resetClientState()
{
   setConnected(false);
   mDead = true;

   // we're disconnected
   loginResponseSignal(false);
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::resetGameData()
{
   for (const auto& [id, p] : mPlayerInfo)
   {
      removePlayerSignal(p->getId());
   }

   clearPlayerInfoMap();
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
void BombermanClient::login(const std::string& nick)
{
   setNick(nick);
   LoginRequestPacket login(nick, false);
   send(&login);
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::stopGame()
{
   stopGame(getGameId());
}

//-----------------------------------------------------------------------------
/*!
   \param player's nick
*/
void BombermanClient::setNick(const std::string& nick)
{
   mNick = nick;
}

//-----------------------------------------------------------------------------
/*!
   \return player's nick
*/
const std::string& BombermanClient::getNick() const
{
   return mNick;
}

//-----------------------------------------------------------------------------
/*!
   \param message message to send
   \param receiverId id of the message receiver
*/
void BombermanClient::sendMessage(const std::string& message, bool finishedTyping, int receiverId)
{
   if (!finishedTyping)
      setMessage(message);

   MessagePacket packet(-1, message, finishedTyping, receiverId);

   send(&packet);

   if (finishedTyping)
   {
      setMessage("");
      SoundManager::getInstance()->playSoundMessageSent();
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::listGames()
{
   if (isConnected() && (GameStateMachine::getInstance()->getState() != Constants::GameActive))
   {
      ListGamesRequestPacket packet;
      send(&packet);
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::createGameAutomatic()
{
   createGame(
      "coding",
      Level::getLevelDirectoryName(Level::LevelCastle).toStdString(),
      1,
      1800,
      10,
      true,
      true,
      true,
      true,
      true,
      Constants::Dimension13x11
   );
}

//-----------------------------------------------------------------------------
/*!
   \param name game's name
*/
void BombermanClient::createGame(
   const std::string& name,
   const std::string& level,
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
   bool dryRun = GameSettings::getInstance()->getDevelopmentSettings()->isDryRunEnabled();

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
void BombermanClient::joinGame(int game)
{
   JoinGameRequestPacket packet(game);
   send(&packet);
}

//-----------------------------------------------------------------------------
/*!
   \param game game to start
*/
void BombermanClient::startGame(int game)
{
   GameStateMachine::getInstance()->setState(Constants::GamePreparing);

   StartGameRequestPacket packet(game);
   send(&packet);
}

//-----------------------------------------------------------------------------
/*!
   \param game game to stop
*/
void BombermanClient::stopGame(int game)
{
   StopGameRequestPacket request(game);
   send(&request);
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::setLoginAfterConnect(bool loginAfterConnect)
{
   mLoginAfterConnect = loginAfterConnect;
}

//-----------------------------------------------------------------------------
/*!
   \param host host to use
   \param nick nick to use
*/
void BombermanClient::loginRequest(const std::string& host, const std::string& nick)
{
   const std::string previousHost = getHost();
   const bool connectedOrConnecting = (mSocket != nullptr) || (mAddress != nullptr);

   setHost(host);
   setNick(nick);

   // we first have to disconnect from the current server
   // because its hostname obviously differs from ours
   if (connectedOrConnecting && StringUtils::toLower(previousHost) != StringUtils::toLower(host))
   {
      qDebug(
         "BombermanClient::loginRequest: connect to another host: %s -> %s",
         StringUtils::toLower(previousHost).c_str(),
         StringUtils::toLower(host).c_str()
      );

      setConnected(false);
      disconnectFromServer();
   }
   else
   {
      if (!isConnected())
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
void BombermanClient::leaveGameRequest()
{
   LeaveGameRequestPacket packet(getGameId(), getPlayerId());
   send(&packet);
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::gameListRequest()
{
   listGames();
}

//----------------------------------------------------------------------------
/*!
 */
void BombermanClient::host()
{
   if (!mServer)
   {
      mServer = new Server();

      if (mServer->isListening())
      {
         // Server and BombermanClient are both poll-based now, so there's no reason left to
         // run the embedded server on its own thread - it ticks via its own Timer on this
         // (the main) thread, same as before, just without the moveToThread() hop.
         mServer->startPolling();

         hostingSignal(true);
      }
      else
      {
         HelpManager::getInstance()->addMessage("", TEXT_ERROR_UNABLE_TO_BIND, Constants::HelpSeverityError);

         delete mServer;
         mServer = nullptr;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::initializeBots()
{
   if (isPlayerOwner())
   {
      // pass relevant data to bot factory
      mBotFactory->setHostname(getHost());
      mBotFactory->setGameId(getGameId());

      int bots = 0;

      if (isSinglePlayer())
         bots = GameSettings::getInstance()->getCreateGameSettingsSingle()->getBotCount();
      else
         bots = GameSettings::getInstance()->getCreateGameSettingsMulti()->getBotCount();

      mBotFactory->add(bots);
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::initializePlayback()
{
   bool recording = GameSettings::getInstance()->getDevelopmentSettings()->isGameRecordingEnabled();

   GamePlayback::getInstance()->setRecording(recording);

   connect(GamePlayback::getInstance(), SIGNAL(playBack(Packet*)), this, SLOT(processPacket(Packet*)));

   connect(GamePlayback::getInstance(), SIGNAL(finished()), this, SLOT(playbackFinished()));

   connect(this, SIGNAL(playerId(int)), GamePlayback::getInstance(), SLOT(setPlayerId(int)));
}

//-----------------------------------------------------------------------------
/*!
   \return local ips
*/
std::vector<std::string> BombermanClient::getLocalIps() const
{
   std::vector<std::string> ips;

   int count = 0;
   NET_Address** addresses = NET_GetLocalAddresses(&count);

   if (addresses)
   {
      for (int i = 0; i < count; i++)
      {
         const char* address = NET_GetAddressString(addresses[i]);

         if (!address)
            continue;

         std::string ip(address);

         // IPv4 only, no loopback
         if (ip.find(':') == std::string::npos && ip != "127.0.0.1")
            ips.push_back(ip);
      }

      NET_FreeLocalAddresses(addresses);
   }

   return ips;
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if game is single player
*/
bool BombermanClient::isSinglePlayer() const
{
   bool singlePlayer = false;

   singlePlayer = (getGameMode() == Constants::GameModeSinglePlayer);

   return singlePlayer;
}

//-----------------------------------------------------------------------------
/*!
   \return game mode
*/
Constants::GameMode BombermanClient::getGameMode() const
{
   return mGameMode;
}

//-----------------------------------------------------------------------------
/*!
   \param mode game mode
*/
void BombermanClient::setGameMode(const Constants::GameMode& mode)
{
   mGameMode = mode;
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::playbackFinished()
{
   leaveGameRequest();
   showMenuSignal();
   showMainMenuSignal();
   resetGameData();
}

//-----------------------------------------------------------------------------
/*!
 */
void BombermanClient::gameStateChanged()
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      setIngameMessagingActive(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \param idle idle mode flag
*/
void BombermanClient::idle(bool idle)
{
   if (idle)
   {
      if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
      {
         // check if we're in main menu right now
         if (isMainMenuActive())
         {
            GamePlayback::getInstance()->playDemo();
         }
      }
   }
   else
   {
      if (GamePlayback::getInstance()->isReplaying())
      {
         GamePlayback::getInstance()->abort();
         playbackFinished();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \param active when main menu is active
*/
void BombermanClient::setMainMenuActive(bool active)
{
   mMainMenuActive = active;
}

//-----------------------------------------------------------------------------
/*!
   \param pageName name of the current page
*/
void BombermanClient::showIps()
{
   // only do this when we're somewhere in the menus
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      std::vector<std::string> ipList = getLocalIps();

      std::size_t index = 0;
      while (index < ipList.size())
      {
         // combine two ips per message
         std::string combined = ipList[index++];
         if (index < ipList.size())
            combined += ";" + ipList[index++];

         std::string ipText = std::format("your ips are;{}", combined);
         HelpManager::getInstance()->addMessage("", ipText, Constants::HelpSeverityNotification);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \return \c true if main menu is shown
*/
bool BombermanClient::isMainMenuActive() const
{
   return mMainMenuActive;
}
