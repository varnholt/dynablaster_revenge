#ifndef SHAKEPACKETHANDLER_H
#define SHAKEPACKETHANDLER_H

#include <QObject>

// forward declarations
class Game;
class QTimer;


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

   QTimer* mCheckTimer;
};

#endif // SHAKEPACKETHANDLER_H
