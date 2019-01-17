// header
#include "mapitem.h"

// shared
#include "mapitemcreatedpacket.h"


//! mapitem id
int MapItem::mCurrentId = 0;


//-----------------------------------------------------------------------------
/*!
   \param type mapitem type
   \param appearance mapitem appearance
   \param blocking blocking flag
   \param destroyable destroyable flag
   \param x x position
   \param y y position
*/
MapItem::MapItem(
   ItemType type,
   int appearance,
   bool blocking,
   bool destroyable,
   int x,
   int y
)
 : QObject(),
   mType(type),
   mUniqueId(mCurrentId++),
   mAppearance(appearance),
   mBlocking(blocking),
   mDestroyable(destroyable),
   mX(x),
   mY(y),
   mCurrentlyDestroyed(false),
   mDestroyDirection(Constants::DirectionUnknown)
{
   initializeBlocking();
}


//-----------------------------------------------------------------------------
/*!
   \param packet packet to construct mapitem from
*/
MapItem::MapItem(MapItemCreatedPacket *pack)
 : QObject(),
   mType(pack->getItemType()),
   mUniqueId(pack->getUniqueId()),
   mAppearance(pack->getAppearance()),
   mBlocking(false),
   mDestroyable(false),
   mX(pack->getX()),
   mY(pack->getY()),
   mCurrentlyDestroyed(false),
   mDestroyDirection(Constants::DirectionUnknown)
{
   initializeBlocking();
}


//-----------------------------------------------------------------------------
/*!
*/
MapItem::~MapItem()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void MapItem::initializeBlocking()
{
   switch (mType)
   {
      case Block:
         mBlocking = true;
         break;

      case Bomb:
         mBlocking = true;
         break;

      case Stone:
         mBlocking = true;
         mDestroyable = true;
         break;

      // extras are non-blocking
      default:
         break;
   }
}


//-----------------------------------------------------------------------------
/*!
   \return item type
*/
MapItem::ItemType MapItem::getType() const
{
   return mType;
}


//-----------------------------------------------------------------------------
/*!
   \param uniqueId item's unique id
*/
void MapItem::setUniqueId(int uniqueId)
{
   mUniqueId = uniqueId;
}


//-----------------------------------------------------------------------------
/*!
   \return item appearance
*/
int MapItem::getAppearance() const
{
   return mAppearance;
}


//-----------------------------------------------------------------------------
/*!
   \return true if item is blocking
*/
bool MapItem::isBlocking() const
{
   return mBlocking;
}


//-----------------------------------------------------------------------------
/*!
   \return true if item is destroyable
*/
bool MapItem::isDestroyable() const
{
   return mDestroyable;
}


//-----------------------------------------------------------------------------
/*!
   \param x x position
*/
void MapItem::setX(int x)
{
   mX = x;
}


//-----------------------------------------------------------------------------
/*!
   \param y y position
*/
void MapItem::setY(int y)
{
   mY = y;
}


//-----------------------------------------------------------------------------
/*!
   \return x position
*/
int MapItem::getX() const
{
   return mX;
}


//-----------------------------------------------------------------------------
/*!
   \return y position
*/
int MapItem::getY() const
{
   return mY;
}


//-----------------------------------------------------------------------------
/*!
   \return unique mapitem id
*/
int MapItem::getUniqueId() const
{
   return mUniqueId;
}


//-----------------------------------------------------------------------------
/*!
  \param destroyed true if item is currently being destroyed
*/
void MapItem::setCurrentlyDestroyed(bool destroyed)
{
   mCurrentlyDestroyed = destroyed;
}


//-----------------------------------------------------------------------------
/*!
   \return true if mapitem is currently being destroyed
*/
bool MapItem::isCurrentlyDestroyed() const
{
   return mCurrentlyDestroyed;
}


//-----------------------------------------------------------------------------
/*!
   \return direction from which the item was destroyed
*/
Constants::Direction MapItem::getDestroyDirection() const
{
   return mDestroyDirection;
}


//-----------------------------------------------------------------------------
/*!
   \param direction direction from which the item was destroyed
*/
void MapItem::setDestroyDirection(Constants::Direction direction)
{
   mDestroyDirection= direction;
}

