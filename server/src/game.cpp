/***************************************************************************
 *   Copyright (C) 2005 by Matthias Varnholt   *
 *   matto@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// header
#include "game.h"

// shared
#include "bombkickanimation.h"
#include "bombmapitem.h"
#include "bombpacket.h"
#include "collisiondetection.h"
#include "countdownpacket.h"
#include "detonationpacket.h"
#include "gameeventpacket.h"
#include "gamestatspacket.h"
#include "errorpacket.h"
#include "extramapitem.h"
#include "extramapitemcreatedpacket.h"
#include "extrashakepackethandler.h"
#include "extraspawn.h"
#include "joingamerequestpacket.h"
#include "joingameresponsepacket.h"
#include "keypacket.h"
#include "listgamesresponsepacket.h"
#include "loginrequestpacket.h"
#include "loginresponsepacket.h"
#include "map.h"
#include "mapitemcreatedpacket.h"
#include "mapitemdestroyedpacket.h"
#include "mapitemmovepacket.h"
#include "mapitemremovedpacket.h"
#include "messagepacket.h"
#include "player.h"
#include "playerdisease.h"
#include "playerinfectedpacket.h"
#include "playerkilledpacket.h"
#include "playerstats.h"
#include "positionpacket.h"
#include "startgamerequestpacket.h"
#include "startgameresponsepacket.h"
#include "stopgamerequestpacket.h"
#include "stopgameresponsepacket.h"
#include "stonemapitem.h"
#include "timepacket.h"

// Qt
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>

// c
#include <math.h>

// static variables
int Game::sGameId = 0;


//-----------------------------------------------------------------------------
/*!
   constructor
*/
Game::Game()
   : mUpdateTimer(0),
     mMap(0),
     mGameId(++sGameId),
     mRunning(false),
     mIdlePacketSent(false),
     mGameTimeUpdateTimer(0),
     mDuration(0),
     mCreator(0),
     mState(Constants::GameStopped),
     mPreparationTimer(0),
     mPreparationCounter(0),
     mCheckGameOver(false),
     mSkipCountdown(false),
     mPositionSkipCount(0),
     mGamesPlayed(0),
     mSynchronizationActive(false),
     mMaxSpeed(0.0),
     mSyncMaxTime(0),
     mCollisionDetection(0),
     mImmuneTimes(0),
     mPlayerLeftTheGameCount(0),
     mStartPositionsInitialized(false),
     mGameOnlyPopulatedByBotsMessageShown(false),
     mExtraSpawn(0),
     mExtraSpawnEnabled(false)
{
   qDebug("Game::Game: initializing");

   // init config
   QSettings settings (
      SERVER_CONFIG_FILE_SERVER,
      QSettings::IniFormat
   );

   mPositionSkipCount = settings.value("skip_positions", 1).toInt();
   mSkipCountdown = settings.value("skip_countdown", false).toBool();
   BombMapItem::setTickTime(
      settings.value("tick_count", SERVER_BOMB_TICKTIME_DEFAULT).toInt()
   );
   bool shakePacketsEnabled = settings.value("shake_packets", true).toBool();

   mMaxSpeed =
      SERVER_DEFAULT_SPEED + (SERVER_SPEEDUP_INCREMENT * SERVER_MAX_SPEEDUPS);

   mSyncMaxTime =
      settings.value("player_sync_max_time", SERVER_PLAYER_SYNC_MAX_TIME).toInt();

   // create timers
   mUpdateTimer = new QTimer(this);
   mGameTimeUpdateTimer = new QTimer(this);
   mPreparationTimer = new QTimer(this);

   // init direction maps
   mDirectionCheckCenter.append(Constants::DirectionUp);
   mDirectionCheckAll.append(Constants::DirectionUp);
   mDirectionCheckAll.append(Constants::DirectionDown);
   mDirectionCheckAll.append(Constants::DirectionLeft);
   mDirectionCheckAll.append(Constants::DirectionRight);

   // shake packet handler
   mShakePacketHandler = new ExtraShakePacketHandler(this);
   mShakePacketHandler->setGame(this);
   mShakePacketHandler->setEnabled(shakePacketsEnabled);

   // collision detection
   mCollisionDetection = new CollisionDetection(this);
   mCollisionDetection->setGame(this);

   connect(
      mCollisionDetection,
      SIGNAL(playerKicksBomb(Player*,MapItem*,bool,int)),
      this,
      SLOT(playerKicksBomb(Player*,MapItem*,bool,int))
   );

   connect(
      mCollisionDetection,
      SIGNAL(playerIdle(qint8,Player*)),
      this,
      SLOT(playerIdle(qint8,Player*))
   );

   connect(
      mCollisionDetection,
      SIGNAL(playerMove(Player*,float,float,qint8)),
      this,
      SLOT(playerMove(Player*,float,float,qint8))
   );

   // init skulls
   initSkullSetup();
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
Game::~Game()
{
   qDebug("Game::~Game");

   // remove those animations first since they access mMap
   BombKickAnimation::deleteAll();

   delete mMap;
   delete mImmuneTimes;

   mMap = 0;
   mImmuneTimes = 0;
}


//-----------------------------------------------------------------------------
/*!
   initialize server
*/
void Game::initSkullSetup()
{
   // i disabled reading the skulls from configuration for now..
   // from my point of view it just creates unnecessary confusion

   // init config
   /*
   QSettings settings (
      SERVER_CONFIG_FILE_SERVER,
      QSettings::IniFormat
   );
   */

   // supported skulls

   // used skulls
   bool skullAutofire       = true; // settings.value("skull_autofire", true).toBool();
   bool skullMinimumBomb    = true; // settings.value("skull_minimum_bomb", true).toBool();
   bool skullKeyboardInvert = true; // settings.value("skull_keyboard_invert", true).toBool();
   bool skullMushroom       = true; // settings.value("skull_mushroom", true).toBool();
   bool skullInvisible      = true; // settings.value("skull_invisible", true).toBool();
   bool skullInvincible     = true; // settings.value("skull_invincible", true).toBool();

   // unused for now
   bool skullMaximumBomb    = false; // settings.value("skull_maximum_bomb", false).toBool();
   bool skullSlow           = false; // settings.value("skull_slow", false).toBool();
   bool skullFast           = false; // settings.value("skull_fast", false).toBool();
   bool skullNoBomb         = false; // settings.value("skull_nobomb", false).toBool();

   // skull face ordering

   /*
      [0] red    -> autofire
      [1] blue   -> min bomb
      [2] purple -> keyboardinvert
      [3] dizzy  -> mushroom
      [4] green  -> invisible
      [5] gold   -> invincible
   */

   // used sides
   int skullAutofireSide       = 0; // settings.value("skull_autofire_side", 0).toInt();
   int skullMinimumBombSide    = 1; // settings.value("skull_minimum_bomb_side", 1).toInt();
   int skullKeyboardInvertSide = 2; // settings.value("skull_keyboard_invert_side", 2).toInt();
   int skullMushroomSide       = 3; // settings.value("skull_mushroom_side", 3).toInt();
   int skullInvisibleSide      = 4; // settings.value("skull_invisible_side", 4).toInt();
   int skullInvincibleSide     = 5; // settings.value("skull_invincible_side", 5).toInt();

   // unused for now
   int skullMaximumBombSide    = -1; // settings.value("skull_maximum_bomb_side", -1).toInt();
   int skullSlowSide           = -1; // settings.value("skull_slow_side", -1).toInt();
   int skullFastSide           = -1; // settings.value("skull_fast_side", -1).toInt();
   int skullNoBombSide         = -1; // settings.value("skull_nobomb_side", -1).toInt();

   QList<Constants::SkullType> faces;
   for (int i = 0; i < 6; i++)
      faces.push_back(Constants::SkullReset);

   if (skullAutofireSide > -1 && skullAutofireSide < 6)
      faces[skullAutofireSide]=Constants::SkullAutofire;

   if (skullKeyboardInvertSide > -1 && skullKeyboardInvertSide < 6)
      faces[skullKeyboardInvertSide]=Constants::SkullKeyboardInvert;

   if (skullMushroomSide > -1 && skullMushroomSide < 6)
      faces[skullMushroomSide]=Constants::SkullMushroom;

   if (skullInvisibleSide > -1 && skullInvisibleSide < 6)
      faces[skullInvisibleSide]=Constants::SkullInvisible;

   if (skullInvincibleSide > -1 && skullInvincibleSide < 6)
      faces[skullInvincibleSide]=Constants::SkullInvincible;

   if (skullMinimumBombSide > -1 && skullMinimumBombSide < 6)
      faces[skullMinimumBombSide]=Constants::SkullMinimumBomb;

   if (skullMaximumBombSide > -1 && skullMaximumBombSide < 6)
      faces[skullMaximumBombSide]=Constants::SkullMaximumBomb;

   if (skullSlowSide > -1 && skullSlowSide < 6)
      faces[skullSlowSide]=Constants::SkullSlow;

   if (skullFastSide > -1 && skullFastSide < 6)
      faces[skullFastSide]=Constants::SkullFast;

   if (skullNoBombSide > -1 && skullNoBombSide < 6)
      faces[skullNoBombSide]=Constants::SkullNoBomb;

   PlayerDisease::setSkullFaces(faces);

   // init set of supported skulls
   QSet<Constants::SkullType> supportedSkulls;

   if (skullAutofire)
      supportedSkulls.insert(Constants::SkullAutofire);

   if (skullKeyboardInvert)
      supportedSkulls.insert(Constants::SkullKeyboardInvert);

   if (skullMushroom)
      supportedSkulls.insert(Constants::SkullMushroom);

   if (skullInvisible)
      supportedSkulls.insert(Constants::SkullInvisible);

   if (skullInvincible)
      supportedSkulls.insert(Constants::SkullInvincible);

   if (skullMinimumBomb)
      supportedSkulls.insert(Constants::SkullMinimumBomb);

   if (skullMaximumBomb)
      supportedSkulls.insert(Constants::SkullMaximumBomb);

   if (skullSlow)
      supportedSkulls.insert(Constants::SkullSlow);

   if (skullFast)
      supportedSkulls.insert(Constants::SkullFast);

   if (skullNoBomb)
      supportedSkulls.insert(Constants::SkullNoBomb);

   PlayerDisease::setSupportedSkulls(supportedSkulls);
}


//-----------------------------------------------------------------------------
/*!
   initialize server
*/
void Game::initialize()
{
   qDebug("Game::initialize");

   initializeMap();
   initializeTimers();
}



//-----------------------------------------------------------------------------
/*!
   initialize map
*/
void Game::initializeMap()
{
   // if there are still items in the destroyed map, clear that map.
   // that map is only processed in 'bombExploded()' which is only triggered
   // when the game is not stopped. therefore the may be items left in this
   // map after one round
   mDestroyedMapItems.clear();

   // if there was a map before, delete it
   delete mMap;

   // init
   int width = 0;
   int height = 0;
   int stones = 0;

   float extraCount = 0.0f;
   int extraBombs    = (int)mCreateGameData.mExtraBombEnabled;
   int extraFlames   = (int)mCreateGameData.mExtraFlameEnabled;
   int extraKicks    = (int)mCreateGameData.mExtraKickEnabled;
   int extraSpeedUps = (int)mCreateGameData.mExtraSpeedupEnabled;
   int extraSkulls   = (int)mCreateGameData.mExtraSkullsEnabled;

   float extraSum =
        (extraBombs    * SERVER_WEIGHT_BOMBS)
      + (extraFlames   * SERVER_WEIGHT_FLAMES)
      + (extraKicks    * SERVER_WEIGHT_KICKS)
      + (extraSpeedUps * SERVER_WEIGHT_SPEEDUPS)
      + (extraSkulls   * SERVER_WEIGHT_SKULLS);

   // 13x11 field: 16 extras
   // 26x22 field: 32 extras

   // create default map
   QList<QPoint> startPositions;

   if (mCreateGameData.mDimension == Constants::Dimension13x11)
   {
      extraCount = 16.0f;

      width = 13;
      height = 11;
      stones = 60;

      startPositions
         << QPoint(0,0)
         << QPoint(12,10)
         << QPoint(12,0)
         << QPoint(0,10)
         << QPoint(6,5);
   }

   else if (mCreateGameData.mDimension == Constants::Dimension19x17)
   {    
      extraCount = 32.0f;

      width = 19;
      height = 17;
      stones = 120;

      startPositions
         << QPoint(0, 0)
         << QPoint(width - 1, height - 1)
         << QPoint(width - 1, 0)
         << QPoint(0, height - 1)
         << QPoint(0, 8)             // center left
         << QPoint(18, 8)            // center right
         << QPoint(6, 4)             // center quad top left
         << QPoint(12, 4)            // center quad top right
         << QPoint(6, 12)            // center quad bottom left
         << QPoint(12, 12);          // center quad bottom right
   }

   else if (mCreateGameData.mDimension == Constants::Dimension25x21)
   {
      extraCount = 32.0f;

      width = 25;
      height = 21;
      stones = 140;

      startPositions
         << QPoint(0, 0)
         << QPoint(width - 1, height - 1)
         << QPoint(width - 1, 0)
         << QPoint(0, height - 1)
         << QPoint(12, 0)           // center top
         << QPoint(12, 20)          // center bottom
         << QPoint(7, 6)            // center quad top left
         << QPoint(17, 6)           // center quad top right
         << QPoint(7, 14)           // center quad bottom left
         << QPoint(17, 14);         // center quad bottom right
   }

   float valBombs    = 0.0f;
   float valFlames   = 0.0f;
   float valKicks    = 0.0f;
   float valSpeedUps = 0.0f;
   float valSkulls   = 0.0f;

   if (extraBombs > 0)
      valBombs = extraCount * (SERVER_WEIGHT_BOMBS / extraSum);

   if (extraFlames > 0)
      valFlames = extraCount * (SERVER_WEIGHT_FLAMES / extraSum);

   if (extraKicks > 0)
      valKicks = extraCount * (SERVER_WEIGHT_KICKS / extraSum);

   if (extraSpeedUps > 0)
      valSpeedUps = extraCount * (SERVER_WEIGHT_SPEEDUPS / extraSum);

   if (extraSkulls > 0)
      valSkulls = extraCount * (SERVER_WEIGHT_SKULLS / extraSum);

   extraBombs    = valBombs;
   extraFlames   = valFlames;
   extraKicks    = valKicks;
   extraSpeedUps = valSpeedUps;
   extraSkulls   = valSkulls;

   int loop = 0;
   while ((
          extraBombs
        + extraFlames
        + extraKicks
        + extraSpeedUps
        + extraSkulls
      ) < extraCount
      && loop < 5
   )
   {
      if (loop == 0)
         extraBombs = (float)ceil(valBombs);
      if (loop == 1)
         extraFlames = (float)ceil(valFlames);
      if (loop == 2)
         extraKicks = (float)ceil(valKicks);
      if (loop == 3)
         extraSpeedUps = (float)ceil(valSpeedUps);
      if (loop == 4)
         extraSkulls = (float)ceil(valSkulls);

      loop++;
   }

   mMap = Map::generateMap(
      width,         // width
      height,        // height
      stones,        // stones
      extraBombs,    // bombs
      extraFlames,   // flames
      extraSpeedUps, // speedups
      extraKicks,    // kicks
      extraSkulls,   // skulls
      startPositions // start positions
   );


   delete mImmuneTimes;
   int fieldCount = width * height;
   mImmuneTimes = new int[fieldCount];
   memset(mImmuneTimes, 0, fieldCount*sizeof(int));
}


//-----------------------------------------------------------------------------
/*!
   initialize timers
*/
void Game::initializeTimers()
{
   qDebug("Game::initializeTimers");

   // update timer
   connect(
      mUpdateTimer,
      SIGNAL(timeout()),
      this,
      SLOT(update())
   );

   mUpdateTimer->start(1000 / SERVER_HEARTBEAT_IN_HZ);

   // game time timer
   connect(
      mGameTimeUpdateTimer,
      SIGNAL(timeout()),
      this,
      SLOT(processGameTime())
   );

   mGameTimeUpdateTimer->start(1000);

   // game preparation timer
   mPreparationTimer->setInterval(1000);

   connect(
      mPreparationTimer,
      SIGNAL(timeout()),
      this,
      SLOT(updatePrepareGame())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::broadcastStartPositions()
{
   foreach (Player* currentPlayer, mPlayers)
   {
      PositionPacket* positionPacket = new PositionPacket(
         currentPlayer->getId(),
         Constants::KeyDown,
         currentPlayer->getX(),
         currentPlayer->getY(),
         (float)M_PI * 1.5f
      );

      mOutgoingPackets.append(positionPacket);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::initializePlayerStartPositions()
{
   int startPositionIndex = 0;
   foreach (Player* currentPlayer, mPlayers)
   {
      // reset player's extras
      currentPlayer->reset();
      currentPlayer->getPlayerRotation()->reset();

      // reposition player
      QPoint startPosition = mMap->getStartPosition(startPositionIndex);
      currentPlayer->setX(startPosition.x() + 0.5f);
      currentPlayer->setY(startPosition.y() + 0.5f);

      startPositionIndex++;
   }

   setStartPositionsInitialized(true);
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::broadcastCreateMapItems()
{
   QList<MapItemCreatedPacket*> createPackets = mMap->getMapItemCreatedPackets();
   for (int i = 0; i < createPackets.size(); i++)
      mOutgoingPackets.append(createPackets[i]);
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::broadcastClearMapItems()
{
   QList<MapItemRemovedPacket*> removePackets = mMap->getMapItemRemovedPackets();
   for (int i = 0; i < removePackets.size(); i++)
      mOutgoingPackets.append(removePackets[i]);
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::initMapRelatedItems()
{
   broadcastClearMapItems();

   // reinitialize the map
   initializeMap();

   // reinitialize players
   initializePlayerStartPositions();
   broadcastStartPositions();

   // broadcast new map
   broadcastCreateMapItems();
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::broadcastGameInformation()
{
   QList<GameInformation> games;
   games << getGameInformation();
   mOutgoingPackets.append(
      new ListGamesResponsePacket(
         games,
         true
      )
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::broadcastStartGame()
{
   mOutgoingPackets.append(
      new StartGameResponsePacket(
         mGameId,
         true
      )
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::startGame()
{
   qDebug("Game::startNewGame");

   // reset the number of players that left during the running game
   setGameOnlyPopulatedByBotsMessageShown(false);
   resetPlayersLeftTheGameCount();

   // reset the game time
   mGameTime.restart();

   // set state machine to active
   setState(Constants::GameActive);

   mRunning = true;
   mIdlePacketSent = false;

   // increase the number of rounds played
   increaseGamesPlayed();

   // broadcast game time
   processGameTime();

   // broadcast new game information to all players
   broadcastGameInformation();

   // broadcast "started" to all players
   broadcastStartGame();
}


//-----------------------------------------------------------------------------
/*!
   \return game information object
*/
GameInformation Game::getGameInformation()
{
   GameInformation gameInformation = GameInformation(
      getId(),
      getPlayerCount(),
      getMaximumPlayerCount(),
      getName(),
      getLevelName(),
      getCreator()->getId(),
      getMapDimension(),
      getExtras(),
      getDuration(),
      getGamesPlayed(),
      getGameRound()->getCurrent(),
      getGameRound()->getCount(),
      isSpawnExtrasEnabled()
   );

   return gameInformation;
}


//-----------------------------------------------------------------------------
/*!
   the main update loop
*/
void Game::update()
{
   // update positions
   updatePositions();

   // there is no point checking extra or bomb conditions while the game isn't
   // running. for position updates it is because after a game or prior its
   // start the player may have to rotate towards the camera should be stopped
   // by sending zero-speed packets
   if (getState() == Constants::GameActive)
   {
      // check if player collects extras
      updateExtras();

      // update field immunity
      updateImmuneTimes();

      // check if player drops bombs
      updateBombs();

      // check if infected players collide
      updateInfections();
   }

   // send outgoing packets
   sendBroadcastPackets();
}


//-----------------------------------------------------------------------------
/*!
   update all positions
*/
void Game::updatePositions()
{
   // update player positions
   updatePlayerPositions();
}


//-----------------------------------------------------------------------------
/*!
   update player positions
*/
void Game::updatePlayerPositions()
{
   /*
      movement concept:

      if player wants to move x
         if x movement is allowed but y movement is not
            then
               move x
               correct y

      if player wants to move y
         if y movement is allowed but x movement is not
            then
               move y
               correct x

      => if x and y can be reached needs to be checked separately


             0.0, 0.0      1.0, 0.0      2.0, 0.0

   0.0, 0.0  +-------------+-------------+-------------+
             |  :::::::::  |             |             |
             |:::epsilon:::|             |             |
             |:::epsilon:::|             |             |
             |:::epsilon:::|             |             |
             |  :::::::::  |             |             |
   0.0, 1.0  +-------------+-------------+-------------+
             |             |/////////////|
             |             |/////////////|
             |             |/////////////|
             |             |/////////////|
             |             |/////////////|
   0.0, 2.0  +-------------+-------------+
             |             |
             |             |
             |             |
             |             |
             |             |
             +-------------+
   */

   if (isStartPositionInitialized())
   {
      QMapIterator<QTcpSocket*, Player*> p(mPlayerSockets);
      while (p.hasNext())
      {
         p.next();
         Player* player = p.value();
         mCollisionDetection->process(player);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
bool Game::isKickPossible(
   int x,
   int y,
   Constants::Direction kickDir
)
{
   // kick if possible if level dimensions not exceeded and
   // if there's no bomb, stone or block located in the kick
   // direction

   bool kickPossible = false;

   int checkOffsetX = x;
   int checkOffsetY = y;

   switch (kickDir)
   {
      case Constants::DirectionUp:
         checkOffsetY -= 1;
         break;
      case Constants::DirectionDown:
         checkOffsetY += 1;
         break;
      case Constants::DirectionLeft:
         checkOffsetX -= 1;
         break;
      case Constants::DirectionRight:
         checkOffsetX += 1;
         break;
      default:
         break;
   }

   if (
         checkOffsetX >= 0 && checkOffsetX < getMap()->getWidth()
      && checkOffsetY >= 0 && checkOffsetY < getMap()->getHeight()
   )
   {
      // check if there is an obstructing map item
      MapItem* item = getMap()->getItem(checkOffsetX, checkOffsetY);

      if (item)
      {
         if (item->getType() == MapItem::Extra)
         {
            kickPossible = true;
         }
      }
      else
      {
         kickPossible = true;
      }

      // check if there is an obstructing player
      if (kickPossible)
      {
         int px = 0;
         int py = 0;

         foreach (Player* p, mPlayers)
         {
            if (!p->isKilled())
            {
               px = (int)floor(p->getX());
               py = (int)floor(p->getY());

               // there is an obstructing player
               if (
                     px == checkOffsetX
                  && py == checkOffsetY
               )
               {
                  /*
                     only block the kick bomb if the obstructing player
                     is really close enough to the bomb to be kicked.
                     the bomb is located at x,y, so the obstructing player
                     is at least 1.0 distance units away. if the position
                     of the potentially obstructing player is

                     kicking player
                      |
                    +---+---+---+
                    | P1|( )|  P2
                    +---+---+---+
                          |    |
                         0.5  obstructing player
                              0.8

                     the following code could be just removed if the
                     kicking still behaves too glitchy.
                     then just set kickPossible = false.
                  */

                  // horizontal kick movement
                  if (
                        kickDir == Constants::DirectionLeft
                     || kickDir == Constants::DirectionRight
                  )
                  {
                     float dx = fabs(p->getX() - x);

                     if (dx < 1.0f + SERVER_KICK_PLAYER_DISTANCE)
                     {
                        kickPossible = false;
                     }
                  }

                  // vertical kick movement
                  if (
                        kickDir == Constants::DirectionUp
                     || kickDir == Constants::DirectionDown
                  )
                  {
                     float dy = fabs(p->getY() - y);

                     if (dy < 1.0f + SERVER_KICK_PLAYER_DISTANCE)
                     {
                        kickPossible = false;
                     }
                  }
               }
            }
         }
      }
   }

   return kickPossible;
}


//-----------------------------------------------------------------------------
/*!
   \param kickedBomb bomb the kick animation is based on
   \param kickDir kick direction
*/
void Game::createKickAnimation(
   BombMapItem* kickedBomb,
   Constants::Direction kickDir
)
{
   connect(
      kickedBomb,
      SIGNAL(kickAnimation(Constants::Direction,float)),
      this,
      SLOT(bombKickedAnimation(Constants::Direction,float))
   );

   // init kick animation
   // also a kick animation needs the playfield map
   // for collision detection
   BombKickAnimation* bka = new BombKickAnimation(this);
   bka->setDirection(kickDir);
   bka->setMap(getMap());

   // a kick animations needs to "know" about current player
   // positions so they bounce back once they hit another player
   foreach(Player* p, mPlayers)
   {
      if (!p->isKilled())
      {
         bka->updatePlayerPosition(p->getId(), p->getX(), p->getY());
      }
   }

   connect(
      mCollisionDetection,
      SIGNAL(playerPositionChanged(int,float,float)),
      bka,
      SLOT(updatePlayerPosition(int,float,float))
   );

   connect(
      this,
      SIGNAL(playerKilled(int)),
      bka,
      SLOT(removePlayerPosition(int))
   );

   connect(
      this,
      SIGNAL(playerLeaves(int)),
      bka,
      SLOT(removePlayerPosition(int))
   );

   kickedBomb->setBombKickAnimation(bka);
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::updateImmuneTimes()
{
   int val = 0;
   int pos = 0;
   for (int y = 0; y < mMap->getHeight(); y++)
   {
      for (int x = 0; x < mMap->getWidth(); x++)
      {
         pos = y * mMap->getWidth() + x;
         val = mImmuneTimes[pos];
         val -= (1000 / SERVER_HEARTBEAT_IN_HZ);
         mImmuneTimes[pos]=qMax(0, val);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
*/
void Game::makeFieldImmune(int x, int y)
{
   mImmuneTimes[y * mMap->getWidth() + x] = SERVER_IMMUNE_FIELD_DELAY;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \return \c true if field is immune
*/
bool Game::isFieldImmune(int x, int y) const
{
   return mImmuneTimes[y * mMap->getWidth() + x] > 0;
}


//-----------------------------------------------------------------------------
/*!
   \param player affected player
   \param item mapitem
*/
void Game::playerKicksBomb(
   Player* player,
   MapItem* item,
   bool verticallyKicked,
   int keysPressed
)
{
   if (
         item
      && item->getType() == MapItem::Bomb
      && !player->isKilled()
      && player->isKickEnabled()
   )
   {
      BombMapItem* kickedBomb = dynamic_cast<BombMapItem*>(item);

      if (kickedBomb)
      {
         // init kick direction
         Constants::Direction kickDir = Constants::DirectionUnknown;

         if (verticallyKicked)
         {
            kickDir =
               (keysPressed & Constants::KeyUp)
                  ? Constants::DirectionUp
                  : Constants::DirectionDown;
         }
         else
         {
            kickDir =
               (keysPressed & Constants::KeyLeft)
                  ? Constants::DirectionLeft
                  : Constants::DirectionRight;
         }

         if (isKickPossible(item->getX(), item->getY(), kickDir))
         {
            if (!kickedBomb->isKicked())
            {
               createKickAnimation(kickedBomb, kickDir);
            }
            else
            {
               // bomb has already been kicked once
               BombKickAnimation* bka = kickedBomb->getBombKickAnimation();

               // update kick-direction
               bka->setDirection(kickDir);
            }

            // kick it
            kickedBomb->kick();
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param obj ptr to disease
*/
void Game::playerDiseaseStopped()
{
   PlayerDisease* disease = dynamic_cast<PlayerDisease*>(sender());

   if (disease)
   {
      mOutgoingPackets.append(
         new PlayerInfectedPacket(
            disease->getPlayerId(),
            Constants::SkullReset
         )
      );
   }
}


//-----------------------------------------------------------------------------
/*!
   \param player affected player
   \param assignedXPos assigned x position
   \param assignedYPos assigned y position
   \param directions player directions
*/
void Game::playerMove(
   Player* player,
   float assignedXPos,
   float assignedYPos,
   qint8 directions
)
{
   float speedX = 0.0;
   float speedY = 0.0;

   if (!player->isKilled())
   {
      speedX = assignedXPos - player->getX();
      speedY = assignedYPos - player->getY();
   }

   mOutgoingPackets.append(
      new PositionPacket(
         player->getId(),
         directions,
         assignedXPos,
         assignedYPos,
         player->getPlayerRotation()->getAngle(),
         speedX,
         speedY,
         player->getPlayerRotation()->getAngleDelta(),
         player->getSpeed()
      )
   );

   player->setPositionSkipCounter(0);
   player->setKeysPressed(player->getKeysPressed());

   // reset idle packet flag
   mIdlePacketSentSet.remove(player);
}


//-----------------------------------------------------------------------------
/*!
   \param directions player's directions
   \param player affected player
*/
void Game::playerIdle(qint8 directions, Player* player)
{
   if (!mIdlePacketSentSet.contains(player))
   {
      // send a zero-distance (speed=0.0 packet)
      mOutgoingPackets.append(
         new PositionPacket(
            player->getId(),
            directions,
            player->getX(),
            player->getY(),
            player->getPlayerRotation()->getAngle(),
            0.0f,
            0.0f,
            player->getPlayerRotation()->getAngleDelta(),
            0.0f
         )
      );

      mIdlePacketSentSet.insert(player);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param player infected player
*/
void Game::createInfection(
   Player* infectedPlayer,
   Player* infectingPlayer,
   ExtraMapItem* extra,
   const QList<Constants::SkullType> &faces
)
{
   float extraElapsedTime = 0.0f;

   if (extra)
      extraElapsedTime = extra->getElapsedTime();

   QPointer<PlayerDisease> disease = new PlayerDisease();
   disease.data()->setPlayerId(infectedPlayer->getId());

   if (infectingPlayer)
   {
      // infecting player infects other player
      disease.data()->setType(infectingPlayer->getDisease()->getType());
   }
   else
   {            
      // find out which side the rotating cube is showing right now
      int sideStep = ((int)floor(extraElapsedTime + 0.5f) ) % 6;
      Constants::SkullType skullType = faces[sideStep];
      disease.data()->setType(skullType);
   }

   // if the infected player has been infected before, abort the old infection
   if (infectedPlayer->getDisease())
      infectedPlayer->getDisease()->abort();

   disease->activate();
   infectedPlayer->infect(disease);

   // whenever the game changes to 'stopped' state we want to delete the
   // disease and send an according packet to the client
   connect(
      this,
      SIGNAL(stateChanged(Constants::GameState)),
      disease.data(),
      SLOT(abort())
   );

   connect(
      disease.data(),
      SIGNAL(stopped()),
      this,
      SLOT(playerDiseaseStopped())
   );

   PlayerInfectedPacket* packet = new PlayerInfectedPacket(
      infectedPlayer->getId(),
      disease.data()->getType()
   );

   // either add the id of the player that infected the other
   // or add the position of the extra the extra is contained in
   if (infectingPlayer)
   {
      packet->setInfectorId(infectingPlayer->getId());
   }
   else
   {
      packet->setExtraPos(extra->getX(), extra->getY());
   }

   mOutgoingPackets.append(packet);
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::updateExtras()
{
   QMapIterator<QTcpSocket*, Player*> p(mPlayerSockets);
   while (p.hasNext())
   {
      p.next();

      Player* player = p.value();

      // init player position
      int x = floor(player->getX());
      int y = floor(player->getY());

      MapItem* mapItem = mMap->getItem(x, y);

      if (
            mapItem
         && mapItem->getType() == MapItem::Extra
      )
      {
         // update player stats
         player->increaseExtrasCollected();

         // evaluate extra and pass it to the player
         ExtraMapItem* extra = (ExtraMapItem*)mapItem;

         switch (extra->getExtraType())
         {
            case Constants::ExtraBomb:
            {
               player->increaseBombCount();
               break;
            }

            case Constants::ExtraFlame:
            {
               player->increaseFlameCount();
               break;
            }

            case Constants::ExtraSpeedup:
            {
               player->setSpeed(
                  qMin(
                     player->getSpeed() + SERVER_SPEEDUP_INCREMENT,
                     mMaxSpeed
                  )
               );

               break;
            }

            case Constants::ExtraKick:
            {
               player->setKickEnabled(true);
               break;
            }

            case Constants::ExtraSkull:
            {
               QList<Constants::SkullType> faces = extra->getSkullFaces();
               createInfection(player, 0, extra, faces);
               break;
            }

            default:
            {
               break;
            }
         }

         // play that lovely coin sample
         GameEventPacket* gameEventPacket = new GameEventPacket(
            GameEventPacket::ExtraCollected,
            1.0f,
            x,
            y
         );

         gameEventPacket->setPlayerId(player->getId());
         gameEventPacket->setExtraType(extra->getExtraType());

         mOutgoingPackets.append(gameEventPacket);

         // remove extramapitem
         mOutgoingPackets.append(
            new MapItemRemovedPacket(extra)
         );

         // remove extra from the map
         mMap->setItem(
            x,
            y,
            0
         );

         delete extra;

         // if the extra is removed here, remove it from the destroyed maps
         // also, so we don't have a dangling pointer in that list
         mDestroyedMapItems.remove(extra);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::updateInfections()
{
   // it is only required to go through here if skulls are used
   if (mCreateGameData.mExtraSkullsEnabled)
   {
      QList<Player*> players = getPlayers();
      QVector2D pos1;
      QVector2D pos2;
      QVector2D vec;

      foreach (Player* player1, players)
      {
         if (player1->isInfected() && !player1->isKilled())
         {
            foreach (Player* player2, players)
            {
               // player must be someone else
               if (player2 != player1)
               {
                  // player "to be infected" must be not infected yet
                  if (!player2->isInfected() && !player2->isKilled())
                  {
                     pos1 = QVector2D(player1->getX(), player1->getY());
                     pos2 = QVector2D(player2->getX(), player2->getY());

                     vec = pos1 - pos2;

                     // if they are near enough to each other
                     if (vec.length() < 0.3)
                     {
                        // player2: the player who is now infected
                        // player1: the one who was already infected
                        createInfection(player2, player1, 0);
                     }
                  }
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::updateBombs()
{
   QMapIterator<QTcpSocket*, Player*> p(mPlayerSockets);
   while (p.hasNext())
   {
      p.next();

      Player* player = p.value();
      // int keysPressed = player->getKeysPressed();

      // check if player wants to drop a bomb
      if (player->isBombKeyLocked())
      {
         int x = floor(player->getX());
         int y = floor(player->getY());
         MapItem* item = mMap->getItem(x, y);

         if (!item)
         {
            // if player is allowed to drop more bombs
            if (player->getBombsDroppedCount() < player->getBombCount())
            {
               player->setBombsDroppedCount(
                  player->getBombsDroppedCount() + 1
               );

               BombMapItem* bomb = new BombMapItem(
                  player->getId(),
                  player->getFlameCount(),
                  -1,
                  x,
                  y
               );

               connect(
                  bomb,
                  SIGNAL(exploded(BombMapItem*, bool)),
                  this,
                  SLOT(bombExploded(BombMapItem*, bool))
               );

               mMap->setItem(x, y, bomb);

               mOutgoingPackets.append(
                  new MapItemCreatedPacket(bomb, player->getId())
               );
            }
         }

         // remove bomb key once a bomb has been dropped
         player->setKeysPressed(
            player->getKeysPressed() &~ Constants::KeyBomb
         );

         player->setBombKeyLocked(false);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param killer bomb dropper
   \param victim player who was killed
*/
void Game::updateStatsPlayerKilled(Player* killer, Player* victim)
{
   if (
         killer
      && killer != victim
   )
   {
      killer->increaseKills();
   }

   if (victim)
   {
      victim->increaseDeaths();
      victim->increaseSurvivalTime(getDuration() - getTimeLeft());
   }

   broadcastGameStats();
}


//-----------------------------------------------------------------------------
/*!
   \param player player who won
*/
void Game::processPlayerWon(Player* player)
{
   if (player)
   {
      player->increaseWins();
      player->increaseSurvivalTime(getDuration() - getTimeLeft());
      player->setKeysPressed(Constants::KeyDown);
   }

   broadcastGameStats();
}


//-----------------------------------------------------------------------------
/*!
   \param message message to send
*/
void Game::sendMessageToOwner(const QString& message)
{
   MessagePacket* messagePacket =
      new MessagePacket(
         mCreator->getId(),
         message,
         true
      );

   QTcpSocket* socket = mPlayerSockets.key(mCreator);

   sendPacket(socket, messagePacket);
}


//-----------------------------------------------------------------------------
/*!
   \return message shown flag
*/
bool Game::isGameOnlyPopulatedByBotsMessageShown() const
{
   return mGameOnlyPopulatedByBotsMessageShown;
}

//-----------------------------------------------------------------------------
/*!
   \param message message shown flag
*/
void Game::setGameOnlyPopulatedByBotsMessageShown(bool value)
{
   mGameOnlyPopulatedByBotsMessageShown = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::processOnlyBotsLeft()
{
   qDebug("Game::processOnlyBotsLeft: the game is now only populated by bots");

   QString message =
      tr("The game is now only populated by bots. Press F10 to abort.");

   if (!isGameOnlyPopulatedByBotsMessageShown())
   {
      setGameOnlyPopulatedByBotsMessageShown(true);
      sendMessageToOwner(message);
   }

   /*
      what to do now?
      - player could send a StopGameRequestPacket
      - player could send a KillBotRequestPacket
   */
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::broadcastGameStats()
{
   QList<int> ids;
   QList<PlayerStats> overallStats;
   QList<PlayerStats> roundStats;

   foreach (Player* p, mPlayers)
   {
      ids << p->getId();
      overallStats << (*p->getOverallStats());
      roundStats << (*p->getRoundStats());
   }

   mOutgoingPackets.append(
      new GameStatsPacket(
         ids,
         overallStats,
         roundStats
      )
   );
}


//-----------------------------------------------------------------------------
/*!
   \param direction the bomb direction
   \param player the player who was killed
*/
void Game::rotateDeadPlayerTowardsBomb(
   Constants::Direction detonationDirection,
   Player* player,
   int x,
   int y
)
{
   QList<Constants::Direction> dirs;

   // the bomb direction is our first choice
   dirs.append(detonationDirection);

   // the directions 90° to that one our 2nd
   switch (detonationDirection)
   {
      case Constants::DirectionLeft:
         dirs.append(Constants::DirectionUp);
         dirs.append(Constants::DirectionDown);
         break;
      case Constants::DirectionRight:
         dirs.append(Constants::DirectionUp);
         dirs.append(Constants::DirectionDown);
         break;
      case Constants::DirectionUp:
         dirs.append(Constants::DirectionLeft);
         dirs.append(Constants::DirectionRight);
         break;
      case Constants::DirectionDown:
         dirs.append(Constants::DirectionLeft);
         dirs.append(Constants::DirectionRight);
         break;
      default:
         break;
   }

   Constants::Direction testDir = Constants::DirectionUnknown;
   bool hitSomething = false;
   foreach (Constants::Direction dir, dirs)
   {
      int checkX = x;
      int checkY = y;
      hitSomething = false;
      MapItem* checkItem = 0;

      // check if "fall direction" is blocked
      switch (dir)
      {
         case Constants::DirectionLeft:
            checkX--;
            break;
         case Constants::DirectionRight:
            checkX++;
            break;
         case Constants::DirectionUp:
            checkY--;
            break;
         case Constants::DirectionDown:
            checkY++;
            break;
         default:
            break;
      }

      // check if the dead player will hit something
      if (
            checkX >= 0 && checkX < mMap->getWidth()
         && checkY >= 0 && checkY < mMap->getHeight()
      )
      {
         checkItem = mMap->getItem(checkX, checkY);

         if (checkItem && checkItem->isBlocking())
         {
            hitSomething = true;
         }
      }

      if (!hitSomething)
      {
         testDir = dir;
         break;
      }
   }

   // it 3 other directions fail => let the player fall towards the bomb
   if (testDir == Constants::DirectionUnknown)
   {
      switch (detonationDirection)
      {
         case Constants::DirectionLeft:
            testDir = Constants::DirectionRight;
            break;
         case Constants::DirectionRight:
            testDir = Constants::DirectionLeft;
            break;
         case Constants::DirectionUp:
            testDir = Constants::DirectionDown;
            break;
         case Constants::DirectionDown:
            testDir = Constants::DirectionUp;
            break;
         default:
            break;
      }
   }

   if (testDir == Constants::DirectionRight)
      player->setKeysPressed(Constants::KeyLeft);
   else if (testDir == Constants::DirectionLeft)
      player->setKeysPressed(Constants::KeyRight);
   else if (testDir == Constants::DirectionUp)
      player->setKeysPressed(Constants::KeyDown);
   else if (testDir == Constants::DirectionDown)
      player->setKeysPressed(Constants::KeyUp);
}


//-----------------------------------------------------------------------------
/*!
   \param bomb bomp map item
*/
void Game::bombExploded(BombMapItem* bomb, bool /*unused*/)
{
   if (getState() != Constants::GameStopped)
   {
      QList<Packet*> removeItems;

      // init
      MapItem* mapItem = 0;

      // player may drop one more bomb
      Player* player = mPlayers.value(bomb->getPlayerId(), 0);

      // player may already be killed :)
      if (player)
         player->setBombsDroppedCount(
            player->getBombsDroppedCount() - 1
         );

      int flameCount = bomb->getFlames();
      int x = bomb->getX();
      int y = bomb->getY();

      QList<Constants::Direction>* directions;
      directions = &mDirectionCheckCenter;

      bool doneUp    = (bomb->getDetonationOrigin() == BombMapItem::Top);
      bool doneDown  = (bomb->getDetonationOrigin() == BombMapItem::Bottom);
      bool doneLeft  = (bomb->getDetonationOrigin() == BombMapItem::Left);
      bool doneRight = (bomb->getDetonationOrigin() == BombMapItem::Right);

      bool checkGameOver = false;

      // init detonation packet information
      int detonationUp    = flameCount;
      int detonationDown  = flameCount;
      int detonationLeft  = flameCount;
      int detonationRight = flameCount;

      int xDist = 0;
      int yDist = 0;

      // simple stuff: if bomb covered an extra, destroy it
      MapItem* shadowedItem = bomb->getShadowedItem();
      if (shadowedItem)
      {
         removeItems.append(
            new MapItemDestroyedPacket(
               shadowedItem,
               bomb->getPlayerId(),
               Constants::DirectionUnknown,
               flameCount
            )
         );

         shadowedItem->setCurrentlyDestroyed(true);
         mDestroyedMapItems.insert(shadowedItem);

         // create appropriate game event
         GameEventPacket* gameEventPacket = new GameEventPacket(
            GameEventPacket::ExtraDestroyed,
            1.0f,
            x,
            y
         );

         mOutgoingPackets.append(gameEventPacket);
      }

      // go beginning from the center into each direction
      for (int flameIndex = 0; flameIndex <= flameCount; flameIndex++)
      {
         // center only needs one direction to be checked
         // append the others, after the center element has been processed
         if (flameIndex == 1)
         {
            directions = &mDirectionCheckAll;
         }

         for (int dirIndex = 0; dirIndex < directions->size(); dirIndex++)
         {
            Constants::Direction direction = directions->at(dirIndex);

            bool processDirection = true;

            xDist = x;
            yDist = y;

            // calculate next position
            switch (direction)
            {
               case Constants::DirectionUp:
               {
                  if (doneUp)
                     processDirection = false;

                  yDist = y - flameIndex;
                  break;
               }

               case Constants::DirectionDown:
               {
                  if (doneDown)
                     processDirection = false;

                  yDist = y + flameIndex;
                  break;
               }

               case Constants::DirectionLeft:
               {
                  if (doneLeft)
                     processDirection = false;

                  xDist = x - flameIndex;
                  break;
               }

               case Constants::DirectionRight:
               {
                  if (doneRight)
                     processDirection = false;

                  xDist = x + flameIndex;
                  break;
               }

               default:
               {
                  break;
               }
            }

            if (
                  processDirection
               && xDist >= 0
               && yDist >= 0
               && xDist < mMap->getWidth()
               && yDist < mMap->getHeight()
            )
            {
               // check for players that are eventually killed
               QMapIterator<qint8, Player*> p(mPlayers);
               while (p.hasNext())
               {
                  p.next();

                  Player* currentPlayer = p.value();

                  /*

                    i had the issue that a position of 5.0 has been communicated
                    to the bot which as been interpreted by floor as 4.9999999999
                    i.e. as 4.
                    so while the bot assumed to be at the right position (5.0)
                    the game set him as killed. that's the reason to add just
                    a little tolerance here.

                     +------------------+------------------+------------------+
                                        |                  |
                                       4.0                5.0
                                        <------------------>

                                      | | |              | | |
                                    4-e 4 4+e          4-e e 4+e


                        = >leads to next issue:

                           +-------------+
                           |             |
                           |      |      |
                           |      |      |
                           |     \_/     |
                           +-------------+
                           |             |
                           |             |
                           |             |
                           |             |
                           +-------------+ <- player is here => player is never hit
                           |             |
                           |             |
                           |             |
                           |             |
                           +-------------+

                  */

   //               float playerPosX = currentPlayer->getX();
   //               float playerPosY = currentPlayer->getY();
   //
   //               int pXminusE = floor(playerPosX - 0.001);
   //               int pX       = floor(playerPosX);
   //               int pXplusE  = floor(playerPosX + 0.001);
   //
   //               int pYminusE = floor(playerPosY - 0.001);
   //               int pY       = floor(playerPosY);
   //               int pYplusE  = floor(playerPosY + 0.001);
   //
   //               if (
   //                     xDist == pXminusE && xDist == pX && xDist == pXplusE
   //                  && yDist == pYminusE && yDist == pY && yDist == pYplusE
   //              )

                  float pX = currentPlayer->getX();
                  float pY = currentPlayer->getY();

                  /*
                     // recently used, but finally removed when all doubles have
                     // been replaced by floats

                  if (floor(pX) < floor(pX + 0.001f))
                     pX += 0.001f;
                  if (floor(pY) < floor(pY + 0.001f))
                     pY += 0.001f;
                  */

                  int currentPlayerX = floor(pX);
                  int currentPlayerY = floor(pY);

                  if (
                        currentPlayerX == xDist
                     && currentPlayerY == yDist
                  )
                  {
                     // kill player - once ;)
                     if (!currentPlayer->isKilled())
                     {
                        if (!currentPlayer->isInvincible())
                        {
                           currentPlayer->setKilled(true);

                           // rotate killed player towards the bomb that killed him
                           rotateDeadPlayerTowardsBomb(
                              direction,
                              currentPlayer,
                              xDist,
                              yDist
                           );

                           // if the bomb has been originally detonated by another player, then
                           // we have a different killer. the player who dropped the first bomb
                           // in the detonation chain is considered "the killer".
                           Player* killer = player;

                           if (
                                 bomb->getIgniterId() != -1
                              && bomb->getIgniterId() != player->getId()
                           )
                           {
                              killer = mPlayers.value(bomb->getIgniterId(), 0);
                           }

                           // update stats
                           updateStatsPlayerKilled(
                              killer,
                              currentPlayer
                           );

                           emit playerKilled(currentPlayer->getId());

                           // create player killed packet
                           mOutgoingPackets.append(
                              new PlayerKilledPacket(
                                 currentPlayer->getId(),
                                 bomb->getPlayerId(),
                                 direction,
                                 bomb->getFlames()
                              )
                           );

                           // show player killed message
                           broadcastMessage(
                              tr("%1 was killed.").arg(currentPlayer->getNick())
                           );

                           // check if game is over
                           checkGameOver = true;
                        }
                     }
                  }
               }

               // ignite bombs that are kicked right into a detonation
               BombKickAnimation::ignite(xDist, yDist);

               // check for mapitems that need to be removed
               mapItem = mMap->getItem(xDist, yDist);

               // if a mapitem is found or the field is immune, stop the
               // detonation
               bool stopDetonation = false;
               if (isFieldImmune(xDist, yDist))
                  stopDetonation = true;

               // if there is a mapitem in the way
               // which is not the bomb that just exploded, then continue
               else if (
                     mapItem
                  && mapItem != bomb
               )
               {
                  stopDetonation = true;

                  if (
                        mapItem->isDestroyable()
                     && !mapItem->isCurrentlyDestroyed()
                     && mapItem->getType() != MapItem::Bomb
                  )
                  {
                     // map item was destroyed
                     removeItems.append(
                        new MapItemDestroyedPacket(
                           mapItem,
                           bomb->getPlayerId(),
                           direction,
                           flameCount
                        )
                     );

                     if (mapItem->getType() == MapItem::Extra)
                     {
                        // create appropriate game event
                        GameEventPacket* gameEventPacket = new GameEventPacket(
                           GameEventPacket::ExtraDestroyed,
                           1.0f,
                           mapItem->getX(),
                           mapItem->getY()
                        );

                        mOutgoingPackets.append(gameEventPacket);
                     }

                     // mark the current item as "is being destroyed"
                     //
                     // background:
                     //
                     //    if surrounding bombs get initiated by the
                     //    currently detonating bomb, they musn't destroy
                     //    any more surrounding mapitems. therefore the
                     //    mapitem that was destroyed shall last a little
                     //    while in order to block the other bombs.
                     mapItem->setCurrentlyDestroyed(true);

                     mDestroyedMapItems.insert(mapItem);
                  }

                  // initiate surrounding bombs
                  if (
                        mapItem->getType() == MapItem::Bomb
                     && !mapItem->isCurrentlyDestroyed()
                  )
                  {
                     // neighboured bombs explode from another bomb's explosion
                     BombMapItem* neighbourBomb = (BombMapItem*)mapItem;

                     // origin: left
                     if (neighbourBomb->getX() > mapItem->getX())
                        neighbourBomb->setDetonationOrigin(BombMapItem::Left);

                     // origin: right
                     if (neighbourBomb->getX() < mapItem->getX())
                        neighbourBomb->setDetonationOrigin(BombMapItem::Right);

                     // origin: top
                     if (neighbourBomb->getY() > mapItem->getY())
                        neighbourBomb->setDetonationOrigin(BombMapItem::Top);

                     // origin: bottom
                     if (neighbourBomb->getY() < mapItem->getY())
                        neighbourBomb->setDetonationOrigin(BombMapItem::Bottom);

                     // trigger recursive explosion
                     if (neighbourBomb->getInterval() > SERVER_BOMB_NEIGHBOUR_DELAY)
                     {
                        // the id of the player who ignited another bomb
                        // with his own one is inherited here; this is done
                        // in order to compute a proper score for the kills.
                        neighbourBomb->setIgniterId(bomb->getPlayerId());

                        // reduce timer interval in order to make the bomb
                        // explode pretty soon
                        neighbourBomb->setInterval(SERVER_BOMB_NEIGHBOUR_DELAY);
                     }
                  }
               }

               if (stopDetonation)
               {
                  // in any case:
                  // stop explosion into the given direction
                  switch (direction)
                  {
                     case Constants::DirectionUp:
                     {
                        doneUp = true;
                        detonationUp = flameIndex /*- 1*/;
                        break;
                     }

                     case Constants::DirectionDown:
                     {
                        doneDown = true;
                        detonationDown = flameIndex /*- 1*/;
                        break;
                     }

                     case Constants::DirectionLeft:
                     {
                        doneLeft = true;
                        detonationLeft = flameIndex /*- 1*/;
                        break;
                     }

                     case Constants::DirectionRight:
                     {
                        doneRight = true;
                        detonationRight = flameIndex /*- 1*/;
                        break;
                     }

                     default:
                     {
                        break;
                     }
                  }
               }
            }
         }
      }

      // cut off explosions on the map's borders
      // this is just done for visual purposes
      if (x + detonationRight >= mMap->getWidth())
         detonationRight -= (x + detonationRight - mMap->getWidth() + 1);

      if (x - detonationLeft < 0)
         detonationLeft = x;

      if (y + detonationDown >= mMap->getHeight())
         detonationDown -= (y + detonationDown - mMap->getHeight() + 1);

      if (y - detonationUp < 0)
         detonationUp = y;

      /*
      qDebug(
         "Game::bombExploded(): at: (%d, %d) "
         "up %d, down %d, left %d, right %d",
         x,
         y,
         detonationUp,
         detonationDown,
         detonationLeft,
         detonationRight
      );
      */

      // cleanup map when all explosions are finished
      foreach (MapItem* destroyedItem, mDestroyedMapItems)
      {
         mMap->setItem(
            destroyedItem->getX(),
            destroyedItem->getY(),
            0
         );

         if (destroyedItem->getType() == MapItem::Stone)
         {
            makeFieldImmune(
               destroyedItem->getX(),
               destroyedItem->getY()
            );

            StoneMapItem* stone = (StoneMapItem*)destroyedItem;
            ExtraMapItem* extra = stone->getExtraMapItem();

            if (extra)
            {
               mMap->setItem(
                  destroyedItem->getX(),
                  destroyedItem->getY(),
                  extra
               );

               // send mapitem
               mOutgoingPackets.append(
                  new ExtraMapItemCreatedPacket(extra)
               );

               extra->initializeStartTime();
            }
         }
      }

      qDeleteAll(mDestroyedMapItems);
      mDestroyedMapItems.clear();

      if (checkGameOver)
      {
         // a player died - so check if there are players left in the game
         updateGameoverCondition();
      }

      // play a bomb sample
      float intensity =
           detonationUp
         + detonationDown
         + detonationLeft
         + detonationRight;

      intensity *= 0.03125;

      mOutgoingPackets.append(
         new GameEventPacket(
            GameEventPacket::BombExploded,
            intensity
         )
      );

      // first the detonation packet
      mOutgoingPackets.append(
         new DetonationPacket(
            x,
            y,
            detonationUp,
            detonationDown,
            detonationLeft,
            detonationRight,
            flameCount
         )
      );

      // remove bombmapitem
      mOutgoingPackets.append(
         new MapItemRemovedPacket(bomb)
      );

      // remove bomb from the map
      mMap->setItem(
         bomb->getX(),
         bomb->getY(),
         0
      );

      bomb->deleteLater();

      // then the "item destroyed" packets
      mOutgoingPackets.append(removeItems);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if game is only populated by bots
*/
bool Game::isGamePopulatedByBots() const
{
   bool botsOnly = true;

   Player* currentPlayer = 0;
   QMapIterator<qint8, Player*> p(mPlayers);
   while (p.hasNext())
   {
      p.next();

      currentPlayer = p.value();

      if (
            !currentPlayer->isBot()
         && !currentPlayer->isKilled()
      )
      {
         botsOnly = false;
         break;
      }
   }

   return botsOnly;
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::updateGameoverCondition()
{
   int alivePlayerCount = 0;
   Player* currentPlayer = 0;
   Player* potentialWinner = 0;

   // check for players that are eventually killed
   QMapIterator<qint8, Player*> p(mPlayers);
   while (p.hasNext())
   {
      p.next();

      currentPlayer = p.value();

      if (!currentPlayer->isKilled())
      {
         potentialWinner = currentPlayer;
         alivePlayerCount++;
      }
   }

   // the game is over
   if (alivePlayerCount <= 1)
   {
      processPlayerWon(potentialWinner);
      finishGame();
   }

   // only bots left
   else if (isGamePopulatedByBots())
   {
      processOnlyBotsLeft();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::resetRoundStats()
{
   QMapIterator<qint8, Player*> p(mPlayers);
   while (p.hasNext())
   {
      p.next();
      p.value()->getRoundStats()->reset();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::prepareGame()
{
   // tell clients to clear their maps
   initMapRelatedItems();

   // start countdown
   if (mSkipCountdown)
   {
      startGame();
   }
   else
   {
      // update game state
      setState(Constants::GamePreparing);

      // start preparation timer
      mPreparationTime.restart();
      mPreparationTimer->start();

      mPreparationCounter = SERVER_PREPARATION_TIME + SERVER_PREPARATION_SYNC_TIME;

      // first countdown is called immediately
      updatePrepareGame();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::updatePrepareGame()
{
   mPreparationCounter--;

   mOutgoingPackets.append(new CountdownPacket(mPreparationCounter));

   if (mPreparationCounter == 0)
   {
      qDebug("Game::updatePrepareGame: starting game now");

      // stop preparation timer
      mPreparationTimer->stop();

      // start the game
      startGame();
   }
   else
   {
      qDebug(
         "Game::updatePrepareGame: starting game in %d secs",
         mPreparationCounter
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::finishGame()
{
   if (getState() != Constants::GameFinishing)
   {
      setState(Constants::GameFinishing);

      mMap->stopBombs();

      // this is the time to display some sort of finish animation

      QTimer::singleShot(
         SERVER_FINISHING_TIME,
         this,
         SLOT(stopGame())
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::stopGame()
{
   setState(Constants::GameStopped);
   setStartPositionsInitialized(false);

   bool finished = false;
   mRunning = false;

   qDebug("Game::updateGameoverCondition: game over");

   // when someone left the game it could be the case that there's only
   // one player left in the game; for that purpose the server triggers
   // updateGameoverCondition() which detects there's only one player alive
   // therefore the round is over. but that is not sufficient:
   // we don't want to start any more new rounds with only one player left
   // => end the game in that case
   if (getPlayerCount() <= 1 && getPlayersLeftTheGameCount() > 0)
   {
      finished = true;
   }
   else
   {
      // process next round
      nextRound();
      finished = mGameRound.isFinished();
   }

   if (finished)
   {
      // send and rounds stats if a round is finished
      broadcastGameStats();
      resetRoundStats();

      // reset count and finished flags, we're done.
      mGameRound.reset();
   }

   // send updated game information to everyone (this is mainly done to
   // communicate the updated round count)
   broadcastGameInformation();

   // broadcast "gameover" to all players
   // the "finished" flag is added to let the client know whether to switch
   // back to the lounge or to stay in the win drawable
   mOutgoingPackets.append(
      new StopGameResponsePacket(
         mGameId,
         finished
      )
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::nextRound()
{
   mGameRound.next();

   // restart game until all rounds are finished
   if (!mGameRound.isFinished())
   {
      int delay =
            SHOW_WINNER_DISPLAY_TIME
          + SHOW_WINNER_FADE_IN_TIME
          + SHOW_WINNER_FADE_OUT_TIME
          + SHOW_WINNER_ADDITIONAL_TIME;

      QTimer::singleShot(
         delay,
         this,
         SLOT(prepareGame())
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::startSynchronization()
{
   if (!isSynchronizationActive())
   {
      mSynchronizationTime.restart();
      setSynchronizationActive(true);

      synchronize();
   }
}


//-----------------------------------------------------------------------------
/*!
   \param active synchronization active flag
*/
void Game::setSynchronizationActive(bool active)
{
   mSynchronizationActive = active;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if synchronization is active
*/
bool Game::isSynchronizationActive() const
{
   return mSynchronizationActive;
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::synchronize()
{
   bool synchronized = true;

   // check is all players already sent their "sync" flag
   QList<Player*> players = getPlayers();
   QList<Player*> criticalPlayers;
   int botCount = 0;

   foreach (Player* player, players)
   {
      if (player->isBot())
         botCount++;

      if (!player->isLoadingSynchronized())
      {
//         qDebug(
//            "Game::synchronize(): waiting for '%s'",
//            qPrintable(player->getNick())
//         );

         synchronized = false;
         criticalPlayers << player;
      }
   }

   if (synchronized)
   {
      setSynchronizationActive(false);
      prepareGame();
   }
   else
   {
      if (mSynchronizationTime.elapsed() < mSyncMaxTime)
      {
         // otherwise wait and retry
         QTimer::singleShot(100, this, SLOT(synchronize()));
      }
      else
      {
         setSynchronizationActive(false);

         // time has elapsed, kick those who died trying
         foreach (Player* player, criticalPlayers)
         {
            qDebug(
               "Game::synchronize: time to kick '%s' out of the game",
               qPrintable(player->getNick())
            );

            QTcpSocket* socket = mPlayerSockets.key(player);
            emit forceLeaveGame(socket);

            ErrorPacket* errorPacket = new ErrorPacket(
               Constants::ErrorSyncTimeout, tr("sync aborted.;your pc is too slow.")
            );

            sendPacket(socket, errorPacket);
         }

         int playersLeftCount = players.size() - criticalPlayers.size();
         int humanPlayersLeftCount = playersLeftCount - botCount;

         // let the others play (if they're more than 2 guys and not only bots)
         if (
               playersLeftCount > 1
            && playersLeftCount > botCount
         )
         {
            prepareGame();
         }
         else
         {
            if (humanPlayersLeftCount > 0)
            {
               // there's no use running a game with one player in it
               // therefore we stop it.
               stopGame();
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   send single packet
*/
void Game::sendPacket(QTcpSocket* socket, Packet* packet)
{
   // init bytearray
   packet->serialize();

   /*
   qDebug(
      "Game::sendPacket: sending packet to player %p (%d bytes)",
      socket,
      packet->size()
   );
   */

   // send packet
   if (socket)
      socket->write(*packet);

   // clean up
   delete packet;
}


//-----------------------------------------------------------------------------
/*!
   send outgoing packets
*/
void Game::sendBroadcastPackets()
{
   if (!mOutgoingPackets.isEmpty())
   {
      QMapIterator<QTcpSocket*, Player*> p(mPlayerSockets);

      bool synced = false;
      Packet* packet = 0;
      Packet::TYPE pType = Packet::INVALID;

      while (p.hasNext())
      {
         p.next();
         synced = p.value()->isLoadingSynchronized();

         QTcpSocket* socket = p.key();

         // write outgoing packets to socket
         for (int i = 0; i < mOutgoingPackets.size(); ++i)
         {
            mOutgoingPackets.at(i)->serialize();

            if (socket)
            {
               packet = mOutgoingPackets.at(i);
               pType = packet->getType();

               if (
                     synced
                  || pType == Packet::JOINGAMERESPONSE
                  || pType == Packet::MESSAGE
               )
               {
                  socket->write(*packet);
               }
            }
         }
      }

      // clean up
      while (!mOutgoingPackets.isEmpty())
      {
         delete mOutgoingPackets.takeFirst();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param tcpSocket tcp socket
   \param packet packet to process
*/
void Game::processPacket(
   QTcpSocket* tcpSocket,
   Packet* packet
)
{
   switch(packet->getType())
   {
      case Packet::MESSAGE:
      {
         MessagePacket* senderPacket = (MessagePacket*)packet;

         // during the game we don't care if any of the players is currently
         // typing something. this is just relevant for the lounge
         bool broadcastAllowed =
               (mRunning && senderPacket->isTypingFinished())
            || !mRunning;

         if (broadcastAllowed)
         {
            int senderId = mPlayerSockets[tcpSocket]->getId();

            QMapIterator<QTcpSocket*, Player*> p(mPlayerSockets);
            while (p.hasNext())
            {
               p.next();

               QTcpSocket* currentSocket = p.key();

               // send message to all players unless it's private
               if (
                     senderPacket->getReceiverId() == -1
                  || senderPacket->getReceiverId() == p.value()->getId()
               )
               {
                  MessagePacket* messagePacket =
                     new MessagePacket(
                        senderId,
                        QString("%1: %2")
                           .arg(mPlayerSockets[tcpSocket]->getNick())
                           .arg(senderPacket->getMessage().trimmed()),
                        senderPacket->isTypingFinished(),
                        senderPacket->getReceiverId()
                     );

                  sendPacket(currentSocket, messagePacket);
               }
            }
         }

         break;
      }

      case Packet::KEY:
      {
         KeyPacket* keyPacket = (KeyPacket*)packet;

         Player* player = mPlayerSockets[tcpSocket];

         if (
               player
            && !player->isKilled()
            && getState() == Constants::GameActive
         )
         {
            // fix up the player's inputs to not get confused in
            // any way (the player may to go to the left and to
            // the right at the same time for example)
            qint8 keys = keyPacket->getKeys();
            qint8 previousKeys = player->getKeysPressed();

            if (
                  (previousKeys & Constants::KeyRight)
               && (keys & Constants::KeyLeft)
            )
            {
               keys &= ~(Constants::KeyRight);
            }

            if (
                  (previousKeys & Constants::KeyLeft)
               && (keys & Constants::KeyRight)
            )
            {
               keys &= ~(Constants::KeyLeft);
            }

            if (
                  (previousKeys & Constants::KeyUp)
               && (keys & Constants::KeyDown)
            )
            {
               keys &= ~(Constants::KeyUp);
            }

            if (
                  (previousKeys & Constants::KeyDown)
               && (keys & Constants::KeyUp)
            )
            {
               keys &= ~(Constants::KeyDown);
            }

            if (player->isInfected())
            {
               switch (player->getDisease()->getType())
               {
                  case Constants::SkullAutofire:
                     player->getDisease()->applyAutofire(keys);
                     break;

                  case Constants::SkullKeyboardInvert:
                     player->getDisease()->applyKeyboardInvert(keys);
                     break;

                  default:
                     break;
               }
            }

            /*
               the all-time-famous bomb hickup issue

               client perspective

                  player presses bomb key
                  -> client sends key packet
                  player resets bomb key immediately
                  -> client send key packet

               server perspective

                  one processing cycle:

                     server receives a bomb key packet
                     -> set keyspressed
                     server receives a key packet without the bomb key set
                     -> set keyspressed

                     => bomb keys not noticed :(

               "long story short" => lock the bomb key until it is processed!
            */


            if (keys & Constants::KeyBomb)
            {
               player->setBombKeyLocked(true);
            }

            player->setKeysPressed(keys);

//            qDebug(
//               "Game::data: key packet processed: pid: %d, keys: %d, prev: %d",
//               keyPacket->getPlayerId(),
//               keys,
//               previousKeys
//            );
         }

         break;
      }

      case Packet::POSITION:
      {
         qWarning("Game::data(): Packet::POSITION received. no no no. wrong direction!");
         break;
      }

      case Packet::STOPGAMEREQUEST:
      {
         qDebug("Game::data: Packet::STOPGAMEREQUEST received");

         if (getState() == Constants::GameActive)
         {
            Player* player = mPlayerSockets[tcpSocket];

            if (player)
            {
               if (getCreator() == player)
               {
                  finishGame();

                  // show player killed message
                  broadcastMessage(
                     tr("%1 aborted the game.").arg(player->getNick())
                  );

               }
            }
         }

         break;
      }

      case Packet::INVALID:
      {
         qWarning("Game::data: Packet::INVALID received");
         break;
      }

      default:
         break;
   }
}


//----------------------------------------------------------------------------
/*!
   \return game id
*/
int Game::getId() const
{
   return mGameId;
}


//----------------------------------------------------------------------------
/*!
   \return game name
*/
const QString& Game::getName() const
{
   return mCreateGameData.mName;
}


//----------------------------------------------------------------------------
/*!
   \return level name
*/
const QString& Game::getLevelName() const
{
   return mCreateGameData.mLevel;
}


//----------------------------------------------------------------------------
/*!
   \return player count
*/
int Game::getPlayerCount() const
{
   return mPlayers.count();
}


//----------------------------------------------------------------------------
/*!
   \return maximum player count
*/
int Game::getMaximumPlayerCount() const
{
   return mMap->getMaxPlayers();
}


//----------------------------------------------------------------------------
/*!
   \param data create game data
*/
void Game::setCreateGameData(const CreateGameData &data)
{
   mCreateGameData = data;
}

//----------------------------------------------------------------------------
/*!
   \param name new game name
*/
void Game::setName(const QString &name)
{
   mCreateGameData.mName = name;
}


//----------------------------------------------------------------------------
/*!
   \param player player who joined the game
*/
bool Game::joinGame(Player* player, QTcpSocket* playerSocket)
{
   bool joiningAllowed = false;

   if (getPlayerCount() < getMaximumPlayerCount())
   {
      // assign player color
      player->setColor(getColorForNextPlayer());

      // reset player stats on join game event
      player->resetStats();
      player->setKilled(true);

      // send join information to all other players
      QMapIterator<qint8, Player*> p(mPlayers);

      while (p.hasNext())
      {
         p.next();

         Player* existingPlayer = p.value();

         JoinGameResponsePacket* existingPlayerPacket =
            new JoinGameResponsePacket(
               true,
               mGameId,
               existingPlayer->getId(),
               existingPlayer->getNick(),
               existingPlayer->getColor()
            );

         sendPacket(playerSocket, existingPlayerPacket);
      }

      // insert the new player and send him his granted packet
      mPlayers.insert(
         player->getId(),
         player
      );

      mPlayerSockets.insert(
         playerSocket,
         player
      );

      mOutgoingPackets.append(
         new JoinGameResponsePacket(
            true,
            mGameId,
            player->getId(),
            player->getNick(),
            player->getColor()
         )
      );

      joiningAllowed = true;

      /*
         that is obsolete code

         // init and broadcast the new player's start position
         // send the player's start position to everybody
         QPoint startPosition = QPoint(mPlayers.size() - 1, 0);

         player->setX(startPosition.x() + 0.5);
         player->setY(startPosition.y() + 0.5);

         PositionPacket* positionPacket = new PositionPacket(
            player->getId(),
            Constants::KeyDown,
            player->getX(),
            player->getY(),
            M_PI * 1.5f
         );

         mOutgoingPackets.append(positionPacket);
      */
   }
   else
   {
      // game is full
      sendPacket(
         playerSocket,
         new JoinGameResponsePacket(
            false,
            mGameId,
            player->getId(),
            player->getNick(),
            player->getColor()
         )
      );
   }

   return joiningAllowed;
}


//-----------------------------------------------------------------------------
/*!
   \param player ptr to player
   \param game ptr to game
   \param tcpSocket player's tcp socket
*/
void Game::processSpectator(QTcpSocket* tcpSocket)
{
   if (
         getState() == Constants::GameActive
      || getState() == Constants::GamePreparing
   )
   {
      int timeLeft = getTimeLeft();

      /*
      if (timeLeft > SERVER_MAX_REMAINING_TIME)
      {
         // update player newly joined player (time left)
         sendPacket(
            tcpSocket,
            new TimePacket(getTimeLeft())
         );

         // update player newly joined player (game info)
         QList<GameInformation> games;
         games << getGameInformation();

         sendPacket(
            tcpSocket,
            new ListGamesResponsePacket(
               games,
               true
            )
         );

         // show him the current map
         QList<MapItemCreatedPacket*> createPackets = mMap->getMapItemCreatedPackets();
         for (int i = 0; i < createPackets.size(); i++)
         {
            sendPacket(
               tcpSocket,
               createPackets[i]
            );
         }

         // make client show the game
         sendPacket(
            tcpSocket,
            new CountdownPacket(
                 SERVER_PREPARATION_TIME
               + SERVER_PREPARATION_SYNC_TIME
               - 1
            )
         );

         // tell him the game has started
         sendPacket(
            tcpSocket,
            new StartGameResponsePacket(getId(), true)
         );
      }
      */

      if (timeLeft > SERVER_MAX_REMAINING_TIME)
      {
         mSpectators.push_back(tcpSocket);

         QTimer::singleShot(
            SERVER_SPECTATOR_DELAY,
            this,
            SLOT(processSpectatorMessage())
         );
      }
   }
}


//----------------------------------------------------------------------------
/*!
*/
void Game::processSpectatorMessage()
{
   if (!mSpectators.isEmpty())
   {
      QPointer<QTcpSocket> tcpSocket = mSpectators.takeFirst();

      if (tcpSocket)
      {
         int timeLeft = getTimeLeft();

         // tell player next round will start in n seconds
         MessagePacket* message1 =
            new MessagePacket(
              -1,
               tr("Please wait, the game is currently active."),
               true
            );

         MessagePacket* message2 =
            new MessagePacket(
              -1,
              (timeLeft == 1)
                 ? tr("The next game will start in about 1 second.")
                 : tr("The next game will start in about %1 seconds.").arg(timeLeft),
               true
            );

         sendPacket(tcpSocket, message1);
         sendPacket(tcpSocket, message2);
      }
   }
}


//----------------------------------------------------------------------------
/*!
   \return position skip count
*/
int Game::getPositionSkipCount() const
{
   return mPositionSkipCount;
}


//----------------------------------------------------------------------------
/*!
   \return bot count
*/
int Game::getBotCount() const
{
   int bots = 0;
   foreach (Player* player, mPlayers)
   {
      if (player->isBot())
         bots++;
   }
   return bots;
}


//----------------------------------------------------------------------------
/*!
   \return ptr to game round instance
*/
GameRound *Game::getGameRound()
{
   return &mGameRound;
}


//----------------------------------------------------------------------------
/*!
*/
void Game::increasePlayersLeftTheGameCount()
{
   mPlayerLeftTheGameCount++;
}


//----------------------------------------------------------------------------
/*!
*/
void Game::resetPlayersLeftTheGameCount()
{
   mPlayerLeftTheGameCount=0;
}


//----------------------------------------------------------------------------
/*!
   \return players left game counter
*/
int Game::getPlayersLeftTheGameCount() const
{
   return mPlayerLeftTheGameCount;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if start position is initialized
*/
bool Game::isStartPositionInitialized() const
{
   return mStartPositionsInitialized;
}


//----------------------------------------------------------------------------
/*!
   \param value start position initialized flag
*/
void Game::setStartPositionsInitialized(bool value)
{
   mStartPositionsInitialized = value;
}


//----------------------------------------------------------------------------
/*!
   \param player player who left the game
*/
void Game::removePlayer(Player* player, QTcpSocket* playerSocket)
{
   increasePlayersLeftTheGameCount();

   mPlayerSockets.remove(playerSocket);
   mPlayers.remove(player->getId());

   // reset player stats on leave game event
   player->resetStats();

   // also the player needs to synchronize loading
   // the next time he's joining a game
   player->setLoadingSynchronized(false);

   emit playerLeaves(player->getId());
}


//----------------------------------------------------------------------------
/*!
   \param packet packet to send
*/
void Game::addOutgoingPacket(Packet *packet)
{
   mOutgoingPackets << packet;
}


//----------------------------------------------------------------------------
/*!
   \return time left
*/
int Game::getTimeLeft()
{
   return mCreateGameData.mDuration - (mGameTime.elapsed() * 0.001f);;
}


//----------------------------------------------------------------------------
/*!
*/
void Game::processGameTime()
{
   if (getState() == Constants::GameActive)
   {
      int timeLeft = getTimeLeft();

      if (timeLeft <= 0)
      {
         finishGame();
      }

      mOutgoingPackets.append(
         new TimePacket(timeLeft)
      );
   }
}


//----------------------------------------------------------------------------
/*!
*/
void Game::setCreator(Player* creator)
{
   mCreator = creator;
}


//----------------------------------------------------------------------------
/*!
   \return ptr to the game's creator
*/
Player* Game::getCreator() const
{
   return mCreator;
}


//----------------------------------------------------------------------------
/*!
   \return list of players
*/
QList<Player *> Game::getPlayers() const
{
   return mPlayers.values();
}


//----------------------------------------------------------------------------
/*!
   \param state game state
*/
void Game::setState(Constants::GameState state)
{
   mState = state;
   emit stateChanged(state);
}


//----------------------------------------------------------------------------
/*!
   \return state
*/
Constants::GameState Game::getState() const
{
   return mState;
}


//----------------------------------------------------------------------------
/*!
*/
void Game::increaseGamesPlayed()
{
   mGamesPlayed++;
}


//----------------------------------------------------------------------------
/*!
   \return number of rounds played
*/
int Game::getGamesPlayed() const
{
   return mGamesPlayed;
}


//----------------------------------------------------------------------------
/*!
   \return color for next player
*/
Constants::Color Game::getColorForNextPlayer() const
{
   Constants::Color color = Constants::ColorWhite;

   QSet<Constants::Color> colors;
   for (int i = 1; i <= 10; i++)
      colors.insert((Constants::Color)i);

   foreach (Player* p, mPlayers.values())
      colors.remove(p->getColor());

   QList<Constants::Color> colorList = colors.toList();
   qSort(colorList);

   if (!colorList.isEmpty())
      color = colorList.first();

   return color;
}


//----------------------------------------------------------------------------
/*!
   \return ptr to player socket map
*/
QMap<QTcpSocket*, Player*>* Game::getPlayerSockets()
{
   return &mPlayerSockets;
}


//----------------------------------------------------------------------------
/*!
   \return ptr to game map
*/
Map *Game::getMap() const
{
   return mMap;
}


//----------------------------------------------------------------------------
/*!
   \return map dimensions
*/
Constants::Dimension Game::getMapDimension() const
{
   return mCreateGameData.mDimension;
}


//----------------------------------------------------------------------------
/*!
   \param message message to broadcast
*/
void Game::broadcastMessage(const QString & message)
{
   mOutgoingPackets.append(
      new MessagePacket(
         -1,
         message,
         true
      )
   );
}


//----------------------------------------------------------------------------
/*!
   \return list of extras
*/
int Game::getExtras() const
{
   int extras = 0;

   if (mCreateGameData.mExtraBombEnabled)
      extras |= Constants::ExtraBomb;

   if (mCreateGameData.mExtraFlameEnabled)
      extras |= Constants::ExtraFlame;

   if (mCreateGameData.mExtraSpeedupEnabled)
      extras |= Constants::ExtraSpeedup;

   if (mCreateGameData.mExtraKickEnabled)
      extras |= Constants::ExtraKick;

   if (mCreateGameData.mExtraSkullsEnabled)
      extras |= Constants::ExtraSkull;

   return extras;
}



//----------------------------------------------------------------------------
/*!
   \return game duration
*/
int Game::getDuration() const
{
   return mCreateGameData.mDuration;
}


//-----------------------------------------------------------------------------
/*!
   \param direction move direction
   \param speed move speed
*/
void Game::bombKickedAnimation(Constants::Direction direction, float speed)
{
   BombMapItem* item = dynamic_cast<BombMapItem*>(sender());

   if (item)
   {
      mOutgoingPackets.append(
         new MapItemMovePacket(
            item->getUniqueId(),
            speed,
            direction,
            item->getX(),
            item->getY()
         )
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::initializeExtraSpawn()
{
   mExtraSpawn = new ExtraSpawn(this);
   mExtraSpawn->setMap(getMap());

   connect(
      mExtraSpawn,
      SIGNAL(spawn()),
      this,
      SLOT(spawn())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void Game::spawn()
{
   // only spawn extras in active state
   if (getState() == Constants::GameActive)
   {
      int x = 0;
      int y = 0;
      int px = 0;
      int py = 0;
      bool done = false;
      MapItem* item = 0;
      Map* map = getMap();
      int width = map->getWidth();
      int height = map->getHeight();

      while (!done)
      {
         x = qrand() % width;
         y = qrand() % height;

         item = map->getItem(x, y);

         // there must be no item at x,y
         if (!item)
         {
            // maybe we're done
            done = true;

            // there must be no player at x,y
            foreach (Player* p, mPlayers)
            {
               if (!p->isKilled())
               {
                  px = (int)floor(p->getX());
                  py = (int)floor(p->getY());

                  if (
                        px == x
                     && py == y
                  )
                  {
                     // nope, neeeeeext!
                     done = false;
                     break;
                  }
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if extra spawning is enabled
*/
bool Game::isSpawnExtrasEnabled() const
{
   return mExtraSpawnEnabled;
}
