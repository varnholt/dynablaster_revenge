#ifndef BOMBMAPITEM_H
#define BOMBMAPITEM_H

// base
#include "mapitem.h"

// Qt
#include <QObject>
#include <QPointer>
#include <QTimer>

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
         Left   = 1,
         Right  = 2,
         Top    = 3,
         Bottom = 4
      };

      //! constructor
      BombMapItem(
         int playerId,
         int flames,
         int id,
         int x,
         int y
      );

      //! destructor
      virtual ~BombMapItem();

      //! getter for player id
      qint8 getPlayerId() const;

      //! getter for flames
      qint8 getFlames() const;

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
      void setIgniterId(qint8 id);

      //! getter for igniter id
      qint8 getIgniterId() const;

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


   signals:

      //! bomb exploded
      void exploded(
         BombMapItem* bomb,
         bool recursive = false
      );

      //! kick animation was started or stopped
      void kickAnimation(
         Constants::Direction direction = Constants::DirectionUnknown,
         float speed = 0.0f
      );


   protected slots:

      //! active explosion
      void explodeActive();

      //! delayed explosion triggered from animation
      void explodeDelayed();


   protected:

      //! bomb's timer
      QTimer mTimer;

      //! bomb owner
      qint8 mPlayerId;

      //! bomb flames
      qint8 mFlames;

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
      qint8 mIgniterId;
};

#endif // BOMBMAPITEM_H

