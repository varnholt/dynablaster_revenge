#ifndef MAPITEMCREATEDPACKET_H
#define MAPITEMCREATEDPACKET_H

// shared
#include "mapitempacket.h"

class MapItemCreatedPacket : public MapItemPacket
{

   public:

      //! write constructor
      MapItemCreatedPacket(MapItem*, qint8 creator = -1);

      //! read constructor
      MapItemCreatedPacket();

      //! debugs the member variables
      virtual void debug();

      //! enqueues the member variables to datastream
      virtual void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      virtual void dequeue(QDataStream&);

      //! getter for the mapitem's appearance
      qint32 getAppearance() const;

      //! getter for mapitem's creator
      qint8 getPlayerId() const;


   private:

      //! the mapitem's appearance
      qint32 mAppearance;

      //! the mapitem's creator
      qint8 mPlayerId;
};

#endif // MAPITEMCREATEDPACKET_H
