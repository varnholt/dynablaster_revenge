#ifndef PLAYERINFECTEDPACKET_H
#define PLAYERINFECTEDPACKET_H

// base
#include "packet.h"

// constants
#include "constants.h"

class PlayerInfectedPacket : public Packet
{

public:

    //! constructor
    PlayerInfectedPacket();

    //! write constructor
    PlayerInfectedPacket(
       int32_t playerId,
       Constants::SkullType type
    );

    //! debugs the member variables
    void debug();

    //! enqueues the member variables to datastream
    void enqueue(QDataStream&);

    //! dequeues the member variables from datastream
    void dequeue(QDataStream&);

   //! getter for player id
   int32_t getPlayerId() const;

   //! getter for skull type
   Constants::SkullType getSkullType() const;

   //! getter for infector id
   int32_t getInfectorId() const;

   //! setter for infector id
   void setInfectorId(const int32_t &value);

   //! setter for extra position
   void setExtraPos(uint8_t x, uint8_t y);

   //! getter for extra position x
   uint8_t getExtraPosX() const;

   //! getter for extra position y
   uint8_t getExtraPosY() const;


private:

   //! player id
   int32_t mPlayerId;

   //! skull type
   Constants::SkullType mSkullType;

   //! player id of infector
   int32_t mInfectorId;

   //! x position where extra has been picked up
   uint8_t mExtraPosX;

   //! x position where extra has been picked up
   uint8_t mExtraPosY;
};

#endif // PLAYERINFECTEDPACKET_H
