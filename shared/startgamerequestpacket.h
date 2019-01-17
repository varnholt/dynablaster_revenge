#ifndef STARTGAMEREQUESTPACKET_H
#define STARTGAMEREQUESTPACKET_H

#include "packet.h"

// Qt
#include <QString>

class StartGameRequestPacket : public Packet
{

   public:

      //! write constructor
      StartGameRequestPacket(int id);

      //! read constructor
      StartGameRequestPacket();

      //! destructor
      virtual ~StartGameRequestPacket();

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

#endif // STARTGAMEREQUESTPACKET_H
