// header
#include "playerkilledpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "PlayerKilled"


//-----------------------------------------------------------------------------
/*!
*/
PlayerKilledPacket::PlayerKilledPacket(
   qint32 playerId,
   qint32 playerKilledById,
   Constants::Direction direction,
   float intensity
)
   : Packet(Packet::PLAYERKILLED),
     mPlayerId(playerId),
     mPlayerKilledById(playerKilledById),
     mDirection(direction),
     mIntensity(intensity)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
PlayerKilledPacket::PlayerKilledPacket()
   : Packet(Packet::PLAYERKILLED),
     mPlayerId(0),
     mPlayerKilledById(0),
     mDirection(Constants::DirectionUp),
     mIntensity(0.0f)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
qint32 PlayerKilledPacket::getPlayerId() const
{
	return mPlayerId;
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void PlayerKilledPacket::enqueue(QDataStream & out)
{
   // write members
   out << mPlayerId;
   out << mPlayerKilledById;
   out << mDirection;
   out << mIntensity;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void PlayerKilledPacket::dequeue(QDataStream & in)
{
   qint32 direction = 0;

   // read members
   in
      >> mPlayerId
      >> mPlayerKilledById
      >> direction
      >> mIntensity;

   mDirection = (Constants::Direction)direction;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void PlayerKilledPacket::debug()
{
   // output packet members
   qDebug(
      "PlayerKilledPacket: player: %d, killed by: %d, intensity: %f",
      mPlayerId,
      mPlayerKilledById,
      mIntensity
   );
}

