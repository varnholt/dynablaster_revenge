#ifndef JOINGAMERESPONSEPACKET_H
#define JOINGAMERESPONSEPACKET_H

#include <string>

// base
#include "packet.h"

// shared
#include "constants.h"

class JoinGameResponsePacket : public Packet
{
public:
   //! write constructor
   JoinGameResponsePacket(bool success, int32_t gameid, int32_t id, const std::string& nick, Constants::Color color);

   //! read constructor
   JoinGameResponsePacket();

   //! destructor
   virtual ~JoinGameResponsePacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! setter for the game id
   void setGameId(int32_t id);

   //! getter for the game id
   int32_t getGameId() const;

   //! setter for the player id
   void setPlayerId(int32_t id);

   //! getter for the player id
   int32_t getPlayerId() const;

   //! getter for player nick
   const std::string& getNick() const;

   //! getter for successful flag
   bool isSuccessful() const;

   //! setter for player color
   void setColor(Constants::Color color);

   //! getter for player color
   Constants::Color getColor() const;

private:
   //! successfully joined
   bool mSuccess;

   //! game's id
   int32_t mGameId;

   //! player's
   int32_t mPlayerId;

   //! player nick
   std::string mNick;

   //! player's color
   Constants::Color mColor;
};

#endif
