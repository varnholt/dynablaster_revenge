#ifndef MESSAGEPACKET_H
#define MESSAGEPACKET_H

#include <string>

#include "packet.h"

class MessagePacket : public Packet
{
public:
   //! write constructor
   MessagePacket(int8_t senderId, const std::string& message, bool finishedTyping, int8_t receiverId = -1);

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
   std::string getMessage() const;

   //! getter for the receiver's id
   int8_t getReceiverId() const;

   //! getter for finished-typing flag
   bool isTypingFinished() const;

private:
   //! sender id
   int8_t mSenderId;

   //! message to send
   std::string mMessage;

   //! receiver id
   int8_t mReceiverId;

   //! player finished typing
   bool mFinishedTyping;
};

#endif  // MESSAGEPACKET_H
