// header
#include "mapitemremovedpacket.h"

// Qt

// defines
#define PACKETNAME "MapItemRemoved"

//-----------------------------------------------------------------------------
/*!
 */
MapItemRemovedPacket::MapItemRemovedPacket(MapItem* item) : MapItemPacket(Packet::MAPITEMREMOVED, item)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   read constructor
*/
MapItemRemovedPacket::MapItemRemovedPacket() : MapItemPacket(Packet::MAPITEMREMOVED)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MapItemRemovedPacket::enqueue(BinaryWriter& out)
{
   MapItemPacket::enqueue(out);
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MapItemRemovedPacket::dequeue(BinaryReader& in)
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
   qDebug("MapItemRemovedPacket: x: %d, y: %d", getX(), getY());
}
