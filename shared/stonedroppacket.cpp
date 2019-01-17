// header
#include "stonedroppacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "StoneDrop"


//-----------------------------------------------------------------------------
/*!
   write constructor

   \param x stone x position
   \param x stone y position
*/
StoneDropPacket::StoneDropPacket(
   qint8 x,
   qint8 y
)
 : Packet(Packet::STONEDROP),
   mX(x),
   mY(y)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
StoneDropPacket::StoneDropPacket()
   : Packet(Packet::STONEDROP),
     mX(0),
     mY(0)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
StoneDropPacket::~StoneDropPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return stone x position
*/
qint8 StoneDropPacket::getX() const
{
   return mX;
}


//-----------------------------------------------------------------------------
/*!
   \return stone y position
*/
qint8 StoneDropPacket::getY() const
{
   return mY;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void StoneDropPacket::enqueue(QDataStream & out)
{
   out << mX;
   out << mY;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void StoneDropPacket::dequeue(QDataStream & in)
{
   in
      >> mX
      >> mY;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void StoneDropPacket::debug()
{
   qDebug(
      "StoneDropPacket: x: %d, y: %d",
      mX,
      mY
   );
}


