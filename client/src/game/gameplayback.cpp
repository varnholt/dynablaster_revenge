#include "gameplayback.h"

// Qt
#include <QDataStream>
#include <QStringList>

// shared
#include "countdownpacket.h"
#include "creategameresponsepacket.h"
#include "joingameresponsepacket.h"
#include "joingameresponsepacket.h"
#include "loginresponsepacket.h"
#include "packet.h"
#include "positionpacket.h"
#include "startgameresponsepacket.h"
#include "stopgameresponsepacket.h"

// game
#include "gamestatemachine.h"

// framework
#include "framework/timerhandler.h"

#include <cstdint>
#include <math.h>

#ifdef Q_OS_MAC
#include "stdlib.h"
#endif

GamePlayback* GamePlayback::sInstance = nullptr;


/*

   thoughts on level loading:

   fast pc:

   |--------------------------------------------------------------------------|
       ^          ^    ^     ^             ^          ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
     login       join  | next event     start game         other events
                (self) |
                  |    |
                  |    |
                level loading

   slow pc:

   |--------------------------------------------------------------------------|
       ^          ^            ^     ^             ^           ^ ^ ^ ^ ^ ^ ^
     login       join          | next event     start game       other events
                (self)         |
                  |            |
                  |            |
                  level loading

   idea:

   add the "level loading" time to all upcoming events so their sequential
   consistence is kept.

*/


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
GamePlayback::GamePlayback(QObject* parent)
   : QObject(parent),
     mBlockSize(0),
     mRecording(false),
     mReplaying(false),
     mLevelLoading(false),
     mPlayerId(0)
{
   sInstance = this;

/*
   mTimer.setInterval(10);

   connect(
      &mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(update())
   );
*/

//   TimerHandler::singleShot(2000, this, SLOT(unitTestRoundReplay()));
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::reset()
{
   setRecording(false);
   setLevelLoading(false);
   mRoundMap.clear();
   mPrevPositions.clear();
   clearCheckpoints();
   clearPacketQueue();
   mPacketFilter.reset();
}


//-----------------------------------------------------------------------------
/*!
   \return instance of playback
*/
GamePlayback *GamePlayback::getInstance()
{
   if (!sInstance)
   {
      new GamePlayback();
   }

   return sInstance;
}


//-----------------------------------------------------------------------------
/*!
   \param recording recording enabled or disabled flag
*/
void GamePlayback::setRecording(bool recording)
{
   mRecording = recording;

   if (isRecording())
   {
      clearPacketQueue();
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if recording
*/
bool GamePlayback::isRecording() const
{
   return mRecording;
}


//-----------------------------------------------------------------------------
/*!
   \param loading level loading flag
*/
void GamePlayback::setLevelLoading(bool loading)
{
   mLevelLoading = loading;
}


//-----------------------------------------------------------------------------
/*!
*/
bool GamePlayback::isLevelLoading() const
{
   return mLevelLoading;
}


//-----------------------------------------------------------------------------
/*!
*/
int GamePlayback::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::replay()
{
   if (!mPacketQueue.isEmpty())
   {
      setReplaying(true);

      mStartTime = mPacketQueue.first()->getTimestamp();
      mElapsed.restart();

      update();
   }
   else
   {
      qDebug("GamePlayback::replay(): buffer is empty");
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::record(Packet *packet)
{
   mPacketQueue << packet;
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::clearPacketQueue()
{
   qDeleteAll(mPacketQueue);
   mPacketQueue.clear();
}


//-----------------------------------------------------------------------------
/*!
   \param givenName name to save to
*/
void GamePlayback::save(const QString &givenName)
{
   QString filename = givenName;

   if (filename.isNull())
   {
      filename =
         QString("playback-%1.dbr").arg(
            QTime::currentTime().toString().replace(':','_')
         );
   }

   QFile file(filename);

   if (file.open(QIODevice::WriteOnly))
   {
      foreach (Packet* packet, mPacketQueue)
      {
         packet->serialize();
         file.write(*packet);
      }

      qDebug("GamePlayback::save: %s dumped", qPrintable(filename));
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to interpolate
*/
void GamePlayback::smoothInterpolationData(Packet* packet)
{
   PositionPacket* prevPos= nullptr;
   PositionPacket* pos= dynamic_cast<PositionPacket*>(packet);

   int playerId= pos->getPlayerId();
   QMap<int, PositionPacket*>::Iterator it;
   it= mPrevPositions.find( playerId );
   if ( it != mPrevPositions.constEnd() )
   {
      prevPos = it.value();
      *it= pos;
   }
   else
   {
      prevPos= nullptr;
      mPrevPositions.insert( playerId, pos );
   }

   if (prevPos)
   {
      // number of server heartbeats between two packets
      int time= prevPos->getTimestamp().msecsTo( pos->getTimestamp() );
      double serverTicks = time * SERVER_HEARTBEAT_IN_HZ / 1000.0;

      double pdx = static_cast<double>(prevPos->getDeltaX());
      double pdy = static_cast<double>(prevPos->getDeltaY());
      double pdr = static_cast<double>(prevPos->getAngleDelta());

      double dx = static_cast<double>(pos->getX() - prevPos->getX());
      double dy = static_cast<double>(pos->getY() - prevPos->getY());
      double dr = static_cast<double>(pos->getAngle() - prevPos->getAngle());

      // fix rotation delta direction
      if (dr > M_PI)
         dr = M_PI * 2.0 - dr;

      if (dr < -M_PI)
         dr = M_PI * 2.0 + dr;

      if (dr > 0.5 || dr <- 0.5)
      {
         dr = 0.0;
      }

      dx *= serverTicks;
      dy *= serverTicks;
      dr *= serverTicks;

      if ( (dx!=0.0) || (dy!=0.0) || (pdx!=0.0) | (pdy!=0.0) )
      {
         qDebug("[%d] %d: %.3f,%.3f,%.3f -> %.3f,%.3f,%.3f (%.2f) ",
            time,
            playerId,
            pdx,
            pdy,
            pdr,
            dx,
            dy,
            dr,
            serverTicks
         );
      }

      if (time < 100)
      {
         prevPos->setDeltaX( dx );
         prevPos->setDeltaY( dy );
         prevPos->setAngleDelta( static_cast<float>(dr) );
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param filename filename to load
*/
void GamePlayback::load(const QString &filename)
{
   mPrevPositions.clear();
   QFile file(filename);

   if (file.open(QIODevice::ReadOnly))
   {
      mFilename = filename;

      QDataStream in(&file);
      in.setVersion(QDataStream::Qt_4_6);

      bool error = false;

      while (packetAvailable(file, in))
      {
         // block was read completely
         Packet* packet = Packet::deserialize(in);

         if (packet)
         {
            mPacketQueue.push_back(packet);

            // smooth interpolation data
            // if (packet->getType() == Packet::POSITION)
            //   smoothInterpolationData(packet);
         }
         else
         {
            error = true;
         }

         mBlockSize = 0;
      }

      if (error)
      {
         qWarning("GamePlayback::load: incompatible packets detected");
         clearPacketQueue();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream
   \return true if sufficient data was received
*/
bool GamePlayback::packetAvailable(const QFile& file, QDataStream& in)
{
   // blocksize not initialized yet
   if (mBlockSize == 0)
   {
      // not enough data to read blocksize?
      if (file.bytesAvailable() < static_cast<int32_t>(sizeof(quint16))) // yeah, yeah, yeah... '2' is probably more concise.
        return false;

     // read blocksize
      in >> mBlockSize;
   }

   // enough data?
   return (file.bytesAvailable() >= mBlockSize);
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::updateCheckpoint()
{
   QMap<int, QList<PlaybackCheckpoint> >::iterator it;
   it = mCheckPoints.find(mPacketFilter.getGameId());
   if (it != mCheckPoints.end())
   {
      int round = mPacketFilter.getRoundCounter();

      if (round < it.value().size())
         it.value()[round].setVisited(true);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void GamePlayback::processPacket(Packet* packet)
{
   switch (packet->getType())
   {
      case Packet::JOINGAMERESPONSE:
      {
         JoinGameResponsePacket* joinResponse = dynamic_cast<JoinGameResponsePacket*>(packet);

         // only do the loading stuff for ourself
         if (joinResponse->getPlayerId() == getPlayerId())
         {
            // level loading starts here;
            // we need to interrupt until the level is actually loaded!
            setLevelLoading(true);
            mLevelLoadingTime.restart();
         }

         break;
      }

      default:
      {
         break;
      }
   }

   emit playBack(packet);
}


//-----------------------------------------------------------------------------
/*!
   \return checkpoint time for current checkpoint
*/
int GamePlayback::getCheckpointTime()
{
   // if the packet filter is valid (i.e. used), we use it to jump to the
   // next checkpoint. the next checkpoint is enabled whenever a game starts.
   // as soon it is enabled, we can skip the whole game
   int checkpointTime = 0;

   if (mPacketFilter.isValid())
   {
      int gameId = mPacketFilter.getGameId();

      QList<PlaybackCheckpoint>& cps = mCheckPoints[gameId];
      int round = 0;
      foreach (PlaybackCheckpoint cp, cps)
      {
         if (cp.isVisited() && (round < mPacketFilter.getRoundNumber()))
         {
            checkpointTime = cp.getElapsed();
            round++;
         }
         else
         {
            break;
         }
      }
   }

   return checkpointTime;
}



//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::update()
{
   /*

      |-------------------\/------\/-----\/--------------|--------------------|
    start               event_1  ....  event_n          now                  end

   */

   int event = 0;
   int now = static_cast<int32_t>(mElapsed.elapsed());
   Packet* packet = nullptr;
   bool packetFiltered = false;

   // shift the current time by the checkpoint time in case a filter is used
   now += getCheckpointTime();

   while (!mPacketQueue.isEmpty())
   {
      packet = mPacketQueue.first();
      event = mStartTime.msecsTo(packet->getTimestamp());

      // check if packet filter needs to be activated
      if (mPacketFilter.isValid())
      {
         updatePacketFilter(packet);
         packetFiltered = mPacketFilter.isEnabled();
      }

      // next event is after current time => wait
      if (event > now)
         break;

      mPacketQueue.removeFirst();

      // process packets either if we don't filter or while the filter is disabled
      if (!packetFiltered)
      {
         processPacket(packet);
      }
      else
      {
         // if the packet has been filtered, it should be destroyed now
         delete packet;
      }
   }

   if (mPacketQueue.isEmpty())
   {
      setReplaying(false);
      emit finished();
   }

   // update is disabled while the level is loaded
   if (isReplaying() && !isLevelLoading())
   {
      TimerHandler::singleShot(10, this, SLOT(update()));
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::playDemo()
{
   if (!isReplaying())
   {
      qDebug("GamePlayback::playDemo()");

      reset();

      load("data/demo/demo.dr");

      analyze();

      mPacketFilter.setGameId(1);
      mPacketFilter.setRoundNumber(0);

      replay();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::abort()
{
   setReplaying(false);
   clearPacketQueue();
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::levelLoadingFinished()
{
   if (!isRecording())
   {
      int elapsed = static_cast<int32_t>(mLevelLoadingTime.elapsed());

      // add a second, you never know ;)
      // elapsed += 1000;

      qDebug(
         "GamePlayback::levelLoadingFinished(): %dms",
         elapsed
      );

      // if replay was active -> continue
      if (isLevelLoading())
      {
         setLevelLoading(false);

         // correct all subsequent packet timestamps
         foreach(Packet* p, mPacketQueue)
         {
            QTime stamp= p->getTimestamp();
            stamp= stamp.addMSecs(elapsed);
            p->setTimeStamp(stamp);
         }

         update();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::setPlayerId(int id)
{
   mPlayerId = id;
}


//-----------------------------------------------------------------------------
/*!
*/
bool GamePlayback::isReplaying() const
{
   return mReplaying;
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::setReplaying(bool value)
{
   mReplaying = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::unitTestRoundReplay()
{
   setRecording(false);
   setLevelLoading(false);
   clearPacketQueue();

   load("playback-19_02_44.dbr");

   analyze();

   mPacketFilter.setGameId(1);
   mPacketFilter.setRoundNumber(0);

   replay();
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::analyze()
{
   if (!mPacketQueue.isEmpty())
   {
      qDebug("File opened:     %s", qPrintable(mFilename));
      qDebug("Packet count:    %d", mPacketQueue.size());

      qDebug(
         "Time range:      %s - %s",
         qPrintable(mPacketQueue.first()->getTimestamp().toString()),
         qPrintable(mPacketQueue.last()->getTimestamp().toString())
      );

      int ourPlayerId = -1;
      QList<int> gamesAvailable;
      foreach (Packet* packet, mPacketQueue)
      {
         switch (packet->getType())
         {
            case Packet::LOGINRESPONSE:
            {
               LoginResponsePacket* lrp = dynamic_cast<LoginResponsePacket*>(packet);
               ourPlayerId = lrp->getId();
               break;
            }

            case Packet::JOINGAMERESPONSE:
            {
               JoinGameResponsePacket* jgrp = dynamic_cast<JoinGameResponsePacket*>(packet);
               if (jgrp->getPlayerId() == ourPlayerId)
                  gamesAvailable << jgrp->getGameId();
               break;
            }

            default:
               break;
         }
      }

      QStringList gameIds;
      foreach (int gameId, gamesAvailable)
         gameIds << QString::number(gameId);

      qDebug(
         "Games available: %s [count = %d]",
         qPrintable(gameIds.join("; ")),
         gameIds.size()
      );

      qDebug("\nTimeline");
      qDebug("--------------------------------------------------------------------------------");

      QMap<int, int> startedRoundMap;
      foreach (Packet* packet, mPacketQueue)
      {
         switch (packet->getType())
         {
            case Packet::CREATEGAMERESPONSE:
            {
               CreateGameResponsePacket* cgrp = dynamic_cast<CreateGameResponsePacket*>(packet);

               qDebug(
                  "[%s] Game created: %d",
                  qPrintable(packet->getTimestamp().toString()),
                  cgrp->getGameInformation().getId()
               );

               break;
            }

            case Packet::JOINGAMERESPONSE:
            {
               JoinGameResponsePacket* jgrp = dynamic_cast<JoinGameResponsePacket*>(packet);

               qDebug(
                  "[%s] Player '%s' joined game %d",
                  qPrintable(packet->getTimestamp().toString()),
                  qPrintable(jgrp->getNick()),
                  jgrp->getGameId()
               );

               break;
            }

            case Packet::STARTGAMERESPONSE:
            {
               StartGameResponsePacket* sgrp = dynamic_cast<StartGameResponsePacket*>(packet);

               int round = startedRoundMap[sgrp->getId()];

               qDebug(
                  "[%s] Round %d of game %d started",
                  qPrintable(packet->getTimestamp().toString()),
                  round,
                  sgrp->getId()
               );

               startedRoundMap[sgrp->getId()]++;


               int gameId = sgrp->getId();

               // lookup or insert checkpoint data
               // a checkpoint time is equal to the time a new round begins
               QMap<int, int>::iterator it = mRoundMap.find(gameId);

               if (it != mRoundMap.end())
               {
                  // add one checkpoint per round
                  // also pass the elapsed time to that round
                  QTime startTime = mPacketQueue.first()->getTimestamp();
                  int dt = abs(sgrp->getTimestamp().msecsTo(startTime));

                  // this is a bit lame, we jump back the countdown lenght
                  // back in time because i don't wanna parse the countdown packets here, too
                  dt -= (SERVER_PREPARATION_TIME + SERVER_PREPARATION_SYNC_TIME - 1) * 1000;

                  addCheckpoint(gameId, dt);
               }
               else
               {
                  mCheckPoints.insert(gameId, QList<PlaybackCheckpoint>());
               }

               break;
            }

            case Packet::STOPGAMERESPONSE:
            {
               StopGameResponsePacket* sgrp = dynamic_cast<StopGameResponsePacket*>(packet);

               int gameId = sgrp->getId();

               QMap<int, int>::iterator it = mRoundMap.find(gameId);

               int round = 0;
               if (it != mRoundMap.end())
               {
                  it.value()++;
               }
               else
               {
                  mRoundMap.insert(gameId, 0);
               }

               qDebug(
                  "[%s] Round %d of game %d stopped",
                  qPrintable(packet->getTimestamp().toString()),
                  round,
                  gameId
               );

               break;
            }

            default:
               break;
         }
      }
   }
   else
   {
      qDebug("Packet queue is empty.");
   }
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to process
*/
void GamePlayback::updatePacketFilter(Packet* packet)
{
   // update game running state
   //
   // if we receive "MapItemRemove"-packets outside a running game
   // they are supposed to be processed as they are used to clean up
   // the game arena
   if (packet->getType() == Packet::STOPGAMERESPONSE)
   {
      mPacketFilter.setRunning(false);
   }

   // server starts to clean up the map
   // this happens right before the countdown
   //
   // whenenver we receive this packet type outside a running game
   // we can be sure a new game will now be started which we want
   // to skip.
   else if (packet->getType() == Packet::MAPITEMREMOVED)
   {
      if (!mPacketFilter.isRunning())
      {
         mPacketFilter.setEnabled(
            !mPacketFilter.isRoundReached()
         );
      }
   }

   // increase round counter once per round.
   // this is done to be able to match the game/round which has been selected
   // also the "running" state is updated which is used to determine whether
   // to interpret "MapItemRemove"-packets or not.
   else if (packet->getType() == Packet::COUNTDOWN)
   {
      mPacketFilter.setRunning(true);

      CountdownPacket* cp = dynamic_cast<CountdownPacket*>(packet);

      int timeLeft = cp->getTimeLeft();

      if (timeLeft == SERVER_PREPARATION_TIME + SERVER_PREPARATION_SYNC_TIME - 1)
      {
         qDebug(
            "GamePlayback::updatePacketFilter: new round %d (%s)",
            mPacketFilter.getRoundCounter(),
            qPrintable(QTime::currentTime().toString("hh:mm:ss.zzz"))
         );

         // very important step:
         // if we have a checkpoint we add to our elapsed time, we need to
         // reset our elapsed time at first. however this does not apply to the
         // first checkpoint we come across :)
         if (getCheckpointTime() > 0)
            mElapsed.restart();

         // the checkpoint is set in order to skip all packets
         // until the next round is reached
         updateCheckpoint();

         mPacketFilter.increaseRoundCounter();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param gameId game id
   \param dt delta time
*/
void GamePlayback::addCheckpoint(int gameId, int dt)
{
   PlaybackCheckpoint cp;
   cp.setElapsed(dt);
   mCheckPoints[gameId].append(cp);
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayback::clearCheckpoints()
{
   mCheckPoints.clear();
}


