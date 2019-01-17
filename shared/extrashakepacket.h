#ifndef EXTRASHAKEPACKET_H
#define EXTRASHAKEPACKET_H

#include "packet.h"

class ExtraShakePacket : public Packet
{
   public:

      //! write constructor
      ExtraShakePacket(int uniqueId);

      //! read constructor
      ExtraShakePacket();

      //! destructor
      virtual ~ExtraShakePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for item unique id
      int getMapItemUniqueId() const;


   private:

      //! unique id
      int mMapItemUniqueId;
};

#endif // ExtraShakePacket_H


