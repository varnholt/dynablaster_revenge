
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
BombMapItem::BombMapItem(int playerId, int flames, int id, int x, int y)
    : MapItem(Bomb, id, true, false, x, y),
      mPlayerId(playerId),
      mFlames(flames),
      mKicked(false),
      mAnimation(nullptr),
      mDetonationOrigin(Active),
      mShadowedItem(nullptr),
      mIgniterId(-1)
{
   mTimer.timeoutSignal.connect([this]() { explodeActive(); });

   mTimer.start(getTickTime());
}

//-----------------------------------------------------------------------------
/*!
 */
BombMapItem::~BombMapItem()
{
   mTimer.stop();

   // mAnimation (unique_ptr) cleans itself up automatically; safe to do synchronously here -
   // this destructor never runs nested inside mAnimation's own signal dispatch (unlike
   // explodeDelayed(), see below), only from deferred teardown or whole-map destruction.
}

//-----------------------------------------------------------------------------
/*!
   \return player id
*/
int8_t BombMapItem::getPlayerId() const
{
   return mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   \return flames
*/
int8_t BombMapItem::getFlames() const
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
      explodedSignal(this, false);
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
      mAnimation->startedSignal.connect([this](Constants::Direction direction, float speed) { kickAnimationSignal(direction, speed); });

      // stopped() used to relay into kickAnimation()'s 2-arg signal via Qt's own
      // fewer-args-fills-defaults connect() behavior - replicate the same defaults explicitly.
      mAnimation->stoppedSignal.connect([this]() { kickAnimationSignal(Constants::DirectionUnknown, 0.0f); });

      // notify animation when bomb exploded - reads mAnimation live (not a captured snapshot)
      // so QPointer's auto-null still protects against mAnimation being destroyed later
      mTimer.timeoutSignal.connect(
         [this]()
         {
            if (mAnimation)
            {
               mAnimation->readyToExplode();
            }
         }
      );

      // and notify map item back when animation reached the center of a field
      mAnimation->explodeSignal.connect([this]() { explodeDelayed(); });

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
   explodedSignal(this, false);

   if (isKicked())
   {
      // this runs synchronously from inside mAnimation's own explodeSignal dispatch -
      // destroying mAnimation right here would destroy the object while one of its own
      // methods is still on the call stack. Defer to the next tick instead (same cadence
      // deleteLater() used to provide) - std::shared_ptr since Timer::singleShot's
      // std::function needs a copyable target, a moved-in unique_ptr wouldn't compile.
      Timer::singleShot(0, [anim = std::shared_ptr<BombKickAnimation>(std::move(mAnimation))]() {});
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
void BombMapItem::setShadowedItem(MapItem* shadowedItem)
{
   mShadowedItem = shadowedItem;
}

//-----------------------------------------------------------------------------
/*!
   \param id igniter id
*/
void BombMapItem::setIgniterId(int8_t id)
{
   mIgniterId = id;
}

//-----------------------------------------------------------------------------
/*!
   \return igniter id
*/
int8_t BombMapItem::getIgniterId() const
{
   return mIgniterId;
}

//-----------------------------------------------------------------------------
/*!
   \return item that has been shadowed by a bomb
*/
MapItem* BombMapItem::getShadowedItem()
{
   return mShadowedItem;
}

//-----------------------------------------------------------------------------
/*!
   \return bomb kick animation
*/
BombKickAnimation* BombMapItem::getBombKickAnimation() const
{
   return mAnimation.get();
}

//-----------------------------------------------------------------------------
/*!
   \param animation kick animation, ownership transfers to this bomb
*/
void BombMapItem::setBombKickAnimation(std::unique_ptr<BombKickAnimation> animation)
{
   mAnimation = std::move(animation);
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
