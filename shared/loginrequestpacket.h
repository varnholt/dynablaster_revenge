#ifndef LOGINREQUESTPACKET_H
#define LOGINREQUESTPACKET_H

#include <string>

// base
#include "packet.h"

class LoginRequestPacket : public Packet
{
public:
   //! write constructor
   LoginRequestPacket(const std::string& mNick, bool bot);

   //! read constructor
   LoginRequestPacket();

   //! destructor
   virtual ~LoginRequestPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

   //! getter for player nick
   const std::string& getNick() const;

   //! getter for bot flag
   bool isBot() const;

private:
   //! player nick
   std::string mNick;

   //! player is a bot
   bool mBot;
};

#endif
