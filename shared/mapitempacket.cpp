// header
#include "mapitempacket.h"
#include "mapitem.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "MapItem"


MapItemPacket::MapItemPacket(Packet::TYPE type)
   : Packet(type),
     mX(0),
     mY(0),
     mUniqueId(-1),
     mItemType(MapItem::Unknown)
{
   mPacketName = PACKETNAME;
}


MapItemPacket::MapItemPacket(Packet::TYPE type, MapItem *item)
   : Packet(type),
     mX(item->getX()),
     mY(item->getY()),
     mUniqueId(item->getUniqueId()),
     mItemType(item->getType())
{
   mPacketName = PACKETNAME;
}


int MapItemPacket::getX() const
{
   return mX;
}


int MapItemPacket::getY() const
{
   return mY;
}

MapItem::ItemType MapItemPacket::getItemType() const
{
   return mItemType;
}

int MapItemPacket::getUniqueId() const
{
   return mUniqueId;
}

void MapItemPacket::enqueue(QDataStream & out)
{
   // write members
   out << (int)mItemType;
   out << mX;
   out << mY;
   out << mUniqueId;
}


void MapItemPacket::dequeue(QDataStream & in)
{
   // read members
   qint32 itemType = 0;
   in >> itemType;
   mItemType = (MapItem::ItemType)itemType;

   in
      >> mX
      >> mY
      >> mUniqueId;
}


void MapItemPacket::debug()
{
   qDebug( "MapItemPacketPacket: x: %d, y: %d", mX, mY );
}


