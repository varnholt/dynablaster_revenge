#ifndef MESSAGEPACKET_H
#define MESSAGEPACKET_H

#include "packet.h"

class MessagePacket : public Packet
{
   public:

      //! write constructor
      MessagePacket(
         qint8 senderId,
         const QString& message,
         bool finishedTyping,
         qint8 receiverId = -1
      );

      //! read constructor
      MessagePacket();

      //! destructor
      virtual ~MessagePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for sender id
      qint8 getSenderId() const;

      //! getter for the message
      QString getMessage() const;

      //! getter for the receiver's id
      qint8 getReceiverId() const;

      //! getter for finished-typing flag
      bool isTypingFinished() const;


   private:

      //! sender id
      qint8 mSenderId;

      //! message to send
      QString mMessage;

      //! receiver id
      qint8 mReceiverId;

      //! player finished typing
      bool mFinishedTyping;

};

#endif // MESSAGEPACKET_H

