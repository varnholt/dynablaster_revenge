#ifndef STOPGAMERESPONSEPACKET_H
#define STOPGAMERESPONSEPACKET_H

#include "packet.h"

// Qt
#include <QString>

class StopGameResponsePacket : public Packet
{

   public:

      //! write constructor, -1 if not accepted
      StopGameResponsePacket(int id, bool finished);

      //! read constructor
      StopGameResponsePacket();

      //! destructor
      virtual ~StopGameResponsePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game id
      int getId();

      //! getter for stopped flag
      bool isFinished();


   private:

      //! game id
      qint32 mId;

      //! finished flag
      bool mFinished;
};

#endif // STOPGAMERESPONSEPACKET_H
