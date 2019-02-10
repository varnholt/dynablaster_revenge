// header
#include "creategameresponsepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "CreateGameResponse"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
CreateGameResponsePacket::CreateGameResponsePacket(
   const GameInformation& gameInformation
)
 : Packet(Packet::CREATEGAMERESPONSE),
   mGameInformation(gameInformation)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
CreateGameResponsePacket::CreateGameResponsePacket()
   : Packet(Packet::CREATEGAMERESPONSE)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
CreateGameResponsePacket::~CreateGameResponsePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game information object
*/
const GameInformation &CreateGameResponsePacket::getGameInformation() const
{
   return mGameInformation;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void CreateGameResponsePacket::enqueue(QDataStream & out)
{
   // write members
   out << mGameInformation;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void CreateGameResponsePacket::dequeue(QDataStream & in)
{
   // read members
   in >> mGameInformation;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void CreateGameResponsePacket::debug()
{
   // debug output login response
   qDebug(
      "CreateGameResponsePacket:debug: id: %d, player id: %d",
      mGameInformation.getId(),
      mGameInformation.getCreatorId()
   );
}


