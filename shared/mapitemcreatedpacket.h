#ifndef MAPITEMCREATEDPACKET_H
#define MAPITEMCREATEDPACKET_H

// shared
#include "mapitempacket.h"

class MapItemCreatedPacket : public MapItemPacket
{
public:
   //! write constructor
   MapItemCreatedPacket(MapItem*, int8_t creator = -1);

   //! read constructor
   MapItemCreatedPacket();

   //! debugs the member variables
   virtual void debug();

   //! enqueues the member variables to datastream
   virtual void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   virtual void dequeue(BinaryReader&);

   //! getter for the mapitem's appearance
   int32_t getAppearance() const;

   //! getter for mapitem's creator
   int8_t getPlayerId() const;

private:
   //! the mapitem's appearance
   int32_t mAppearance;

   //! the mapitem's creator
   int8_t mPlayerId;
};

#endif  // MAPITEMCREATEDPACKET_H
