// header
#include "leavegamerequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "LeaveGameRequest"

//-----------------------------------------------------------------------------
/*!
*/
LeaveGameRequestPacket::LeaveGameRequestPacket()
   : Packet(Packet::LEAVEGAMEREQUEST),
     mGameId(-1),
     mPlayerId(-1)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
*/
LeaveGameRequestPacket::LeaveGameRequestPacket(
   qint32 gameId,
   qint32 playerId
)
   : Packet(Packet::LEAVEGAMEREQUEST),
     mGameId(gameId),
     mPlayerId(playerId)
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
void LeaveGameRequestPacket::enqueue(QDataStream& out)
{
   // write player data
   out
      << mGameId
      << mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void LeaveGameRequestPacket::dequeue(QDataStream& in)
{
   // read player data
   in
      >> mGameId
      >> mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void LeaveGameRequestPacket::debug()
{
   // debug output login request
   qDebug(
      "LeaveGameRequestPacket:debug: no members"
   );
}


//-----------------------------------------------------------------------------
/*!
   \param id player id to set
*/
void LeaveGameRequestPacket::setPlayerId(qint32 tmpId)
{
   mPlayerId = tmpId;
}


//-----------------------------------------------------------------------------
/*!
   \return player's id
*/
qint32 LeaveGameRequestPacket::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param id game id to set
*/
void LeaveGameRequestPacket::setGameId(qint32 id)
{
   mGameId = id;
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
qint32 LeaveGameRequestPacket::getGameId() const
{
   return mGameId;
}


