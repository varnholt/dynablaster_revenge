#ifndef MAP_H
#define MAP_H

// Qt
#include <QList>
#include <QPoint>

// clib
#include <limits.h>

// forward declarations
class MapItem;
class MapItemCreatedPacket;
class MapItemRemovedPacket;


class Map
{

   public:

      //! constructor
      Map(
         int width,
         int height
      );

      //! destructor
      virtual ~Map();

      //! init map
      void initialize();

      //! init test map
      void initializeTestMap();

      //! get item at x, y
      virtual MapItem* getItem(
         int x,
         int y
      ) const;

      //! set item at x, y
      virtual void setItem(
         int x,
         int y,
         MapItem*
      );

      //! check if an extra is hidden in some stone
      bool isHiddenExtraAvailable() const;

      //! getter for the field's width
      qint32 getWidth() const;

      //! getter for the field's height
      qint32 getHeight() const;

      //! getter for the maximum player count
      int getMaxPlayers();

      //! setter for the player start positions
      void setStartPositions(const QList<QPoint>& positions);

      //! get the start position for the given player id
      QPoint getStartPosition(int playerNumber);

      //! generate a whole map
      static Map* generateMap(
         int width,
         int height,
         int stoneCount,
         int extraBombCount,
         int extraFlameCount,
         int extraSpeedUpCount,
         int extraKickCount,
         int extraSkullCount,
         const QList<QPoint>& startPositions
      );

      //! get a list of mapitem-created-packets for the current map
      QList<MapItemCreatedPacket*> getMapItemCreatedPackets();

      //! get a list of mapitem-removed-packets for the current map
      QList<MapItemRemovedPacket*> getMapItemRemovedPackets();

      //! stop all ticking bombs on the map
      void stopBombs();

      //! calculate manhattan length between 2 points
      static int getManhattanLength(int x1, int y1, int x2, int y2);

      //! filter list of points by given maximum manhattan length
      static QList<QPoint> getManhattanFiltered(
         const QPoint& pos,
         const QList<QPoint>&,
         int manhattanLengthMax,
         int manhattanLengthMin = INT_MIN
      );


   protected:

      //! width of the map
      qint32 mWidth;

      //! height of the map
      qint32 mHeight;

      //! the map itself
      MapItem** mMap;

      //! start positions
      QList<QPoint> mStartPositions;
};

#endif

