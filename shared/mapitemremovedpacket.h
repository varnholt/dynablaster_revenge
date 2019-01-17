#ifndef MAPITEMREMOVEDPACKET_H
#define MAPITEMREMOVEDPACKET_H

#include "mapitempacket.h"

class MapItem;

class MapItemRemovedPacket : public MapItemPacket
{

   public:

      //! write constructor
      MapItemRemovedPacket(MapItem *item);

      //! read constructor
      MapItemRemovedPacket();

      //! debugs the member variables
      virtual void debug();

      //! enqueues the member variables to datastream
      virtual void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      virtual void dequeue(QDataStream&);


   protected:
};

#endif // MAPITEMREMOVEDPACKET_H
