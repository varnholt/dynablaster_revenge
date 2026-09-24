#ifndef STONEDROPPACKET_H
#define STONEDROPPACKET_H

#include "packet.h"

class StoneDropPacket : public Packet
{
   public:

      //! write constructor
      StoneDropPacket(
         int8_t x,
         int8_t y
      );

      //! read constructor
      StoneDropPacket();

      //! destructor
      virtual ~StoneDropPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);


      //! getter for stone x position
      int8_t getX() const;

      //! getter for stone y position
      int8_t getY() const;


   private:

      //! x position
      int8_t mX;

      //! y position
      int8_t mY;
};

#endif // STONEDROPPACKET_H


