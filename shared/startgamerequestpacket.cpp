// header
#include "startgamerequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "StartGameRequest"


//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
StartGameRequestPacket::StartGameRequestPacket(
   int id
)
 : Packet(Packet::STARTGAMEREQUEST),
   mId(id)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
StartGameRequestPacket::StartGameRequestPacket()
   : Packet(Packet::STARTGAMEREQUEST)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
StartGameRequestPacket::~StartGameRequestPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int StartGameRequestPacket::getId()
{
   return mId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void StartGameRequestPacket::enqueue(QDataStream & out)
{
   // write members
   out << mId;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void StartGameRequestPacket::dequeue(QDataStream & in)
{
   // read members
   in >> mId;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void StartGameRequestPacket::debug()
{
   // debug output login response
   qDebug(
      "StartGameRequestPacket:debug: id: %d",
      mId
   );
}


