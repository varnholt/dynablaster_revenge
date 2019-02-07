// header
#include "gamemessage.h"
#include "framework/timerhandler.h"

// static variables
int GameMessage::sDisplayTime=10000;


GameMessage::GameMessage(QObject* parent)
   : QObject(parent),
     mSenderId(-1),
     mReceiverId(-1)
{
   mTime = FrameTimer::currentTime();

   // notify others when message display time is expired
   TimerHandler::singleShot(
      sDisplayTime,
      this,
      SIGNAL(expired())
   );
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


GameMessage::GameMessage(const GameMessage& message)
   : QObject(message.parent())
{
   mMessage = message.mMessage;
   mTime = message.mTime;
   mSenderId = message.mSenderId;
   mReceiverId = message.mReceiverId;
   mReceiverName = message.mReceiverName;
   mSenderName = message.mSenderName;

   // notify others when message display time is expired
   TimerHandler::singleShot(
      sDisplayTime,
      this,
      SIGNAL(expired())
   );
}


void GameMessage::setMessage(const QString& message)
{
   mMessage = message;
}


void GameMessage::setDisplayTime(int time)
{
   sDisplayTime = time;
}


