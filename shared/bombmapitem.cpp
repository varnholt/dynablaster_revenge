
// header
#include "bombmapitem.h"

// shared
#include "bombkickanimation.h"

// static variables
int BombMapItem::sTickTime = 0;


//-----------------------------------------------------------------------------
/*!
   \param playerId player's id
   \param intensity bomb intensity (flame count)
*/
BombMapItem::BombMapItem(
   int playerId,
   int flames,
   int id,
   int x,
   int y
)
   : MapItem(Bomb, id, true, false, x, y),
     mPlayerId(playerId),
     mFlames(flames),
     mKicked(false),
     mAnimation(0),
     mDetonationOrigin(Active),
     mShadowedItem(0),
     mIgniterId(-1)
{
   connect(
      &mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(explodeActive())
   );

   mTimer.start(getTickTime());
}


//-----------------------------------------------------------------------------
/*!
*/
BombMapItem::~BombMapItem()
{
   mTimer.stop();

   // delete running animations in any case
   if (mAnimation)
   {
      mAnimation->deleteLater();
      mAnimation = 0;
   }
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
qint8 BombMapItem::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \return flames
*/
qint8 BombMapItem::getFlames() const
{
   return mFlames;
}


//-----------------------------------------------------------------------------
/*!
*/
void BombMapItem::explodeActive()
{
   if (!mKicked)
   {
      emit exploded(this, false);
   }
}



//-----------------------------------------------------------------------------
/*!
*/
void BombMapItem::stopTimer()
{
   mTimer.stop();
}


//-----------------------------------------------------------------------------
/*!
   \param map map to work on
   \param dir direction the bomb is kicked to
*/
void BombMapItem::kick()
{
   // only connect these signals and slots once
   if (!isKicked())
   {
      connect(
         mAnimation,
         SIGNAL(started(Constants::Direction,float)),
         this,
         SIGNAL(kickAnimation(Constants::Direction,float))
      );

      connect(
         mAnimation,
         SIGNAL(stopped()),
         this,
         SIGNAL(kickAnimation())
      );

      // notify animation when bomb exploded
      connect(
         &mTimer,
         SIGNAL(timeout()),
         mAnimation,
         SLOT(readyToExplode())
      );

      // and notify map item back when animation reached the center of a field
      connect(
         mAnimation,
         SIGNAL(explode()),
         this,
         SLOT(explodeDelayed())
      );

      setKicked(true);
   }

   // start kick animation
   mAnimation->setX(getX() + 0.5f);
   mAnimation->setY(getY() + 0.5f);
   mAnimation->start();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombMapItem::explodeDelayed()
{
   emit exploded(this, false);

   if (isKicked())
   {
      mAnimation->deleteLater();
      mAnimation = 0;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param origin detonation origin
*/
void BombMapItem::setDetonationOrigin(DetonationOrigin origin)
{
   mDetonationOrigin = origin;
}


//-----------------------------------------------------------------------------
/*!
   \return detonation origin
*/
BombMapItem::DetonationOrigin BombMapItem::getDetonationOrigin() const
{
   return mDetonationOrigin;
}


//-----------------------------------------------------------------------------
/*!
  \param ms timer interval
*/
void BombMapItem::setInterval(int ms)
{
   mTimer.setInterval(ms);
}


//-----------------------------------------------------------------------------
/*!
   \return timer interval
*/
int BombMapItem::getInterval() const
{
   return mTimer.interval();
}


//-----------------------------------------------------------------------------
/*!
  \param id player id
*/
void BombMapItem::setPlayerId(int id)
{
   mPlayerId = id;
}


//-----------------------------------------------------------------------------
/*!
  \return \c true if item is kicked
*/
bool BombMapItem::isKicked() const
{
   return mKicked;
}


//-----------------------------------------------------------------------------
/*!
   \param kicked kicked flag
*/
void BombMapItem::setKicked(bool kicked)
{
   mKicked = kicked;
}


//-----------------------------------------------------------------------------
/*!
   \param shadowedItem item that has been shadowed by a bomb
*/
void BombMapItem::setShadowedItem(MapItem *shadowedItem)
{
   mShadowedItem = shadowedItem;
}


//-----------------------------------------------------------------------------
/*!
   \param id igniter id
*/
void BombMapItem::setIgniterId(qint8 id)
{
   mIgniterId = id;
}


//-----------------------------------------------------------------------------
/*!
   \return igniter id
*/
qint8 BombMapItem::getIgniterId() const
{
   return mIgniterId;
}


//-----------------------------------------------------------------------------
/*!
   \return item that has been shadowed by a bomb
*/
MapItem *BombMapItem::getShadowedItem()
{
   return mShadowedItem;
}


//-----------------------------------------------------------------------------
/*!
   \return bomb kick animation
*/
BombKickAnimation *BombMapItem::getBombKickAnimation() const
{
   return mAnimation;
}


//-----------------------------------------------------------------------------
/*!
   \param shadowedItem item that has been shadowed by a bomb
*/
void BombMapItem::setBombKickAnimation(BombKickAnimation *animation)
{
   mAnimation = animation;
}


//-----------------------------------------------------------------------------
/*!
   \param time tick time
*/
void BombMapItem::setTickTime(int time)
{
   sTickTime = time;
}


//-----------------------------------------------------------------------------
/*!
   \return tick time
*/
int BombMapItem::getTickTime()
{
   return sTickTime;
}

