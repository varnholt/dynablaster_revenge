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
   int8_t senderId,
   const QString& message,
   bool finishedTyping,
   int8_t receiverId
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
int8_t MessagePacket::getReceiverId() const
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
int8_t MessagePacket::getSenderId() const
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


