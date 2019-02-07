// header
#include "infectionanimation.h"

// game
#include "playeritem.h"


InfectionAnimation::InfectionAnimation(QObject *parent) :
   QObject(parent),
   mPlayerItem(0),
   mDuration(0),
   mSkullType(Constants::SkullAutofire)
{
}


void InfectionAnimation::setPlayerItem(PlayerItem *item)
{
   mPlayerItem = item;
}


void InfectionAnimation::setSkullType(Constants::SkullType skullType)
{
   mSkullType = skullType;
}


PlayerItem *InfectionAnimation::getPlayerItem() const
{
   return mPlayerItem;
}


Constants::SkullType InfectionAnimation::getSkullType() const
{
   return mSkullType;
}


void InfectionAnimation::start()
{
   connect(
      &mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(animate())
   );

   mTimer.start(500);

   animate();
}


void InfectionAnimation::animate()
{
   PlayerItem* player= getPlayerItem();
   if (player)
   {
      if (!player->isKilled())
         player->setFlash(1.0f);
   }
}

