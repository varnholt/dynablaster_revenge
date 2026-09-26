// header
#include "listgamesrequestpacket.h"

// Qt

// defines
#define PACKETNAME "ListGameRequest"

//-----------------------------------------------------------------------------
/*!
 */
ListGamesRequestPacket::ListGamesRequestPacket() : Packet(Packet::LISTGAMESREQUEST)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   destructor
*/
ListGamesRequestPacket::~ListGamesRequestPacket()
{
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void ListGamesRequestPacket::enqueue(BinaryWriter& /*out*/)
{
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void ListGamesRequestPacket::dequeue(BinaryReader& /*in*/)
{
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void ListGamesRequestPacket::debug()
{
   // debug output login request
   qDebug("ListGamesRequestPacket:debug: no members");
}
