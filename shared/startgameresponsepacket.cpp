// header
#include "startgameresponsepacket.h"

// Qt

// defines
#define PACKETNAME "StartGameResponse"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
StartGameResponsePacket::StartGameResponsePacket(int id, bool started) : Packet(Packet::STARTGAMERESPONSE), mId(id), mStarted(started)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   read constructor
*/
StartGameResponsePacket::StartGameResponsePacket() : Packet(Packet::STARTGAMERESPONSE), mId(-1), mStarted(false)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   destructor
*/
StartGameResponsePacket::~StartGameResponsePacket()
{
}

//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int StartGameResponsePacket::getId()
{
   return mId;
}

//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
bool StartGameResponsePacket::isStarted()
{
   return mStarted;
}

//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void StartGameResponsePacket::enqueue(BinaryWriter& out)
{
   // write members
   out << mId << mStarted;
}

//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void StartGameResponsePacket::dequeue(BinaryReader& in)
{
   // read members
   in >> mId >> mStarted;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void StartGameResponsePacket::debug()
{
   // debug output login response
   qDebug("StartGameResponsePacket:debug: id: %d, started: %d", mId, mStarted);
}
