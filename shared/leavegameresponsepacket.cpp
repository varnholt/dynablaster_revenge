// header
#include "leavegameresponsepacket.h"

// Qt

// defines
#define PACKETNAME "LeaveGameResponse"

//-----------------------------------------------------------------------------
/*!
 */
LeaveGameResponsePacket::LeaveGameResponsePacket() : Packet(Packet::LEAVEGAMERESPONSE), mGameId(-1), mPlayerId(-1)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
 */
LeaveGameResponsePacket::LeaveGameResponsePacket(int32_t gameId, int32_t playerId)
    : Packet(Packet::LEAVEGAMERESPONSE), mGameId(gameId), mPlayerId(playerId)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   destructor
*/
LeaveGameResponsePacket::~LeaveGameResponsePacket()
{
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void LeaveGameResponsePacket::enqueue(BinaryWriter& out)
{
   // write player data
   out << mGameId << mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void LeaveGameResponsePacket::dequeue(BinaryReader& in)
{
   // read player data
   in >> mGameId >> mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void LeaveGameResponsePacket::debug()
{
   // debug output login request
   qDebug("LeaveGameResponsePacket:debug: no members");
}

//-----------------------------------------------------------------------------
/*!
   \param id player id to set
*/
void LeaveGameResponsePacket::setPlayerId(int32_t tmpId)
{
   mPlayerId = tmpId;
}

//-----------------------------------------------------------------------------
/*!
   \return player's id
*/
int32_t LeaveGameResponsePacket::getPlayerId() const
{
   return mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   \param id game id to set
*/
void LeaveGameResponsePacket::setGameId(int32_t id)
{
   mGameId = id;
}

//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int32_t LeaveGameResponsePacket::getGameId() const
{
   return mGameId;
}
