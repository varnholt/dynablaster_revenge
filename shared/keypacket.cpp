// header
#include "keypacket.h"

// qt

// defines
#define PACKETNAME "Key"

/*!----------------------------------------------------------------------------
   read constructor
*/
KeyPacket::KeyPacket() : Packet(Packet::KEY), playerId(0), keys(0)
{
   mPacketName = PACKETNAME;
}

/*!----------------------------------------------------------------------------
   write constructor

   \param playerId player's id
   \param key keys pressed
*/
KeyPacket::KeyPacket(int8_t id, int8_t k) : Packet(Packet::KEY), playerId(id), keys(k)
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
void KeyPacket::enqueue(BinaryWriter& out)
{
   // write player id
   out << playerId;

   // write keyboard input
   out << keys;
}

/*!----------------------------------------------------------------------------
   \param in input datastream
*/
void KeyPacket::dequeue(BinaryReader& in)
{
   // read player id, key
   in >> playerId >> keys;
}

/*!----------------------------------------------------------------------------
   \return pressed keys
*/
int8_t KeyPacket::getKeys()
{
   return keys;
}

/*!----------------------------------------------------------------------------
   \return player id
*/
int8_t KeyPacket::getPlayerId()
{
   return playerId;
}

/*!----------------------------------------------------------------------------
   debug members
*/
void KeyPacket::debug()
{
   // output player id and key
   qDebug("KeyPacket: player id: %d, keys: %d", playerId, keys);
}
