//
// C++ Implementation: positionpacket
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

// header
#include "positionpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Position"


/*!----------------------------------------------------------------------------
   write constructor
   
   \param pId player id
   \param xPos player x position
   \param yPos player y position
*/
PositionPacket::PositionPacket(
   qint8 pId,
   qint8 dirs,
   float xPos,
   float yPos,
   float angle,
   float xDelta,
   float yDelta,
   float angleDelta,
   float speed
)
 : Packet(Packet::POSITION),
   mPlayerId(pId),
   mDirections(dirs),
   mX(xPos),
   mY(yPos),
   mDx(xDelta),
   mDy(yDelta),
   mAngle(angle),
   mAngleDelta(angleDelta),
   mSpeed(speed)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   read constructor
*/
PositionPacket::PositionPacket()
   : Packet(Packet::POSITION),
     mPlayerId(0),
     mDirections(0),
     mX(0.0f),
     mY(0.0f),
     mDx(0.0f),
     mDy(0.0f),
     mAngle(0.0f),
     mAngleDelta(0.0f),
     mSpeed(0.0f)
{
   mPacketName = PACKETNAME;
}


/*!----------------------------------------------------------------------------
   destructor
*/
PositionPacket::~PositionPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return player x position
*/
float PositionPacket::getX() const
{
   return mX;
}

//-----------------------------------------------------------------------------
/*!
   \return player delta x
*/
float PositionPacket::getDeltaX() const
{
   return mDx;
}


//-----------------------------------------------------------------------------
//! set player's x delta
void PositionPacket::setDeltaX(double deltax)
{
   mDx= deltax;
}



//-----------------------------------------------------------------------------
/*!
   \return player y position
*/
float PositionPacket::getY() const
{
   return mY;
}

//-----------------------------------------------------------------------------
/*!
   \return player delta y
*/
float PositionPacket::getDeltaY() const
{
   return mDy;
}

//-----------------------------------------------------------------------------
//! set player's y delta
void PositionPacket::setDeltaY(double deltay)
{
   mDy= deltay;
}

//-----------------------------------------------------------------------------
/*!
   \return player angle
*/
float PositionPacket::getAngle() const
{
   return mAngle;
}

//-----------------------------------------------------------------------------
/*!
   \return player angle delta
*/
float PositionPacket::getAngleDelta() const
{
    return mAngleDelta;
}


//-----------------------------------------------------------------------------
//! set player's rotation delta
void PositionPacket::setAngleDelta(float angleDelta)
{
   mAngleDelta= angleDelta;
}

//-----------------------------------------------------------------------------
/*!
   \return player speed
*/
float PositionPacket::getSpeed() const
{
   return mSpeed;
}


//-----------------------------------------------------------------------------
/*!
   \return player directions
*/
qint8 PositionPacket::getDirections()
{
   return mDirections;
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
qint8 PositionPacket::getPlayerId()
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void PositionPacket::enqueue(QDataStream & out)
{
   // write player id
   out << mPlayerId;

   // write directions
   out << mDirections;

   // write positions
   out << mX;
   out << mY;
   out << mAngle;

   // write deltas and speed
   out << mDx;
   out << mDy;
   out << mAngleDelta;   
   out << mSpeed;

   // debug();
}


/*!----------------------------------------------------------------------------
   \param in datastream read members from
*/
void PositionPacket::dequeue(QDataStream & in)
{
   // read player id, x, y
   in
      >> mPlayerId
      >> mDirections
      >> mX
      >> mY
      >> mAngle
      >> mDx
      >> mDy
      >> mAngleDelta
      >> mSpeed;

   // debug();
}


/*!----------------------------------------------------------------------------
   debug output of members
*/
void PositionPacket::debug()
{
   // output player id and x, y
   qDebug(
      "PositionPacket: player id: %d, position: (%f, %f, %f)  dir: (%f,%f,%f)",
      mPlayerId,
      mX,
      mY,
      mAngle,
      mDx,
      mDy,
      mAngleDelta
   );
}


