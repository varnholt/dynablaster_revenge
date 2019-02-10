// header
#include "leavegameresponsepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "LeaveGameResponse"

//-----------------------------------------------------------------------------
/*!
*/
LeaveGameResponsePacket::LeaveGameResponsePacket()
   : Packet(Packet::LEAVEGAMERESPONSE),
     mGameId(-1),
     mPlayerId(-1)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
*/
LeaveGameResponsePacket::LeaveGameResponsePacket(
   qint32 gameId,
   qint32 playerId
)
   : Packet(Packet::LEAVEGAMERESPONSE),
     mGameId(gameId),
     mPlayerId(playerId)
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
void LeaveGameResponsePacket::enqueue(QDataStream& out)
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
void LeaveGameResponsePacket::dequeue(QDataStream& in)
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
void LeaveGameResponsePacket::debug()
{
   // debug output login request
   qDebug(
      "LeaveGameResponsePacket:debug: no members"
   );
}


//-----------------------------------------------------------------------------
/*!
   \param id player id to set
*/
void LeaveGameResponsePacket::setPlayerId(qint32 tmpId)
{
   mPlayerId = tmpId;
}


//-----------------------------------------------------------------------------
/*!
   \return player's id
*/
qint32 LeaveGameResponsePacket::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param id game id to set
*/
void LeaveGameResponsePacket::setGameId(qint32 id)
{
   mGameId = id;
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
qint32 LeaveGameResponsePacket::getGameId() const
{
   return mGameId;
}


