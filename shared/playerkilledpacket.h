#ifndef PLAYERKILLEDPACKET_H
#define PLAYERKILLEDPACKET_H

// base
#include "packet.h"

// constants
#include "constants.h"


class PlayerKilledPacket : public Packet
{
   public:

      //! read constructor
      PlayerKilledPacket();

      //! write constructor
      PlayerKilledPacket(
         qint32 playerId,
         qint32 playerKilledById,
         Constants::Direction direction,
         float intensity
      );

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

	  //! getter for player id
	  qint32 getPlayerId() const;


   private:

      //! player id
      qint32 mPlayerId;

      //! killed by id
      qint32 mPlayerKilledById;

      //! direction the player was killed from
      Constants::Direction mDirection;

      //! intensity the player was killed with
      float mIntensity;
};

#endif // PLAYERKILLEDPACKET_H
