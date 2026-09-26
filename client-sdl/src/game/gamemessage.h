#pragma once

// GLES3 port of client/src/game/gamemessage.cpp.

#include "framework/frametimer.h"

#include <QObject>

#include <string>

class GameMessage : public QObject
{
   Q_OBJECT

public:
   GameMessage(QObject* parent = nullptr);

   void operator=(const GameMessage&);
   GameMessage(const GameMessage& message);

   void setMessage(const std::string&);

   static void setDisplayTime(int time);

   virtual void initialize() {};

signals:
   void expired();

protected:
   std::string mMessage;
   FrameTimer mTime;
   int mSenderId;
   int mReceiverId;
   std::string mSenderName;
   std::string mReceiverName;

   static int sDisplayTime;
};
