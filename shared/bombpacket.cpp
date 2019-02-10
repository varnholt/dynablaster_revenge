// header
#include "bombpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Bomb"

/*!----------------------------------------------------------------------------
   write constructor

   \param pId player id
   \param xPos player x position
   \param yPos player y position
*/
BombPacket::BombPacket(
   qint8 pId,
   quint8 xPos,
   quint8 yPos
)
 : Packet(Packet::BOMB),
   playerId(pId),
   x(xPos),
   y(yPos)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   read constructor
*/
BombPacket::BombPacket()
   : Packet(Packet::BOMB),
     playerId(0),
     x(0),
     y(0)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   destructor
*/
BombPacket::~BombPacket()
{
}


/*!----------------------------------------------------------------------------
   \return bomb x position
*/
quint8 BombPacket::getX()
{
   return x;
}


/*!----------------------------------------------------------------------------
   \return player id
*/
qint8 BombPacket::getPlayerId()
{
   return playerId;
}


/*!----------------------------------------------------------------------------
   \return bomb y position
*/
quint8 BombPacket::getY()
{
   return y;
}


/*!----------------------------------------------------------------------------
   \param out datastream to write members to
*/
void BombPacket::enqueue(QDataStream & out)
{
   // write player id
   out << playerId;

   // write bombs
   out << x;
   out << y;
}


/*!----------------------------------------------------------------------------
   \param in datastream read members from
*/
void BombPacket::dequeue(QDataStream & in)
{
   // read player id, x, y
   in >> playerId >> x >> y;
}


/*!----------------------------------------------------------------------------
   debug output of members
*/
void BombPacket::debug()
{
   // output bomb's x-, y- position
   qDebug(
      "BombPacket:bomb: player: %d (%d, %d)",
      playerId,
      x,
      y
   );
}


