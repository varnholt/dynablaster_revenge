// header
#include "listgamesresponsepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "ListGameResponse"


//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
ListGamesResponsePacket::ListGamesResponsePacket(
   const QList<GameInformation>& games,
   bool update
)
 : Packet(Packet::LISTGAMESRESPONSE),
   mGames(games),
   mUpdate(update)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
ListGamesResponsePacket::ListGamesResponsePacket()
   : Packet(Packet::LISTGAMESRESPONSE),
     mUpdate(false)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
ListGamesResponsePacket::~ListGamesResponsePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game's list
*/
const QList<GameInformation> ListGamesResponsePacket::getGames()
{
   return mGames;
}


//-----------------------------------------------------------------------------
/*!
   \param update update flag
*/
void ListGamesResponsePacket::setUpdate(bool update)
{
   mUpdate = update;
}


//-----------------------------------------------------------------------------
/*!
   \return update flag
*/
bool ListGamesResponsePacket::isUpdate() const
{
   return mUpdate;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void ListGamesResponsePacket::enqueue(QDataStream & out)
{
   // write members
   out << mUpdate;
   out << mGames;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void ListGamesResponsePacket::dequeue(QDataStream & in)
{
   // read members
   in >> mUpdate;
   in >> mGames;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void ListGamesResponsePacket::debug()
{
   // debug output login response
   qDebug(
      "ListGamesResponsePacket:debug: number of games sent: %d, update: %d",
      mGames.size(),
      isUpdate()
   );
}


