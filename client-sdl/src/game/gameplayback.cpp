#include "gameplayback.h"

GamePlayback* GamePlayback::sInstance = nullptr;

GamePlayback::GamePlayback(QObject* parent)
   : QObject(parent)
   , mRecording(false)
   , mReplaying(false)
{
   sInstance = this;
}

GamePlayback* GamePlayback::getInstance()
{
   if (!sInstance)
      new GamePlayback();

   return sInstance;
}

bool GamePlayback::isReplaying() const
{
   return mReplaying;
}

void GamePlayback::setReplaying(bool value)
{
   mReplaying = value;
}

void GamePlayback::setRecording(bool recording)
{
   mRecording = recording;
}

bool GamePlayback::isRecording() const
{
   return mRecording;
}

void GamePlayback::record(Packet*) {}
void GamePlayback::playDemo() {}
void GamePlayback::abort() {}
void GamePlayback::setPlayerId(int) {}
