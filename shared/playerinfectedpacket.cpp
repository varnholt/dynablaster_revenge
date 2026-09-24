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
   int32_t playerId,
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
int32_t PlayerInfectedPacket::getPlayerId() const
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
   int32_t skull = 0;

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
uint8_t PlayerInfectedPacket::getExtraPosY() const
{
   return mExtraPosY;
}


//-----------------------------------------------------------------------------
/*!
  \return x position of extra
*/
uint8_t PlayerInfectedPacket::getExtraPosX() const
{
   return mExtraPosX;
}


//-----------------------------------------------------------------------------
/*!
  \param x x position of extra
  \param y y position of extra
*/
void PlayerInfectedPacket::setExtraPos(uint8_t x, uint8_t y)
{
   mExtraPosX = x;
   mExtraPosY = y;
}


//-----------------------------------------------------------------------------
/*!
   \return infector id
*/
int32_t PlayerInfectedPacket::getInfectorId() const
{
   return mInfectorId;
}


//-----------------------------------------------------------------------------
/*!
   \param value infector id
*/
void PlayerInfectedPacket::setInfectorId(const int32_t &value)
{
   mInfectorId = value;
}


