//
// C++ Interface: bombpacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BOMBPACKET_H
#define BOMBPACKET_H

#include "packet.h"

/**
	@author Matthias Varnholt <matto@gmx.de>
*/
class BombPacket : public Packet
{

public:

   //! write constructor
   BombPacket(
      qint8 playerId,
      quint8 x,
      quint8 y
   );

   //! read constructor
   BombPacket();

   //! destructor
   virtual ~BombPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(QDataStream&);

   //! dequeues the member variables from datastream
   void dequeue(QDataStream&);

   //! getter for player id
   qint8 getPlayerId();

   //! getter for the bomb's x field position
   quint8 getX();

   //! getter for the bomb's y field position
   quint8 getY();


private:

   //! player id
   qint8 playerId;

   //! the bomb's x field position
   quint8 x;

   //! the bomb's x field position
   quint8 y;

};

#endif

