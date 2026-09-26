// header
#include "loginresponsepacket.h"

// Qt

// defines
#define PACKETNAME "LoginResponse"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param nickName player's nick
   \param col player's color
*/
LoginResponsePacket::LoginResponsePacket(bool broadcast, int32_t id, const std::string& nick, const ServerConfiguration& serverConfig)
    : Packet(Packet::LOGINRESPONSE), mBroadcast(broadcast), mId(id), mNick(nick), mServerConfiguration(serverConfig)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   read constructor
*/
LoginResponsePacket::LoginResponsePacket() : Packet(Packet::LOGINRESPONSE), mBroadcast(false), mId(-1)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   destructor
*/
LoginResponsePacket::~LoginResponsePacket()
{
}

//-----------------------------------------------------------------------------
/*!
   \param id id to set
*/
void LoginResponsePacket::setId(int32_t tmpId)
{
   mId = tmpId;
}

//-----------------------------------------------------------------------------
/*!
   \return player's id
*/
int32_t LoginResponsePacket::getId() const
{
   return mId;
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void LoginResponsePacket::enqueue(BinaryWriter& out)
{
   // write player data
   out << mBroadcast << mId << mNick << mServerConfiguration;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void LoginResponsePacket::dequeue(BinaryReader& in)
{
   // read player data
   in >> mBroadcast >> mId >> mNick >> mServerConfiguration;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void LoginResponsePacket::debug()
{
   // debug output login response
   qDebug("LoginResponsePacket:loginresponse: player: %d (%s)", mId, (mId != -1) ? "accepted" : "denied");
}

//-----------------------------------------------------------------------------
/*!
   \return player's nickname
*/
const std::string& LoginResponsePacket::getNick() const
{
   return mNick;
}

//-----------------------------------------------------------------------------
/*!
   \return broadcast flag
*/
bool LoginResponsePacket::isBroadcast() const
{
   return mBroadcast;
}

//-----------------------------------------------------------------------------
/*!
   \return reference to server configuration
*/
const ServerConfiguration& LoginResponsePacket::getServerConfiguration() const
{
   return mServerConfiguration;
}
