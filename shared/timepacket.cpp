// header
#include "timepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Time"

//-----------------------------------------------------------------------------
/*!
*/
TimePacket::TimePacket(
   int left
)
   : Packet(Packet::TIME),
     mTimeLeft(left)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
TimePacket::TimePacket()
   : Packet(Packet::TIME)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \return server time
*/
int TimePacket::getTimeLeft() const
{
   return mTimeLeft;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void TimePacket::enqueue(QDataStream & out)
{
   // write members
   out << mTimeLeft;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void TimePacket::dequeue(QDataStream & in)
{
   // read members
   in >> mTimeLeft;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void TimePacket::debug()
{
   // output packet members
   qDebug(
      "TimePacket: game time left: %d",
      mTimeLeft
   );
}

