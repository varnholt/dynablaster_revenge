// header
#include "playermodifiedpacket.h"

// Qt

// defines
#define PACKETNAME "PlayerModified"

//-----------------------------------------------------------------------------
/*!
 */
PlayerModifiedPacket::PlayerModifiedPacket(Constants::Color color) : Packet(Packet::PLAYERMODIFIED), mColor(color)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   read constructor
*/
PlayerModifiedPacket::PlayerModifiedPacket() : Packet(Packet::PLAYERMODIFIED)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   \return player color
*/
Constants::Color PlayerModifiedPacket::getColor() const
{
   return (Constants::Color)mColor;
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void PlayerModifiedPacket::enqueue(BinaryWriter& out)
{
   // write members
   out << mColor;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void PlayerModifiedPacket::dequeue(BinaryReader& in)
{
   // read members
   in >> mColor;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void PlayerModifiedPacket::debug()
{
   // output packet members
   qDebug("PlayerModifiedPacket: player color: %d", mColor);
}
