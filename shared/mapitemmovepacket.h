#pragma once

// base
#include "packet.h"

// shared
#include "constants.h"


class MapItemMovePacket : public Packet
{   
   public:

      //! write constructor
      MapItemMovePacket(
         int mapItemId,
         float speed,
         Constants::Direction direction,
         int nominalX = -1,
         int nominalY = -1
      );

      //! read constructor
      MapItemMovePacket();

      //! destructor
      virtual ~MapItemMovePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);


      //! getter for object id
      int getMapItemId();

      //! getter mapitem speed
      float getSpeed() const;

      //! getter for mapitem direction
      Constants::Direction getDirection() const;

      //! getter for nominal x position
      int getNominalX() const;

      //! getter for nominal y position
      int getNominalY() const;


   private:

      //! unique mapitem id
      int mMapItemId;

      //! mapitem speed
      float mSpeed;

      //! mapitem direction
      Constants::Direction mDirection;

      //! nominal x
      int mNominalX;

      //! nominal y
      int mNominalY;
};



