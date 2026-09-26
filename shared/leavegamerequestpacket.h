#ifndef LEAVEGAMEREQUESTPACKET_H
#define LEAVEGAMEREQUESTPACKET_H

#include "packet.h"

// Qt
#include <QString>

class LeaveGameRequestPacket : public Packet
{
public:
   //! constructor
   LeaveGameRequestPacket();

   //! constructor
   LeaveGameRequestPacket(int32_t gameId, int32_t playerId);

   //! destructor
   virtual ~LeaveGameRequestPacket();

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

#endif  // LEAVEGAMEREQUESTPACKET_H
