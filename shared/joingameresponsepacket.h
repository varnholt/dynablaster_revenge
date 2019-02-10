#ifndef JOINGAMERESPONSEPACKET_H
#define JOINGAMERESPONSEPACKET_H

// base
#include "packet.h"

// shared
#include "constants.h"

class JoinGameResponsePacket : public Packet
{

public:

   //! write constructor
   JoinGameResponsePacket(
      bool success,
      qint32 gameid,
      qint32 id,
      const QString& nick,
      Constants::Color color
   );

   //! read constructor
   JoinGameResponsePacket();

   //! destructor
   virtual ~JoinGameResponsePacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(QDataStream&);

   //! dequeues the member variables from datastream
   void dequeue(QDataStream&);

   //! setter for the game id
   void setGameId(qint32 id);

   //! getter for the game id
   qint32 getGameId() const;

   //! setter for the player id
   void setPlayerId(qint32 id);

   //! getter for the player id
   qint32 getPlayerId() const;

   //! getter for player nick
   const QString& getNick() const;

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
   qint32 mGameId;

   //! player's
   qint32 mPlayerId;

   //! player nick
   QString mNick;

   //! player's color
   Constants::Color mColor;
};

#endif

