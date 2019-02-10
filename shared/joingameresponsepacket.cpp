// header
#include "joingameresponsepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "JoinGameResponse"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param nickName player's nick
*/
JoinGameResponsePacket::JoinGameResponsePacket(
   bool success,
   qint32 gameId,
   qint32 playerId,
   const QString& nick,
   Constants::Color color
)
 : Packet(Packet::JOINGAMERESPONSE),
   mSuccess(success),
   mGameId(gameId),
   mPlayerId(playerId),
   mNick(nick),
   mColor(color)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
JoinGameResponsePacket::JoinGameResponsePacket()
 : Packet(Packet::JOINGAMERESPONSE),
   mSuccess(false),
   mGameId(-1),
   mPlayerId(-1),
   mColor(Constants::ColorWhite)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
JoinGameResponsePacket::~JoinGameResponsePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \param id player id to set
*/
void JoinGameResponsePacket::setPlayerId(qint32 tmpId)
{
   mPlayerId = tmpId;
}


//-----------------------------------------------------------------------------
/*!
   \return player's id
*/
qint32 JoinGameResponsePacket::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param id game id to set
*/
void JoinGameResponsePacket::setGameId(qint32 id)
{
   mGameId = id;
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
qint32 JoinGameResponsePacket::getGameId() const
{
   return mGameId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void JoinGameResponsePacket::enqueue(QDataStream & out)
{
   // write player data
   out
      << mSuccess
      << mGameId
      << mPlayerId
      << mNick
      << (qint32)mColor;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void JoinGameResponsePacket::dequeue(QDataStream & in)
{
   qint32 color = 0;

   // read player data
   in
      >> mSuccess
      >> mGameId
      >> mPlayerId
      >> mNick
      >> color;

   mColor = (Constants::Color)color;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void JoinGameResponsePacket::debug()
{
   // debug output login response
   qDebug(
      "JoinGameResponsePacket:loginresponse: game: %d, player: %d (%s)",
      mGameId,
      mPlayerId,
      (mSuccess) ? "accepted" : "denied"
   );
}



//-----------------------------------------------------------------------------
/*!
   \return player's nickname
*/
const QString& JoinGameResponsePacket::getNick() const
{
   return mNick;
}

//-----------------------------------------------------------------------------
/*!
   \return success flag
*/
bool JoinGameResponsePacket::isSuccessful() const
{
   return mSuccess;
}


//----------------------------------------------------------------------------
/*!
   \param color player color
*/
void JoinGameResponsePacket::setColor(Constants::Color color)
{
   mColor = color;
}


//----------------------------------------------------------------------------
/*!
   \return player color
*/
Constants::Color JoinGameResponsePacket::getColor() const
{
   return mColor;
}

