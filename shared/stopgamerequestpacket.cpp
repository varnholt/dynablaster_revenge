// header
#include "stopgamerequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "StopGameRequest"


//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
StopGameRequestPacket::StopGameRequestPacket(
   int id
)
 : Packet(Packet::STOPGAMEREQUEST),
   mId(id)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
StopGameRequestPacket::StopGameRequestPacket()
   : Packet(Packet::STOPGAMEREQUEST)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
StopGameRequestPacket::~StopGameRequestPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int StopGameRequestPacket::getId()
{
   return mId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void StopGameRequestPacket::enqueue(QDataStream & out)
{
   // write members
   out << mId;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void StopGameRequestPacket::dequeue(QDataStream & in)
{
   // read members
   in >> mId;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void StopGameRequestPacket::debug()
{
   // debug output login response
   qDebug(
      "StopGameRequestPacket:debug: id: %d",
      mId
   );
}


