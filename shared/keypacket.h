#ifndef KEYPACKET_H
#define KEYPACKET_H

// base
#include "packet.h"

// shared
#include "constants.h"


class KeyPacket : public Packet
{

public:

   //! read constructor
   KeyPacket();

   //! write constructor
   KeyPacket(
      int8_t playerId,
      int8_t keys
   );

   //! destructor
   virtual ~KeyPacket();

   //! enqueue member variables
   void enqueue(QDataStream&);

   //! dequeue member variables
   void dequeue(QDataStream&);

   //! getter for key combination
   int8_t getKeys();

   //! getter for player id
   int8_t getPlayerId();

   //! debug function
   void debug();

private:

   //! player id
   int8_t playerId;

   //! key combination
   int8_t keys;

};

#endif

