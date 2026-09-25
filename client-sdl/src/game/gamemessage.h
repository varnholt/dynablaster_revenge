#pragma once

// GLES3 port of client/src/game/gamemessage.cpp.

#include "framework/frametimer.h"

#include <QObject>
#include <QString>

class GameMessage : public QObject
{
   Q_OBJECT

public:
   GameMessage(QObject* parent = nullptr);

   void operator=(const GameMessage&);
   GameMessage(const GameMessage& message);

   void setMessage(const QString&);

   static void setDisplayTime(int time);

   virtual void initialize() {};

signals:
   void expired();

protected:
   QString mMessage;
   FrameTimer mTime;
   int mSenderId;
   int mReceiverId;
   QString mSenderName;
   QString mReceiverName;

   static int sDisplayTime;
};
