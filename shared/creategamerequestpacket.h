#ifndef CREATEGAMEREQUESTPACKET_H
#define CREATEGAMEREQUESTPACKET_H

// base
#include "packet.h"

// shared
#include "constants.h"
#include "creategamedata.h"

// Qt
#include <QString>

class CreateGameRequestPacket : public Packet
{

   public:

      //! write constructor
      CreateGameRequestPacket(
         const QString& name,
         const QString& level,
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
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);

      //! getter for game name
      const QString getName();

      //! getter for create game data
      CreateGameData getData() const;


   private:

      CreateGameData mData;
};

#endif // CREATEGAMEREQUESTPACKET_H
