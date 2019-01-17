//
// C++ Implementation: ObjectMovedPacket
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
#include "mapitemmovepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "MapItemMove"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param pId player id
   \param xPos player x position
   \param yPos player y position
   \param angle optional rotation angle
*/
MapItemMovePacket::MapItemMovePacket(
   int id,
   float speed,
   Constants::Direction dir,
   int nominalX,
   int nominalY
)
 : Packet(Packet::MAPITEMMOVE),
   mMapItemId(id),
   mSpeed(speed),
   mDirection(dir),
   mNominalX(nominalX),
   mNominalY(nominalY)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
MapItemMovePacket::MapItemMovePacket()
   : Packet(Packet::MAPITEMMOVE),
     mMapItemId(0),
     mSpeed(0.0),
     mDirection(Constants::DirectionUnknown),
     mNominalX(0),
     mNominalY(0)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
MapItemMovePacket::~MapItemMovePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return player x position
*/
float MapItemMovePacket::getSpeed() const
{
   return mSpeed;
}


//-----------------------------------------------------------------------------
/*!
   \return mapitem direction
*/
Constants::Direction MapItemMovePacket::getDirection() const
{
   return mDirection;
}


//-----------------------------------------------------------------------------
/*!
   \return nominal x
*/
int MapItemMovePacket::getNominalX() const
{
   return mNominalX;
}


//-----------------------------------------------------------------------------
/*!
   \return nominal y
*/
int MapItemMovePacket::getNominalY() const
{
   return mNominalY;
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
int MapItemMovePacket::getMapItemId()
{
   return mMapItemId;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MapItemMovePacket::enqueue(QDataStream & out)
{
   // write player id
   out << mMapItemId;
   out << mSpeed;
   out << (qint8)mDirection;
   out << mNominalX;
   out << mNominalY;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MapItemMovePacket::dequeue(QDataStream & in)
{
   qint8 direction = 0;

   in
      >> mMapItemId
      >> mSpeed
      >> direction
      >> mNominalX
      >> mNominalY;

   mDirection = (Constants::Direction)direction;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void MapItemMovePacket::debug()
{
   // output player id and x, y
   qDebug(
      "MapItemMovePacket: unique id: %d, speed: %f, nominal: %d, %d",
      mMapItemId,
      mSpeed,
      mNominalX,
      mNominalY
   );
}


