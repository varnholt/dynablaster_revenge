#ifndef CREATEGAMEREQUESTPACKET_H
#define CREATEGAMEREQUESTPACKET_H

// base
#include "packet.h"

// shared
#include "constants.h"
#include "creategamedata.h"

#include <string>

class CreateGameRequestPacket : public Packet
{
public:
   //! write constructor
   CreateGameRequestPacket(
      const std::string& name,
      const std::string& level,
      int rounds,
      int duration,
      int maxPlayers,
      bool extraBombEnabled,
      bool extraFlameEnabled,
      bool extraSpeedupEnabled,
      bool extraKickEnabled,
      bool extraSkullsEnabled,
      Constants::Dimension dimension
   );

   //! read constructor
   CreateGameRequestPacket();

   //! destructor
   virtual ~CreateGameRequestPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for game name
   const std::string getName();

   //! getter for create game data
   CreateGameData getData() const;

private:
   CreateGameData mData;
};

#endif  // CREATEGAMEREQUESTPACKET_H
