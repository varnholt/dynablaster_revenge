#ifndef POSITIONINTERPOLATION_H
#define POSITIONINTERPOLATION_H

// Qt
#include <QMap>
#include <QObject>

// shared
#include "constants.h"
#include "framework/frametimer.h"

// forward declarations
class BombermanClient;
class MapItem;
class MapItemAnimation;
class PlayerInfo;

class PositionInterpolation : public QObject
{
   Q_OBJECT

   public:

      //! constructor
      PositionInterpolation(QObject *parent = 0);


   signals:

      //! emit player position
      void setPlayerPosition(int id, float x, float y, float ang);

      //! emit map item position
      void setMapItemPosition(MapItem* item, float x, float y, float z);

      //! emit player speed
      void setPlayerSpeed(int id, float x, float y, float ang);

      //! one of the items bounces
      void bounce();


   public slots:

      //! a mapitem starts movement (bomb is kicked)
      void moveMapItem(
         MapItem* item,
         Constants::Direction dir,
         float speed,
         int nominalX,
         int nominalY
      );

      //! a mapitem is removed (bomb exploded)
      void removeMapItem(MapItem* item);


   protected slots:
      //! enabled/disable timers
      void gameStateChanged();

      //! update positions
      void update();


   protected:

      //! interpolate player positions
      void interpolatePlayerPositions(float dt);

      //! interpolate map item positions (only bombs right now)
      void interpolateMapItemPositions(float dt);


      //! last update time
      float mTime;

      //! update timer
      FrameTimer mTimer;

      //! list of animated mapitems
      QList<MapItem*> mMapItems;

      //! map mapitem <-> mapitemanimation
      QMap<MapItem*, MapItemAnimation*> mMapItemAnimations;
};

#endif // POSITIONINTERPOLATION_H

