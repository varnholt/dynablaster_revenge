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
   PlayerKilledPacket(int32_t playerId, int32_t playerKilledById, Constants::Direction direction, float intensity);

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for player id
   int32_t getPlayerId() const;

private:
   //! player id
   int32_t mPlayerId;

   //! killed by id
   int32_t mPlayerKilledById;

   //! direction the player was killed from
   Constants::Direction mDirection;

   //! intensity the player was killed with
   float mIntensity;
};

#endif  // PLAYERKILLEDPACKET_H
