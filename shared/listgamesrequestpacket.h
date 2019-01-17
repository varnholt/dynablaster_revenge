#ifndef LISTGAMESREQUESTPACKET_H
#define LISTGAMESREQUESTPACKET_H

#include "packet.h"

// Qt
#include <QString>

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
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

};

#endif // LISTGAMESREQUESTPACKET_H
