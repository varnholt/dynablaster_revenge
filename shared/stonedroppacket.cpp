// header
#include "stonedroppacket.h"

// Qt

// defines
#define PACKETNAME "StoneDrop"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param x stone x position
   \param x stone y position
*/
StoneDropPacket::StoneDropPacket(int8_t x, int8_t y) : Packet(Packet::STONEDROP), mX(x), mY(y)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   read constructor
*/
StoneDropPacket::StoneDropPacket() : Packet(Packet::STONEDROP), mX(0), mY(0)
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
int8_t StoneDropPacket::getX() const
{
   return mX;
}

//-----------------------------------------------------------------------------
/*!
   \return stone y position
*/
int8_t StoneDropPacket::getY() const
{
   return mY;
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void StoneDropPacket::enqueue(BinaryWriter& out)
{
   out << mX;
   out << mY;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void StoneDropPacket::dequeue(BinaryReader& in)
{
   in >> mX >> mY;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void StoneDropPacket::debug()
{
   qDebug("StoneDropPacket: x: %d, y: %d", mX, mY);
}
