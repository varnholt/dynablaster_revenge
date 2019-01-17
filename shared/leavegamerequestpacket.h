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
      LeaveGameRequestPacket(
         qint32 gameId,
         qint32 playerId
      );

      //! destructor
      virtual ~LeaveGameRequestPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);


      // getters and setters

      //! setter for the game id
      void setGameId(qint32 id);

      //! getter for the game id
      qint32 getGameId() const;

      //! setter for the player id
      void setPlayerId(qint32 id);

      //! getter for the player id
      qint32 getPlayerId() const;


   protected:

      //! game's id
      qint32 mGameId;

      //! player's
      qint32 mPlayerId;


};

#endif // LEAVEGAMEREQUESTPACKET_H
