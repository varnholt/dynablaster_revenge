// header
#include "stopgameresponsepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "StopGameResponse"


//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
StopGameResponsePacket::StopGameResponsePacket(
   int id,
   bool finished
)
 : Packet(Packet::STOPGAMERESPONSE),
   mId(id),
   mFinished(finished)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
StopGameResponsePacket::StopGameResponsePacket()
   : Packet(Packet::STOPGAMERESPONSE),
     mId(-1),
     mFinished(false)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
StopGameResponsePacket::~StopGameResponsePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game's id
*/
int StopGameResponsePacket::getId()
{
   return mId;
}


//-----------------------------------------------------------------------------
/*!
   \return stopped flag
*/
bool StopGameResponsePacket::isFinished()
{
   return mFinished;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void StopGameResponsePacket::enqueue(QDataStream & out)
{
   // write members
   out << mId << mFinished;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void StopGameResponsePacket::dequeue(QDataStream & in)
{
   // read members
   in >> mId >> mFinished;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void StopGameResponsePacket::debug()
{
   // debug output login response
   qDebug(
      "StopGameResponsePacket:debug: id: %d, all rounds finished: %d",
      mId,
      mFinished
   );
}


