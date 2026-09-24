#ifndef LOGINRESPONSEPACKET_H
#define LOGINRESPONSEPACKET_H

// base
#include "packet.h"

// shared
#include "serverconfiguration.h"

class LoginResponsePacket : public Packet
{

public:

   //! write constructor
   LoginResponsePacket(
      bool broadcasted,
      int32_t id,
      const QString& nick,
      const ServerConfiguration& serverConfig
   );

   //! read constructor
   LoginResponsePacket();

   //! destructor
   virtual ~LoginResponsePacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(QDataStream&);

   //! dequeues the member variables from datastream
   void dequeue(QDataStream&);

   //! setter for the id
   void setId(int32_t id);

   //! getter for the id
   int32_t getId() const;

   //! getter for player nick
   const QString& getNick() const;

   //! getter for broadcast flag
   bool isBroadcast() const;

   //! getter for server configuration
   const ServerConfiguration& getServerConfiguration() const;


private:

   //! individual allowance or broadcast packet
   bool mBroadcast;

   //! player's
   int32_t mId;

   //! player nick
   QString mNick;

   //! server configuration data
   ServerConfiguration mServerConfiguration;

};

#endif

