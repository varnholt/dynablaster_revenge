// GLES3 port of client/src/game/gamemessage.cpp.

#include "gamemessage.h"
#include "framework/timerhandler.h"

int GameMessage::sDisplayTime = 10000;

GameMessage::GameMessage(QObject* parent) : QObject(parent), mSenderId(-1), mReceiverId(-1)
{
   mTime = FrameTimer::currentTime();

   TimerHandler::singleShot(sDisplayTime, [this]() { emit expired(); });
}

void GameMessage::operator=(const GameMessage& message)
{
   mMessage = message.mMessage;
   mTime = message.mTime;
   mSenderId = message.mSenderId;
   mReceiverId = message.mReceiverId;
   mReceiverName = message.mReceiverName;
   mSenderName = message.mSenderName;
}

GameMessage::GameMessage(const GameMessage& message) : QObject(message.parent())
{
   mMessage = message.mMessage;
   mTime = message.mTime;
   mSenderId = message.mSenderId;
   mReceiverId = message.mReceiverId;
   mReceiverName = message.mReceiverName;
   mSenderName = message.mSenderName;

   TimerHandler::singleShot(sDisplayTime, [this]() { emit expired(); });
}

void GameMessage::setMessage(const std::string& message)
{
   mMessage = message;
}

void GameMessage::setDisplayTime(int time)
{
   sDisplayTime = time;
}
