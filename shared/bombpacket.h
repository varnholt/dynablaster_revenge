#ifndef BOMBPACKET_H
#define BOMBPACKET_H

#include "packet.h"

class BombPacket : public Packet
{

public:

   //! write constructor
   BombPacket(
      int8_t playerId,
      uint8_t x,
      uint8_t y
   );

   //! read constructor
   BombPacket();

   //! destructor
   virtual ~BombPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(QDataStream&);

   //! dequeues the member variables from datastream
   void dequeue(QDataStream&);

   //! getter for player id
   int8_t getPlayerId();

   //! getter for the bomb's x field position
   uint8_t getX();

   //! getter for the bomb's y field position
   uint8_t getY();


private:

   //! player id
   int8_t playerId;

   //! the bomb's x field position
   uint8_t x;

   //! the bomb's x field position
   uint8_t y;

};

#endif

