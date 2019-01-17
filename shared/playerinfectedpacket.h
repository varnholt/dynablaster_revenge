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
       qint32 playerId,
       Constants::SkullType type
    );

    //! debugs the member variables
    void debug();

    //! enqueues the member variables to datastream
    void enqueue(QDataStream&);

    //! dequeues the member variables from datastream
    void dequeue(QDataStream&);

   //! getter for player id
   qint32 getPlayerId() const;

   //! getter for skull type
   Constants::SkullType getSkullType() const;

   //! getter for infector id
   qint32 getInfectorId() const;

   //! setter for infector id
   void setInfectorId(const qint32 &value);

   //! setter for extra position
   void setExtraPos(quint8 x, quint8 y);

   //! getter for extra position x
   quint8 getExtraPosX() const;

   //! getter for extra position y
   quint8 getExtraPosY() const;


private:

   //! player id
   qint32 mPlayerId;

   //! skull type
   Constants::SkullType mSkullType;

   //! player id of infector
   qint32 mInfectorId;

   //! x position where extra has been picked up
   quint8 mExtraPosX;

   //! x position where extra has been picked up
   quint8 mExtraPosY;
};

#endif // PLAYERINFECTEDPACKET_H
