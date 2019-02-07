#ifndef INFECTIONANIMATION_H
#define INFECTIONANIMATION_H

// Qt
#include <QObject>

// shared
#include "constants.h"
#include "framework/frametimer.h"

// forward declarations
class PlayerItem;

class InfectionAnimation : public QObject
{
   Q_OBJECT

public:

   explicit InfectionAnimation(QObject *parent = 0);

   void setPlayerItem(PlayerItem* item);
   void setSkullType(Constants::SkullType skullType);

   PlayerItem* getPlayerItem() const;
   int getDuration() const;
   Constants::SkullType getSkullType() const;


signals:
   
public slots:

   void start();


protected slots:

   void animate();


protected:

   PlayerItem* mPlayerItem;
   int mDuration;
   Constants::SkullType mSkullType;

   FrameTimer mTimer;
};

#endif // INFECTIONANIMATION_H
