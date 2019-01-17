#pragma once

#include <QTcpSocket>

class Packet;

class DummyPlayer : public QTcpSocket
{
   Q_OBJECT

public:
   DummyPlayer(const QString& nickName, int gameId);
   ~DummyPlayer();

private slots:
   void onConnect();
   void onDisconnect();
   void onRead();

private:
   void send(Packet& pack);

   QString mNick;
   int mGameId;
};
