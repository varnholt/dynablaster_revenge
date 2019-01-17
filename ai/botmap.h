#ifndef BOTMAP_H
#define BOTMAP_H

// base
#include "map.h"

// shared
#include "constants.h"

// Qt
#include <QMutex>
#include <QObject>
#include <QGenericMatrix>

// forward declarations
class BotBombMapItem;

class BotMap : public QObject, public Map
{
   Q_OBJECT

   public:

      //! constructors
      BotMap(int width, int height);

      //! destructor
      virtual ~BotMap();

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

      //! update reachable positions
      void updateReachablePositions(
         int xStart,
         int yStart,
         int iteration = 0
      );

      //! update reachable positions
      void updateReachablePositionsRandomized(
         int xStart,
         int yStart,
         int iteration = 0
      );

      //! update list of reachable extras, to be called after updateReachablePositions(...)
      void updateReachableExtras();

      //! getter for reachable positions
      const QList<QPoint> &getReachablePositions() const;

      //! getter for reachable extras
      const QList<QPoint>& getReachableExtras() const;

      //! getter for reachable neighbor positions
      QList<QPoint> getReachableNeighborPositions(int x, int y) const;

      //! getter for reachable neighbor positions in randomized form
      QList<QPoint> getReachableNeighborPositionsRandomized(int x, int y) const;

      //! number of stones neighbored to the given position
      int getStoneCountAroundPoint(int x, int y, int flameCount);

      //! number of extras within stones neighbored to the given position
      int getExtraStoneCountAroundPoint(int x, int y, int flameCount, const QList<int>& extras);

      //! returns a map with all stones that will be destroyed soon
      int* getStonesToBeBombedMap();

      //! debug output reachable positions
      void debugTraversedMatrix();

      //! debug output map items
      void debugMapItems();

      //! create map items from ascii input
      void createMapItemsfromAscii(const QString& ascii);

      //! check if given position is dangerous
      bool isPositionHazardous(int x, int y) const;

      //! check if a position is blocked by a stone, block or bomb
      bool isPositionBlocked(int x, int y) const;

      //! check a single position
      void checkPosition(
         int x,
         int y,
         bool& hazardous,
         bool& abort,
         int distance
      ) const;

      //! initialize player directions
      void initDirections();

      //! check if a bomb drop could kill
      bool isBombDropDeadly(
         int x,
         int y,
         int flames,
         const QList<QPoint>& enemies
      ) const;

      //! get bombs placed by player id
      QList<BotBombMapItem*> getBombs(int playerId = -1) const;

      //! check if player has consumed all its pots
      bool isBombAmountConsumed(int playerId, int bombCount) const;

      //! getter for directions and current list
      const QList<Constants::Direction>& getDirectionsAndCurrent();


   public slots:

      //! a map item has been created
      void createMapItem(MapItem *item);

      //! a map item has been removed
      void removeMapItem(MapItem *item);


   protected:

      //! constructors
      BotMap();

      //! get traversed flag for x, y
      bool isTraversed(
         int x,
         int y
      ) const;

      //! set traversed flag at x, y
      void setTraversed(
         int x,
         int y,
         bool
      );

      //! reset traversed map
      void resetTraversedMap();

      //! debug directions
      void debugDirections(const QList<Constants::Direction>& list);


      // members

      //! possible directions
      QList<Constants::Direction> mDirections;

      //! all directions plus current location
      QList<Constants::Direction> mDirectionsAndCurrent;

      //! randomized directions
      QList<Constants::Direction> mDirectionsRandomized;

      //! the traversed map
      bool* mTraversedPositions;

      //! list of reachable positions
      QList<QPoint> mReachablePositions;

      //! list of reachable extra positions
      QList<QPoint> mReachableExtras;

      //! serialize member access
      mutable QMutex mMutex;
};

#endif // BOTMAP_H
