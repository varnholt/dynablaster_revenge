//
// C++ Interface: keypacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KEYPACKET_H
#define KEYPACKET_H

// base
#include "packet.h"

// shared
#include "constants.h"


/**
	@author Matthias Varnholt <matto@gmx.de>
*/
class KeyPacket : public Packet
{

public:
   
   //! read constructor
   KeyPacket();

   //! write constructor
   KeyPacket(
      qint8 playerId,
      qint8 keys
   );

   //! destructor
   virtual ~KeyPacket();

   //! enqueue member variables
   void enqueue(QDataStream&);

   //! dequeue member variables
   void dequeue(QDataStream&);

   //! getter for key combination
   qint8 getKeys();

   //! getter for player id
   qint8 getPlayerId();

   //! debug function
   void debug();

private:

   //! player id
   qint8 playerId;

   //! key combination
   qint8 keys;

};

#endif

