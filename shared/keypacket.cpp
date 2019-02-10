// header
#include "keypacket.h"

// qt
#include <QDataStream>

// defines
#define PACKETNAME "Key"


/*!----------------------------------------------------------------------------
   read constructor
*/
KeyPacket::KeyPacket()
   : Packet(Packet::KEY),
     playerId(0),
     keys(0)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   write constructor

   \param playerId player's id
   \param key keys pressed
*/
KeyPacket::KeyPacket(
   qint8 id,
   qint8 k
)
 : Packet(Packet::KEY),
   playerId(id),
   keys(k)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   destructor
*/
KeyPacket::~KeyPacket()
{
}


/*!----------------------------------------------------------------------------
   \param out output datastream
*/
void KeyPacket::enqueue(QDataStream & out)
{
   // write player id
   out << playerId;

   // write keyboard input
   out << keys;
}


/*!----------------------------------------------------------------------------
   \param in input datastream
*/
void KeyPacket::dequeue(QDataStream & in)
{
   // read player id, key
   in >> playerId >> keys;
}


/*!----------------------------------------------------------------------------
   \return pressed keys
*/
qint8 KeyPacket::getKeys()
{
   return keys;
}


/*!----------------------------------------------------------------------------
   \return player id
*/
qint8 KeyPacket::getPlayerId()
{
   return playerId;
}


/*!----------------------------------------------------------------------------
   debug members
*/
void KeyPacket::debug()
{
   // output player id and key
   qDebug(
      "KeyPacket: player id: %d, keys: %d",
      playerId,
      keys
   );
}


