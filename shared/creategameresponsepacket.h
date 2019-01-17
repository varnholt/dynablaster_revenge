#ifndef CREATEGAMERESPONSEPACKET_H
#define CREATEGAMERESPONSEPACKET_H

// shared
#include "gameinformation.h"
#include "packet.h"

// Qt
#include <QString>

class CreateGameResponsePacket : public Packet
{

   public:

      //! write constructor
      CreateGameResponsePacket(const GameInformation& gameInformation);

      //! read constructor
      CreateGameResponsePacket();

      //! destructor
      virtual ~CreateGameResponsePacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game information
      const GameInformation& getGameInformation() const;


   private:

      //! game information
      GameInformation mGameInformation;
};

#endif // CREATEGAMERESPONSEPACKET_H
