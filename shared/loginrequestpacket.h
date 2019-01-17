//
// C++ Interface: loginrequestpacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LOGINREQUESTPACKET_H
#define LOGINREQUESTPACKET_H

// base
#include "packet.h"


/**
	@author Matthias Varnholt <matto@gmx.de>
*/
class LoginRequestPacket : public Packet
{

public:

   //! write constructor
   LoginRequestPacket(
      const QString& mNick,
      bool bot
   );

   //! read constructor
   LoginRequestPacket();

   //! destructor
   virtual ~LoginRequestPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(QDataStream&);

   //! dequeues the member variables from datastream
   void dequeue(QDataStream&);

   //! getter for player nick
   const QString& getNick() const;

   //! getter for bot flag
   bool isBot() const;


private:

   //! player nick
   QString mNick;

   //! player is a bot
   bool mBot;
};

#endif

