// header
#include "mapitemcreatedpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "MapItemCreated"

//-----------------------------------------------------------------------------
/*!
   \param item mapitem
*/
MapItemCreatedPacket::MapItemCreatedPacket(
   MapItem *item,
   qint8 creator
)
   : MapItemPacket(Packet::MAPITEMCREATED, item),
     mAppearance(item->getAppearance()),
     mPlayerId(creator)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
MapItemCreatedPacket::MapItemCreatedPacket()
   : MapItemPacket(Packet::MAPITEMCREATED),
     mAppearance(-1)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \return mapitem's appearance
*/
qint32 MapItemCreatedPacket::getAppearance() const
{
   return mAppearance;
}


//-----------------------------------------------------------------------------
/*!
   \return mapitem's creator
*/
qint8 MapItemCreatedPacket::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MapItemCreatedPacket::enqueue(QDataStream & out)
{
	MapItemPacket::enqueue(out);

   // write members
   out << mAppearance;
   out << mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MapItemCreatedPacket::dequeue(QDataStream & in)
{
   MapItemPacket::dequeue(in);

   // read members
   in >> mAppearance;
   in >> mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void MapItemCreatedPacket::debug()
{
   // output packet members
   qDebug(
      "MapItemCreatedPacket: x: %d, y: %d, type: %d, appearance: %d",
      getX(),
      getY(),
      getType(),
      mAppearance
   );
}

