#ifndef TIMEPACKET_H
#define TIMEPACKET_H

// base
#include "packet.h"

// constants
#include "constants.h"


class TimePacket : public Packet
{
   public:

      //! read constructor
      TimePacket();

      //! write constructor
      TimePacket(int timeLeft);

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

     //! getter for elapsed time
     int getTimeLeft() const;


   private:

      //! elapsed time in ms
      qint32 mTimeLeft;
};

#endif // TIMEPACKET_H
