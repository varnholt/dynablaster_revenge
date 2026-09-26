#ifndef SHAKEPACKETHANDLER_H
#define SHAKEPACKETHANDLER_H

#include <QObject>

// shared
#include "timer.h"

// forward declarations
class Game;


class ExtraShakePacketHandler : public QObject
{
   Q_OBJECT

public:

   ExtraShakePacketHandler(QObject *parent = 0);

   void setEnabled(bool enabled);

   void setGame(Game* game);
   Game* getGame() const;
    
signals:
    
public slots:

protected slots:

   void check();


protected:


   Game* mGame;

   Timer mCheckTimer;
};

#endif // SHAKEPACKETHANDLER_H
