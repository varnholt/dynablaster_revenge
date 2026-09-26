#ifndef MESSAGEPACKET_H
#define MESSAGEPACKET_H

#include "packet.h"

class MessagePacket : public Packet
{
public:
   //! write constructor
   MessagePacket(int8_t senderId, const QString& message, bool finishedTyping, int8_t receiverId = -1);

   //! read constructor
   MessagePacket();

   //! destructor
   virtual ~MessagePacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for sender id
   int8_t getSenderId() const;

   //! getter for the message
   QString getMessage() const;

   //! getter for the receiver's id
   int8_t getReceiverId() const;

   //! getter for finished-typing flag
   bool isTypingFinished() const;

private:
   //! sender id
   int8_t mSenderId;

   //! message to send
   QString mMessage;

   //! receiver id
   int8_t mReceiverId;

   //! player finished typing
   bool mFinishedTyping;
};

#endif  // MESSAGEPACKET_H
