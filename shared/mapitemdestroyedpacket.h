#ifndef MAPITEMDESTROYEDPACKET_H
#define MAPITEMDESTROYEDPACKET_H

// base
#include "mapitemremovedpacket.h"

// constants
#include "constants.h"

class MapItemDestroyedPacket : public MapItemPacket
{
   public:

      //! read constructor
      MapItemDestroyedPacket();

      //! write constructor
      MapItemDestroyedPacket(
         MapItem* item,
         int32_t playerId,
         Constants::Direction direction,
         float intensity
      );

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for destroyer id
      int32_t getPlayerId() const;

      //! getter for direction
      Constants::Direction getDirection() const;

      //! getter for intensity of destruction
      float getIntensity() const;


   private:

      //! player id
      int32_t mPlayerId;

      //! direction the item was destroyed from
      Constants::Direction mDirection;

      //! intensity the item was destroyed with
      float mIntensity;
};

#endif // MAPITEMDESTROYEDPACKET_H

