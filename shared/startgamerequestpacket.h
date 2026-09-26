#ifndef STARTGAMEREQUESTPACKET_H
#define STARTGAMEREQUESTPACKET_H

#include "packet.h"

// Qt

class StartGameRequestPacket : public Packet
{
public:
   //! write constructor
   StartGameRequestPacket(int id);

   //! read constructor
   StartGameRequestPacket();

   //! destructor
   virtual ~StartGameRequestPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for game id
   int getId();

private:
   //! game id
   int32_t mId;
};

#endif  // STARTGAMEREQUESTPACKET_H
