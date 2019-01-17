// header
#include "playerinfectedpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "PlayerInfected"


//-----------------------------------------------------------------------------
/*!
*/
PlayerInfectedPacket::PlayerInfectedPacket(
   qint32 playerId,
   Constants::SkullType type
)
   : Packet(Packet::PLAYERINFECTEDPACKET),
     mPlayerId(playerId),
     mSkullType(type),
     mInfectorId(-1),
     mExtraPosX(0),
     mExtraPosY(0)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
PlayerInfectedPacket::PlayerInfectedPacket()
   : Packet(Packet::PLAYERINFECTEDPACKET),
     mPlayerId(0),
     mSkullType(Constants::SkullAutofire),
     mInfectorId(-1),
     mExtraPosX(0),
     mExtraPosY(0)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
qint32 PlayerInfectedPacket::getPlayerId() const
{
    return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \return skull type
*/
Constants::SkullType PlayerInfectedPacket::getSkullType() const
{
   return mSkullType;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void PlayerInfectedPacket::enqueue(QDataStream & out)
{
   // write members
   out << mPlayerId;
   out << mSkullType;
   out << mInfectorId;
   out << mExtraPosX;
   out << mExtraPosY;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void PlayerInfectedPacket::dequeue(QDataStream & in)
{
   qint32 skull = 0;

   // read members
   in
      >> mPlayerId
      >> skull
      >> mInfectorId
      >> mExtraPosX
      >> mExtraPosY;

   mSkullType = (Constants::SkullType)skull;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void PlayerInfectedPacket::debug()
{
   // output packet members
   qDebug(
      "PlayerInfectedPacket: player: %d, skull type: %d",
      mPlayerId,
      mSkullType
   );
}



//-----------------------------------------------------------------------------
/*!
  \return y position of extra
*/
quint8 PlayerInfectedPacket::getExtraPosY() const
{
   return mExtraPosY;
}


//-----------------------------------------------------------------------------
/*!
  \return x position of extra
*/
quint8 PlayerInfectedPacket::getExtraPosX() const
{
   return mExtraPosX;
}


//-----------------------------------------------------------------------------
/*!
  \param x x position of extra
  \param y y position of extra
*/
void PlayerInfectedPacket::setExtraPos(quint8 x, quint8 y)
{
   mExtraPosX = x;
   mExtraPosY = y;
}


//-----------------------------------------------------------------------------
/*!
   \return infector id
*/
qint32 PlayerInfectedPacket::getInfectorId() const
{
   return mInfectorId;
}


//-----------------------------------------------------------------------------
/*!
   \param value infector id
*/
void PlayerInfectedPacket::setInfectorId(const qint32 &value)
{
   mInfectorId = value;
}


