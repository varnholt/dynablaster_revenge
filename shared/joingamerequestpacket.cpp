//
// C++ Implementation: JoinGameRequestPacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

// header
#include "joingamerequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "JoinGameRequest"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
JoinGameRequestPacket::JoinGameRequestPacket(
   int id
)
 : Packet(Packet::JOINGAMEREQUEST),
   mId(id)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
JoinGameRequestPacket::JoinGameRequestPacket()
   : Packet(Packet::JOINGAMEREQUEST)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
JoinGameRequestPacket::~JoinGameRequestPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int JoinGameRequestPacket::getId()
{
   return mId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void JoinGameRequestPacket::enqueue(QDataStream & out)
{
   // write members
   out << mId;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void JoinGameRequestPacket::dequeue(QDataStream & in)
{
   // read members
   in >> mId;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void JoinGameRequestPacket::debug()
{
   // debug output login response
   qDebug(
      "JoinGameRequestPacket:debug: id: %d",
      mId
   );
}


