#ifndef LISTGAMESREQUESTPACKET_H
#define LISTGAMESREQUESTPACKET_H

#include "packet.h"

// Qt

class ListGamesRequestPacket : public Packet
{
public:
   //! constructor
   ListGamesRequestPacket();

   //! destructor
   virtual ~ListGamesRequestPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);
};

#endif  // LISTGAMESREQUESTPACKET_H
