//
// C++ Implementation: CountdownPacket
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
#include "countdownpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Countdown"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
CountdownPacket::CountdownPacket(
   int countdown
)
 : Packet(Packet::COUNTDOWN),
   mTimeLeft(countdown)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
CountdownPacket::CountdownPacket()
   : Packet(Packet::COUNTDOWN)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
CountdownPacket::~CountdownPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return time left
*/
int CountdownPacket::getTimeLeft()
{
   return mTimeLeft;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void CountdownPacket::enqueue(QDataStream & out)
{
   // write members
   out << mTimeLeft;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void CountdownPacket::dequeue(QDataStream & in)
{
   // read members
   in >> mTimeLeft;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void CountdownPacket::debug()
{
   // debug output login response
   qDebug(
      "CountdownPacket:debug: time left: %d",
      mTimeLeft
   );
}


