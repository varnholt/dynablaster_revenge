// header
#include "mapitemdestroyedpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "MapItemDestroyed"


//-----------------------------------------------------------------------------
/*!
*/
MapItemDestroyedPacket::MapItemDestroyedPacket(
   MapItem *item,
   qint32 playerId,
   Constants::Direction direction,
   float intensity
)
   : MapItemPacket(Packet::MAPITEMDESTROYED, item),
     mPlayerId(playerId),
     mDirection(direction),
     mIntensity(intensity)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
MapItemDestroyedPacket::MapItemDestroyedPacket()
   : MapItemPacket(Packet::MAPITEMDESTROYED),
     mPlayerId(0),
     mDirection(Constants::DirectionUp),
     mIntensity(0.0f)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MapItemDestroyedPacket::enqueue(QDataStream & out)
{
   MapItemPacket::enqueue(out);

   // write members
   out << mPlayerId;
   out << mDirection;
   out << mIntensity;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MapItemDestroyedPacket::dequeue(QDataStream & in)
{
   MapItemPacket::dequeue(in);

   qint32 direction = 0;

   // read members
   in
      >> mPlayerId
      >> direction
      >> mIntensity;

   mDirection = (Constants::Direction)direction;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void MapItemDestroyedPacket::debug()
{
   // output packet members
   qDebug(
      "MapItemDestroyedPacket: x: %d, y: %d, player: %d, intensity: %f",
      getX(),
      getY(),
      mPlayerId,
      mIntensity
   );
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
qint32 MapItemDestroyedPacket::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \return destroyed direction
*/
Constants::Direction MapItemDestroyedPacket::getDirection() const
{
   return mDirection;
}


//-----------------------------------------------------------------------------
/*!
   \return intensity
*/
float MapItemDestroyedPacket::getIntensity() const
{
   return mIntensity;
}


