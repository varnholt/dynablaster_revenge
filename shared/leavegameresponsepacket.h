#ifndef LeaveGameResponsePacket_H
#define LeaveGameResponsePacket_H

#include "packet.h"

// Qt

class LeaveGameResponsePacket : public Packet
{
public:
   //! constructor
   LeaveGameResponsePacket();

   //! constructor
   LeaveGameResponsePacket(int32_t gameId, int32_t playerId);

   //! destructor
   virtual ~LeaveGameResponsePacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   // getters and setters

   //! setter for the game id
   void setGameId(int32_t id);

   //! getter for the game id
   int32_t getGameId() const;

   //! setter for the player id
   void setPlayerId(int32_t id);

   //! getter for the player id
   int32_t getPlayerId() const;

protected:
   //! game's id
   int32_t mGameId;

   //! player's
   int32_t mPlayerId;
};

#endif  // LeaveGameResponsePacket_H
