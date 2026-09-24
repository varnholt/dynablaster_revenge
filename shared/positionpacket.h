#ifndef POSITIONPACKET_H
#define POSITIONPACKET_H

#include "packet.h"


class PositionPacket : public Packet
{

public:

   //! write constructor
   PositionPacket(
      int8_t playerId,
      int8_t directions,
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
   int8_t getPlayerId();

   //! getter for player x position
   float getX() const;

   //! getter for player y position
   float getY() const;

   //! getter for player orientation angle
   float getAngle() const;

   //! getter for the player's directions
   int8_t getDirections();

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
   int8_t mPlayerId;

   //! player's directions
   int8_t mDirections;

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

