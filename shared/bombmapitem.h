#ifndef BOMBMAPITEM_H
#define BOMBMAPITEM_H

// base
#include "mapitem.h"
#include "signal.h"

// Qt
#include <QObject>
#include <QPointer>

// shared
#include "timer.h"

// forward declarations
class BombKickAnimation;
class Map;

class BombMapItem : public MapItem
{
   Q_OBJECT

public:
   //! detonation origin if bomb detonates passively
   enum DetonationOrigin
   {
      Active = 0,
      Left = 1,
      Right = 2,
      Top = 3,
      Bottom = 4
   };

   //! constructor
   BombMapItem(int playerId, int flames, int id, int x, int y);

   //! destructor
   virtual ~BombMapItem();

   //! getter for player id
   int8_t getPlayerId() const;

   //! getter for flames
   int8_t getFlames() const;

   //! bomb is kicked
   void kick();

   //! setter for detonation origin
   void setDetonationOrigin(DetonationOrigin origin);

   //! getter for detonation origin
   DetonationOrigin getDetonationOrigin() const;

   //! set detonation interval
   void setInterval(int ms);

   //! get detonation interval
   int getInterval() const;

   //! setter for owner id
   void setPlayerId(int id);

   //! getter for kicked flag
   bool isKicked() const;

   //! setter for kicked flag
   void setKicked(bool kicked);

   //! setter for shadowed item
   void setShadowedItem(MapItem* shadowedItem);

   //! setter for igniter id
   void setIgniterId(int8_t id);

   //! getter for igniter id
   int8_t getIgniterId() const;

   //! getter for shadowed item
   MapItem* getShadowedItem();

   //! getter for kick animation
   BombKickAnimation* getBombKickAnimation() const;

   //! setter for bomb kick animation
   void setBombKickAnimation(BombKickAnimation* animation);

   //! setter for tick time
   static void setTickTime(int time);

   //! getter for tick time
   static int getTickTime();

public slots:

   //! let the bomb explode now
   void stopTimer();

public:
   // Signal<> replacements for BombMapItem's former Qt signals (see
   // project_full_qt_removal_scope memory).

   //! bomb exploded
   Signal<BombMapItem*, bool> explodedSignal;

   //! kick animation was started or stopped
   Signal<Constants::Direction, float> kickAnimationSignal;

protected slots:

   //! active explosion
   void explodeActive();

   //! delayed explosion triggered from animation
   void explodeDelayed();

protected:
   //! bomb's timer
   Timer mTimer;

   //! bomb owner
   int8_t mPlayerId;

   //! bomb flames
   int8_t mFlames;

   //! kicked flag
   bool mKicked;

   //! bomb kick animation
   QPointer<BombKickAnimation> mAnimation;

   //! detonation origin
   DetonationOrigin mDetonationOrigin;

   //! mapitem that may be shadowed by a kicked bomb
   QPointer<MapItem> mShadowedItem;

   //! tick time
   static int sTickTime;

   //! bomb igniter
   int8_t mIgniterId;
};

#endif  // BOMBMAPITEM_H
