#ifndef STOPGAMEREQUESTPACKET_H
#define STOPGAMEREQUESTPACKET_H

#include "packet.h"

// Qt
#include <QString>

class StopGameRequestPacket : public Packet
{

   public:

      //! write constructor, -1 if not accepted
      StopGameRequestPacket(int id);

      //! read constructor
      StopGameRequestPacket();

      //! destructor
      virtual ~StopGameRequestPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game id
      int getId();


   private:

      //! game id
      qint32 mId;

};

#endif // STOPGAMEREQUESTPACKET_H
