//
// C++ Interface: positionpacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef POSITIONPACKET_H
#define POSITIONPACKET_H

#include "packet.h"

/**
	@author Matthias Varnholt <matto@gmx.de>
*/
class PositionPacket : public Packet
{

public:

   //! write constructor
   PositionPacket(
      qint8 playerId,
      qint8 directions,
      float x,
      float y,
      float angle = 0.0f,
      float xDelta = 0.0f,
      float yDelta = 0.0f,
      float angleDelta = 0.0f,
      float speed = 0.0f
   );

   //! read constructor
   PositionPacket();

   //! destructor
   virtual ~PositionPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(QDataStream&);

   //! dequeues the member variables from datastream
   void dequeue(QDataStream&);


   //! getter for player id
   qint8 getPlayerId();

   //! getter for player x position
   float getX() const;

   //! getter for player y position
   float getY() const;

   //! getter for player orientation angle
   float getAngle() const;

   //! getter for the player's directions
   qint8 getDirections();

   //! getter for player's x delta
   float getDeltaX() const;

   //! set player's x delta
   void setDeltaX(double deltax);

   //! getter for player's y delta
   float getDeltaY() const;

   //! set player's y delta
   void setDeltaY(double deltay);

   //! getter for player's rotation delta
   float getAngleDelta() const;

   //! getter for player speed
   float getSpeed() const;
   //! set player's rotation delta
   void setAngleDelta(float angleDelta);

private:

   //! player id
   qint8 mPlayerId;

   //! player's directions
   qint8 mDirections;

   //! player x position
   float mX;

   //! player y position
   float mY;

   //! player direction x
   float mDx;

   //! player direction y
   float mDy;

   //! player orientation
   float mAngle;

   //! player angle direction
   float mAngleDelta;

   //! player speed
   float mSpeed;
};

#endif

