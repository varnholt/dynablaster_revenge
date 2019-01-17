// header
#include "mapitemremovedpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "MapItemRemoved"

//-----------------------------------------------------------------------------
/*!
*/
MapItemRemovedPacket::MapItemRemovedPacket(MapItem *item)
   : MapItemPacket(Packet::MAPITEMREMOVED, item)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
MapItemRemovedPacket::MapItemRemovedPacket()
   : MapItemPacket(Packet::MAPITEMREMOVED)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MapItemRemovedPacket::enqueue(QDataStream & out)
{
   MapItemPacket::enqueue(out);
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MapItemRemovedPacket::dequeue(QDataStream & in)
{
   MapItemPacket::dequeue(in);
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void MapItemRemovedPacket::debug()
{
   // output packet members
   qDebug(
      "MapItemRemovedPacket: x: %d, y: %d",
      getX(),
      getY()
   );
}

