// header
#include "extrashakepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "ExtraShake"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param uniqueId unique id
*/
ExtraShakePacket::ExtraShakePacket(int uniqueId)
 : Packet(Packet::EXTRASHAKE),
   mMapItemUniqueId(uniqueId)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
ExtraShakePacket::ExtraShakePacket()
   : Packet(Packet::EXTRASHAKE),
     mMapItemUniqueId(0)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
ExtraShakePacket::~ExtraShakePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return unique id
*/
int ExtraShakePacket::getMapItemUniqueId() const
{
   return mMapItemUniqueId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void ExtraShakePacket::enqueue(QDataStream & out)
{
   out << mMapItemUniqueId;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void ExtraShakePacket::dequeue(QDataStream & in)
{
   in >> mMapItemUniqueId;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void ExtraShakePacket::debug()
{
   qDebug(
      "ExtraShakePacket: unique id: %d",
      mMapItemUniqueId
   );
}


