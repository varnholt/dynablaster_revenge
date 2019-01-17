#ifndef COUNTDOWNPACKET_H
#define COUNTDOWNPACKET_H

#include "packet.h"

class CountdownPacket : public Packet
{

   public:

      //! write constructor
      CountdownPacket(int timeLeft);

      //! read constructor
      CountdownPacket();

      //! destructor
      virtual ~CountdownPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for remaining time
      int getTimeLeft();


   private:

      //! countdown
      qint8 mTimeLeft;
};

#endif // COUNTDOWNPACKET_H
