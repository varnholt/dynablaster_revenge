//
// C++ Implementation: messagepacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

// header
#include "messagepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Message"

//----------------------------------------------------------------------------
/*!
   \param message message to send
   \param receiverId id of the receiver
*/
MessagePacket::MessagePacket(
   qint8 senderId,
   const QString& message,
   bool finishedTyping,
   qint8 receiverId
)
 : Packet(Packet::MESSAGE),
   mSenderId(senderId),
   mMessage(message),
   mReceiverId(receiverId),
   mFinishedTyping(finishedTyping)
{
   mPacketName = PACKETNAME;
}


//----------------------------------------------------------------------------
/*!
*/
MessagePacket::MessagePacket()
   : Packet(Packet::MESSAGE),
     mReceiverId(-1),
     mFinishedTyping(true)
{
   mPacketName = PACKETNAME;
}


//----------------------------------------------------------------------------
/*!
*/
MessagePacket::~MessagePacket()
{
}


//----------------------------------------------------------------------------
/*!
   \return message to send
*/
QString MessagePacket::getMessage() const
{
   return mMessage;
}

//----------------------------------------------------------------------------
/*!
   \return receiver's id
*/
qint8 MessagePacket::getReceiverId() const
{
   return mReceiverId;
}


//----------------------------------------------------------------------------
/*!
   \return \c true if typing is finished
*/
bool MessagePacket::isTypingFinished() const
{
   return mFinishedTyping;
}


//----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MessagePacket::enqueue(QDataStream & out)
{
   // write sender id
   out << mSenderId;

   // write message
   out << mMessage;

   // write receiver id
   out << mReceiverId;

   // write finished typing flag
   out << mFinishedTyping;
}


//----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MessagePacket::dequeue(QDataStream & in)
{
   // read message and receiver id
   in >> mSenderId
      >> mMessage
      >> mReceiverId
      >> mFinishedTyping;
}


//----------------------------------------------------------------------------
/*!
   \return sender id
*/
qint8 MessagePacket::getSenderId() const
{
   return mSenderId;
}


//----------------------------------------------------------------------------
/*!
   debug output of members
*/
void MessagePacket::debug()
{
   // debug message request
   qDebug(
      "MessagePacket:debug: sender: %d, message: '%s', "
      "receiver: %d, finished: %d",
      mSenderId,
      qPrintable(mMessage),
      mReceiverId,
      mFinishedTyping
   );
}


