#ifndef GAMEPLAYBACK_H
#define GAMEPLAYBACK_H

// Qt
#include <QObject>

// forward declarations
class Packet;

// Deferred subsystem (see project memory - Phase 4): the real GamePlayback
// (client/src/game/gameplayback.{h,cpp}, 1109 lines) is the replay/record system - not required
// to get live client<->server play working. No-op stand-in with the exact real interface
// BombermanClient calls into, not a reimplementation. isRecording()/isReplaying() always false,
// so BombermanClient's playback-gated branches simply behave as "no playback in progress".
class GamePlayback : public QObject
{
   Q_OBJECT

public:

   GamePlayback(QObject* parent = nullptr);

   static GamePlayback* getInstance();

   bool isReplaying() const;
   void setReplaying(bool value);
   void setRecording(bool recording);
   bool isRecording() const;


public slots:

   void record(Packet* packet);
   void playDemo();
   void abort();
   void setPlayerId(int id);


signals:

   void playBack(Packet* packet);
   void finished();


protected:

   static GamePlayback* sInstance;

   bool mRecording;
   bool mReplaying;
};

#endif // GAMEPLAYBACK_H
