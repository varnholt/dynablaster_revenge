#ifndef STONEDROPPACKET_H
#define STONEDROPPACKET_H

#include "packet.h"

class StoneDropPacket : public Packet
{
   public:

      //! write constructor
      StoneDropPacket(
         qint8 x,
         qint8 y
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
      qint8 getX() const;

      //! getter for stone y position
      qint8 getY() const;


   private:

      //! x position
      qint8 mX;

      //! y position
      qint8 mY;
};

#endif // STONEDROPPACKET_H


