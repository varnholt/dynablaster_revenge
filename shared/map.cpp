// header
#include "map.h"

// map items
#include "blockmapitem.h"
#include "bombmapitem.h"
#include "extramapitem.h"
#include "stonemapitem.h"

// shared
#include "mapitemcreatedpacket.h"
#include "mapitemremovedpacket.h"
#include "playerdisease.h"

// Qt
#include <QRandomGenerator>

// cmath
#include <limits.h>

#include <memory>
#include <unordered_set>

//-----------------------------------------------------------------------------
/*!
   constructor
*/
Map::Map(int w, int h)
   : mWidth(w),
     mHeight(h),
     mMap(0)
{
   // init map
   mMap = new MapItem*[mWidth*mHeight];

   // clear map
   memset(mMap, 0, mWidth * mHeight * sizeof(MapItem*));
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
Map::~Map()
{
   // a stone's extra (if any) is now owned by the stone itself (unique_ptr) and cleans
   // itself up automatically - no separate delete needed here
   for (int i = 0; i < mWidth * mHeight; i++)
   {
      delete mMap[i];
   }
}


//-----------------------------------------------------------------------------
/*!
   \return pointer to mapitem
   \param x x position
   \param y y position
*/
MapItem* Map::getItem(int x, int y) const
{
   return mMap[y * mWidth + x];
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
   \param y y position
   \param mapitem
*/
void Map::setItem(int x, int y, MapItem* item)
{
   mMap[y * mWidth + x]=item;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if a hidden extra is available
*/
bool Map::isHiddenExtraAvailable() const
{
   // init
   bool available = false;
   bool block = false;
   MapItem* item = 0;
   StoneMapItem* stone = 0;

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         // skip blocks, they don't need to be investigated any further
         block = (x % 2 && y % 2);

         if (!block)
         {
            // analyze item
            item = getItem(x, y);

            if (
                  item
               && item->getType() == MapItem::Stone
            )
            {
               stone = dynamic_cast<StoneMapItem*>(item);

               if (stone)
               {
                  if (stone->getExtraMapItem())
                  {
                     available = true;
                     break;
                  }
               }
            }

         }
      }
   }

   return available;
}


//-----------------------------------------------------------------------------
/*!
*/
void Map::initialize()
{
   for (int x = 0; x < mWidth; x++)
      for (int y = 0; y < mHeight; y++)
         if (x % 2 && y % 2)
            setItem(x, y, new BlockMapItem(-1, x, y));
}


//-----------------------------------------------------------------------------
/*!
*/
void Map::initializeTestMap()
{
   QList<Point> startPositions;
   startPositions
      << Point(0,0)
      << Point(12,10)
      << Point(12,0)
      << Point(0,10)
      << Point(6,5);

   setStartPositions(startPositions);

   MapItem* item = 0;
   Point playerPosition;

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         item = getItem(x,y);

         // if there's a free position..
         if (!item)
         {
            // ..eventually place a stone
            if ((QRandomGenerator::global()->bounded(100)) > 75)
            {
               // but keep some space around the players' start positions
               for (int p = 0; p < startPositions.size(); p++)
               {
                  playerPosition = startPositions.at(p);

                  // check the same position, right, left, bottom, top
                  if (
                        (x   != playerPosition.x() && y   != playerPosition.y())
                     && (x+1 != playerPosition.x() && y   != playerPosition.y())
                     && (x-1 != playerPosition.x() && y   != playerPosition.y())
                     && (x   != playerPosition.x() && y+1 != playerPosition.y())
                     && (x   != playerPosition.x() && y-1 != playerPosition.y())
                  )
                  {
                     // finally set a stone item
                     setItem(x, y, new StoneMapItem(-1, x, y));
                  }
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \return map width
*/
int32_t Map::getWidth() const
{
   return mWidth;
}


//-----------------------------------------------------------------------------
/*!
   \return map height
*/
int32_t Map::getHeight() const
{
   return mHeight;
}


//-----------------------------------------------------------------------------
/*!
   \param max maximum player count
*/
int Map::getMaxPlayers()
{
   return mStartPositions.size();
}


//-----------------------------------------------------------------------------
/*!
   \param player start positions
*/
void Map::setStartPositions(const QList<Point>& positions)
{
   mStartPositions = positions;
}


//-----------------------------------------------------------------------------
/*!
   \return player's start position
*/
Point Map::getStartPosition(int playerNumber)
{
   return mStartPositions.at(playerNumber);
}


//-----------------------------------------------------------------------------
/*!
   \return a new map generated with the given data
*/
Map* Map::generateMap(
   int width,
   int height,
   int stoneCount,
   int extraBombCount,
   int extraFlameCount,
   int extraSpeedUpCount,
   int extraKickCount,
   int extraSkullCount,
   const QList<Point>& startPositions
)
{
   Map* map = 0;

   // check if the map is actually capable of storing the given
   // number of stones and extras
   int extraSum =
         extraBombCount
       + extraFlameCount
       + extraKickCount
       + extraSpeedUpCount
       + extraSkullCount;

   // number of fields to leave out for the players' start positions
   int freeStonesForStartPositions = 0;
   for (int p = 0; p < startPositions.size(); p++)
   {
      Point playerPosition = startPositions.at(p);

      // center
      freeStonesForStartPositions++;

      // up
      if (
            playerPosition.x() >= 0
         && playerPosition.y() -1 >= 0
         && playerPosition.x() < width
         && playerPosition.y()- 1 < height
      )
      {
         freeStonesForStartPositions++;
      }

      // down
      if (
            playerPosition.x() >= 0
         && playerPosition.y() +1 >= 0
         && playerPosition.x() < width
         && playerPosition.y()+ 1 < height
      )
      {
         freeStonesForStartPositions++;
      }

      // left
      if (
            playerPosition.x() - 1 >= 0
         && playerPosition.y() >= 0
         && playerPosition.x() - 1 < width
         && playerPosition.y() < height
      )
      {
         freeStonesForStartPositions++;
      }

      // right
      if (
            playerPosition.x() + 1 >= 0
         && playerPosition.y() >= 0
         && playerPosition.x() + 1 < width
         && playerPosition.y() < height
      )
      {
         freeStonesForStartPositions++;
      }
   }

   // the allowed number of stones equals the  number of
   // fields without the blocks and the fields to leave out
   // for the players' start positions
   int allowedStoneCount =
        width * height
      - (width - 1)/2 + (height -1)/2
      - freeStonesForStartPositions;

   if (
         (stoneCount <= allowedStoneCount)
      && (extraSum <= allowedStoneCount)
      && (extraSum <= stoneCount)
   )
   {

      // create a new map
      map = new Map(width, height);

      // initialize that map with blocking items
      map->initialize();

      // init the player start positions
      map->setStartPositions(startPositions);

      MapItem* item = 0;
      Point playerPosition;

      int stonesPlaced = 0;
      int extraBombPlaced = 0;
      int extraFlamePlaced = 0;
      int extraKickPlaced = 0;
      int extraSpeedUpPlaced = 0;
      int extraSkullPlaced = 0;

      int randX = 0;
      int randY = 0;
      bool blocksStartPosition = false;

      // initiate blocked positions for those players that begin between 2 blocks
      std::unordered_set<Point> blockedPositions;
      foreach (const Point& p, startPositions)
      {
         // uneven y positions are located between 2 fixed blocks
         if (p.y() % 2 == 1)
         {
            /*
               +---+---+---+
               | 0 |   | 1 |
               +---+---+---+
               |XXX|   |XXX|
               +---+---+---+
               | 2 |   | 3 |
               +---+---+---+
            */

            Point topLeft(    p.x() - 1, p.y() - 1);
            Point topRight(   p.x() + 1, p.y() - 1);
            Point bottomLeft( p.x() - 1, p.y() + 1);
            Point bottomRight(p.x() + 1, p.y() + 1);

            QList<Point> openPositions;
            openPositions << topLeft;
            openPositions << topRight;
            openPositions << bottomLeft;
            openPositions << bottomRight;

            // i want 2 ways definitely open at maximum
            for (int i = 0; i < 2; i++)
            {
               int randIndex = QRandomGenerator::global()->bounded(4);

               blockedPositions.insert(openPositions.at(randIndex));
            }
         }
      }

      while (stonesPlaced < stoneCount)
      {
         randX = QRandomGenerator::global()->bounded(width);
         randY = QRandomGenerator::global()->bounded(height);

         item = map->getItem(randX, randY);

         // if there's a free position..
         if (!item)
         {
            // but keep some space around the players' start positions
            blocksStartPosition = false;

            for (int p = 0; p < startPositions.size(); p++)
            {
               playerPosition = startPositions.at(p);

               // check the same position, right, left, bottom, top
               if (
                     (randX   == playerPosition.x() && randY   == playerPosition.y())

                  || (randX+1 == playerPosition.x() && randY   == playerPosition.y())
                  || (randX-1 == playerPosition.x() && randY   == playerPosition.y())
                  || (randX   == playerPosition.x() && randY+1 == playerPosition.y())
                  || (randX   == playerPosition.x() && randY-1 == playerPosition.y())

                  /*
                  || (randX+1 == playerPosition.x() && randY-1 == playerPosition.y())
                  || (randX-1 == playerPosition.x() && randY-1 == playerPosition.y())
                  || (randX+1 == playerPosition.x() && randY+1 == playerPosition.y())
                  || (randX-1 == playerPosition.x() && randY+1 == playerPosition.y())
                  */

                  || blockedPositions.contains(Point(randX, randY))
               )
               {
                  blocksStartPosition = true;
               }
            }

            if (!blocksStartPosition)
            {
               // finally set a stone item
               map->setItem(
                  randX,
                  randY,
                  new StoneMapItem(
                     -1,
                     randX,
                     randY
                  )
               );

               stonesPlaced++;
            }
         }
      }

      // place extras
      while (
            (extraBombPlaced    < extraBombCount)
         || (extraFlamePlaced   < extraFlameCount)
         || (extraSpeedUpPlaced < extraSpeedUpCount)
         || (extraKickPlaced    < extraKickCount)
         || (extraSkullPlaced   < extraSkullCount)
      )
      {
         randX = QRandomGenerator::global()->bounded(width);
         randY = QRandomGenerator::global()->bounded(height);

         item = map->getItem(randX, randY);

         // if there's a stone that does not contain an extra yet
         if (
               item
            && item->getType() == MapItem::Stone
            && !((StoneMapItem*)item)->getExtraMapItem()
         )
         {
            // create bomb extras
            if (extraBombPlaced < extraBombCount)
            {
               ((StoneMapItem*)item)->setExtraMapItem(
                  std::make_unique<ExtraMapItem>(
                     -1,
                     Constants::ExtraBomb,
                     randX,
                     randY
                  )
               );

               extraBombPlaced++;
            }

            // create flame extras
            else if (extraFlamePlaced < extraFlameCount)
            {
               ((StoneMapItem*)item)->setExtraMapItem(
                  std::make_unique<ExtraMapItem>(
                     -1,
                     Constants::ExtraFlame,
                     randX,
                     randY
                  )
               );

               extraFlamePlaced++;
            }

            // create speedup extras
            else if (extraSpeedUpPlaced < extraSpeedUpCount)
            {
               ((StoneMapItem*)item)->setExtraMapItem(
                  std::make_unique<ExtraMapItem>(
                     -1,
                     Constants::ExtraSpeedup,
                     randX,
                     randY
                  )
               );

               extraSpeedUpPlaced++;
            }

            // create kick extras
            else if (extraKickPlaced < extraKickCount)
            {
               ((StoneMapItem*)item)->setExtraMapItem(
                  std::make_unique<ExtraMapItem>(
                     -1,
                     Constants::ExtraKick,
                     randX,
                     randY
                  )
               );

               extraKickPlaced++;
            }

            // create skull extras
            else if (extraSkullPlaced < extraSkullCount)
            {
               auto extra = std::make_unique<ExtraMapItem>(
                  -1,
                  Constants::ExtraSkull,
                  randX,
                  randY
               );

               // generate skull faces and init start time
               extra->setSkullFaces(PlayerDisease::generateSkullFaces());

               ((StoneMapItem*)item)->setExtraMapItem(std::move(extra));

               extraSkullPlaced++;
            }
         }
      }
   }
   else
   {
      qWarning("Map::generateMap: given map seems to be stoned.");
   }

   return map;
}


//-----------------------------------------------------------------------------
/*!
   \param socket socket to send map to
*/
QList<MapItemCreatedPacket*> Map::getMapItemCreatedPackets()
{
   QList<MapItemCreatedPacket*> packets;

   MapItem* item = 0;

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         item = getItem(x, y);

         if (item)
         {
            MapItemCreatedPacket* packet =
               new MapItemCreatedPacket(item);

            packets.append(packet);
         }
      }
   }

   return packets;
}


//-----------------------------------------------------------------------------
/*!
   \param socket socket to send map to
*/
QList<MapItemRemovedPacket*> Map::getMapItemRemovedPackets()
{
   QList<MapItemRemovedPacket*> packets;

   MapItem* item = 0;

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         item = getItem(x, y);

         if (item)
         {
            packets.append(new MapItemRemovedPacket(item));

            // map items may contain shadowed items
            if (item->getType() == MapItem::Bomb)
            {
               BombMapItem* bomb = (BombMapItem*)item;

               if (bomb->getShadowedItem())
                  packets.append(new MapItemRemovedPacket(bomb->getShadowedItem()));
            }
         }
      }
   }

   return packets;
}


//-----------------------------------------------------------------------------
/*!
*/
void Map::stopBombs()
{
   MapItem* item = 0;

   for (int x = 0; x < mWidth; x++)
   {
      for (int y = 0; y < mHeight; y++)
      {
         item = getItem(x, y);

         if (item)
         {
            if (item->getType() == MapItem::Bomb)
            {
               ((BombMapItem*)item)->stopTimer();
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param x1 point a x
   \param y1 point a y
   \param x2 point b x
   \param y2 point b y
   \return manhattan length between point a and b
*/
int Map::getManhattanLength(int x1, int y1, int x2, int y2)
{
   return qAbs(qAbs(x1) - qAbs(x2)) + qAbs(qAbs(y1) - qAbs(y2));
}


//-----------------------------------------------------------------------------
/*!
   \param pos start pos
   \param points points to check
   \param manhattanLength maximum manhattan length
   \return filtered list of points
*/
QList<Point> Map::getManhattanFiltered(
   const Point &pos,
   const QList<Point>& points,
   int manhattanLengthMax,
   int manhattanLengthMin
)
{
   QList<Point> filtered;

   Point diff;
   foreach (const Point& p, points)
   {
      diff = p - pos;

      if (
            diff.manhattanLength() <= manhattanLengthMax
         && diff.manhattanLength() >= manhattanLengthMin
      )
      {
         filtered << p;
      }
   }

   return filtered;
}


