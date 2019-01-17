#ifndef JOINGAMEREQUESTPACKET_H
#define JOINGAMEREQUESTPACKET_H

#include "packet.h"

class JoinGameRequestPacket : public Packet
{

   public:

      //! write constructor
      JoinGameRequestPacket(int id);

      //! read constructor
      JoinGameRequestPacket();

      //! destructor
      virtual ~JoinGameRequestPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game id
      int getId();


   private:

      //! game's id
      qint8 mId;
};

#endif // JOINGAMEREQUESTPACKET_H
