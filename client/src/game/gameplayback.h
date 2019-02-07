#ifndef GAMEPLAYBACK_H
#define GAMEPLAYBACK_H

// Qt
#include <QFile>
#include <QMap>
#include <QObject>
#include <QQueue>
#include <QTime>

// game
#include "playbackcheckpoint.h"
#include "playbackpacketfilter.h"
#include "framework/frametimer.h"

// forward declarations
class Packet;
class PositionPacket;


class GamePlayback : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      GamePlayback(QObject* parent = 0);

      //! static instance getter
      static GamePlayback* getInstance();

      //! check  if replay is running
      bool isReplaying() const;

      //! setter for replaying flag
      void setReplaying(bool value);

      //! setter for recording flag
      void setRecording(bool recording);

      //! getter for recording flag
      bool isRecording() const;

      //! setter for level loading flag
      void setLevelLoading(bool loaded);

      //! getter for level loading flag
      bool isLevelLoading() const;

      //! getter for player id
      int getPlayerId() const;


   public slots:

      //! start replay
      void replay();

      //! record a packet
      void record(Packet* packet);

      //! reset replay
      void clearPacketQueue();

      //! save playback data
      void save(const QString& filename = QString::null);

      //! load playback data
      void load(const QString& filename);

      //! start demo
      void playDemo();

      //! abort demo
      void abort();

      //! level loading has been finished
      void levelLoadingFinished();

      //! setter for player id
      void setPlayerId(int id);

      //! analyze packets
      void analyze();

      void updateCheckpoint();

      // unit tests

      //! unit test round replay
      void unitTestRoundReplay();


   signals:

      //! playback a packet
      void playBack(Packet* packet);

      //! playback is finished
      void finished();

   protected slots:

      //! calls itself internally
      void update();


   protected:

      //! update packet filter
      void updatePacketFilter(Packet* packet);

      //! process a packet
      void processPacket(Packet* packet);

      //! check for packets
      bool packetAvailable(const QFile &file, QDataStream& source);

      //! get the current checkpoint time (if the filter is active)
      int getCheckpointTime();

      //! add a checkpoint in form of time-startime
      void addCheckpoint(int gameId, int dt);

      //! clear all checkpoints
      void clearCheckpoints();

      //! smooth interpolation data
      void smoothInterpolationData(Packet* packet);

      //! reset state
      void reset();

      //! blocksize of packet which is received from server
      quint16 mBlockSize;

      //! playback start time
      QTime mStartTime;

      //! playback timer
      FrameTimer mTimer;

      //! elapsed timer
      FrameTimer mElapsed;

      //! level loading time used to shift packet timestamps
      FrameTimer mLevelLoadingTime;

      //! packet queue (list of all packets)
      QQueue<Packet*> mPacketQueue;

      //! recording flag
      bool mRecording;

      //! replaying flag
      bool mReplaying;

      //! static replay instance
      static GamePlayback* sInstance;

      //! level loading flag
      bool mLevelLoading;

      //! player id
      int mPlayerId;

      //! filename used to load/save
      QString mFilename;

      //! game id and its rounds
      QMap<int, int> mRoundMap;

      //! game id and its playback checkpoints
      QMap<int, QList<PlaybackCheckpoint> > mCheckPoints;

      //! filter that can be applied to limit the play range to a certain game/round
      PlaybackPacketFilter mPacketFilter;

      //! previous position map
      QMap<int, PositionPacket*> mPrevPositions;
};

#endif // GAMEPLAYBACK_H
