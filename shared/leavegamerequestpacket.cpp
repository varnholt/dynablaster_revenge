// header
#include "leavegamerequestpacket.h"

// Qt

// defines
#define PACKETNAME "LeaveGameRequest"

//-----------------------------------------------------------------------------
/*!
 */
LeaveGameRequestPacket::LeaveGameRequestPacket() : Packet(Packet::LEAVEGAMEREQUEST), mGameId(-1), mPlayerId(-1)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
 */
LeaveGameRequestPacket::LeaveGameRequestPacket(int32_t gameId, int32_t playerId)
    : Packet(Packet::LEAVEGAMEREQUEST), mGameId(gameId), mPlayerId(playerId)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   destructor
*/
LeaveGameRequestPacket::~LeaveGameRequestPacket()
{
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void LeaveGameRequestPacket::enqueue(BinaryWriter& out)
{
   // write player data
   out << mGameId << mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void LeaveGameRequestPacket::dequeue(BinaryReader& in)
{
   // read player data
   in >> mGameId >> mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void LeaveGameRequestPacket::debug()
{
   // debug output login request
   qDebug("LeaveGameRequestPacket:debug: no members");
}

//-----------------------------------------------------------------------------
/*!
   \param id player id to set
*/
void LeaveGameRequestPacket::setPlayerId(int32_t tmpId)
{
   mPlayerId = tmpId;
}

//-----------------------------------------------------------------------------
/*!
   \return player's id
*/
int32_t LeaveGameRequestPacket::getPlayerId() const
{
   return mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   \param id game id to set
*/
void LeaveGameRequestPacket::setGameId(int32_t id)
{
   mGameId = id;
}

//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int32_t LeaveGameRequestPacket::getGameId() const
{
   return mGameId;
}
