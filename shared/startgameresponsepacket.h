#ifndef STARTGAMERESPONSEPACKET_H
#define STARTGAMERESPONSEPACKET_H

#include "packet.h"

// Qt

class StartGameResponsePacket : public Packet
{
public:
   //! write constructor, -1 if not accepted
   StartGameResponsePacket(int id, bool started);

   //! read constructor
   StartGameResponsePacket();

   //! destructor
   virtual ~StartGameResponsePacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for game id
   int getId();

   //! getter for game is started flag
   bool isStarted();

private:
   //! game id
   int32_t mId;

   //! game was started
   bool mStarted;
};

#endif  // STARTGAMERESPONSEPACKET_H
