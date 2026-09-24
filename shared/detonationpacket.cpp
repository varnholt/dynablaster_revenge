// header
#include "detonationpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "Detonation"


//-----------------------------------------------------------------------------
/*!
   \param x x center
   \param y y center
   \param fieldsUp fields the bomb goes up
   \param fieldsDown fields the bomb goes down
   \param fieldsLeft fields the bomb goes to the left
   \param fieldsRight fields the bomb goes to the right
*/
DetonationPacket::DetonationPacket(
   int32_t x,
   int32_t y,
   int8_t fieldsUp,
   int8_t fieldsDown,
   int8_t fieldsLeft,
   int8_t fieldsRight,
   float intensity
)
   : Packet(Packet::DETONATION),
     mX(x),
     mY(y),
     mFieldsUp(fieldsUp),
     mFieldsDown(fieldsDown),
     mFieldsLeft(fieldsLeft),
     mFieldsRight(fieldsRight),
     mIntensity(intensity)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
DetonationPacket::DetonationPacket()
   : Packet(Packet::DETONATION),
     mX(0),
     mY(0),
     mFieldsUp(0),
     mFieldsDown(0),
     mFieldsLeft(0),
     mFieldsRight(0),
     mIntensity(0.0f)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void DetonationPacket::enqueue(QDataStream & out)
{
   // write members
   out << mX;
   out << mY;
   out << mFieldsUp;
   out << mFieldsDown;
   out << mFieldsLeft;
   out << mFieldsRight;
   out << mIntensity;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void DetonationPacket::dequeue(QDataStream & in)
{

   // read members
   in
      >> mX
      >> mY
      >> mFieldsUp
      >> mFieldsDown
      >> mFieldsLeft
      >> mFieldsRight
      >> mIntensity;
}

int32_t DetonationPacket::getX() const
{
	return mX;
}

int32_t DetonationPacket::getY() const
{
	return mY;
}

int8_t DetonationPacket::getUp() const
{
	return mFieldsUp;
}

int8_t DetonationPacket::getDown() const
{
	return mFieldsDown;
}

int8_t DetonationPacket::getLeft() const
{
	return mFieldsLeft;
}

int8_t DetonationPacket::getRight() const
{
   return mFieldsRight;
}

float DetonationPacket::getIntensity() const
{
   return mIntensity;
}

//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void DetonationPacket::debug()
{
   // output packet members
   qDebug(
      "DetonationPacket: x: %d, y: %d, "
      "up: %d, down: %d, left: %d, right: %d, "
      "intensity: %f",
      mX,
      mY,
      mFieldsUp,
      mFieldsDown,
      mFieldsLeft,
      mFieldsRight,
      mIntensity
   );
}

