// header
#include "loginrequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "LoginRequest"


/*!----------------------------------------------------------------------------
   write constructor

   \param nickName player's nick
   \param col player's color
*/
LoginRequestPacket::LoginRequestPacket(
   const QString& nickName,
   bool bot
)
 : Packet(Packet::LOGINREQUEST),
   mNick(nickName),
   mBot(bot)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   read constructor
*/
LoginRequestPacket::LoginRequestPacket()
   : Packet(Packet::LOGINREQUEST),
     mBot(false)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   destructor
*/
LoginRequestPacket::~LoginRequestPacket()
{
}


/*!----------------------------------------------------------------------------
   \return player's nickname
*/
const QString &LoginRequestPacket::getNick() const
{
   return mNick;
}


//-----------------------------------------------------------------------------
/*!
  \return \c true if player is a bot
*/
bool LoginRequestPacket::isBot() const
{
   return mBot;
}


/*!----------------------------------------------------------------------------
   \param out datastream to write members to
*/
void LoginRequestPacket::enqueue(QDataStream & out)
{
   // write player's name
   out << mNick;
   out << mBot;
}


/*!----------------------------------------------------------------------------
   \param in datastream read members from
*/
void LoginRequestPacket::dequeue(QDataStream & in)
{
   // read player's nick and color
   in >> mNick;
   in >> mBot;
}


/*!----------------------------------------------------------------------------
   debug output of members
*/
void LoginRequestPacket::debug()
{
   // debug output login request
   qDebug(
      "LoginRequestPacket:loginrequest: player: %s, bot: %d",
      qPrintable(mNick),
      mBot
   );
}


