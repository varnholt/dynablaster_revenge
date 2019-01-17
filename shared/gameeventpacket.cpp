// header
#include "gameeventpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "GameEvent"


//-----------------------------------------------------------------------------
/*!
   \param event game event
   \param intensity event intensity
*/
GameEventPacket::GameEventPacket(
   GameEvent event,
   float intensity,
   int x,
   int y
)
   : Packet(Packet::GAMEEVENT),
     mEvent(event),
     mIntensity(intensity),
     mPlayerId(-1),
     mExtraType(Constants::ExtraBomb),
     mX(x),
     mY(y)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
GameEventPacket::GameEventPacket()
   : Packet(Packet::GAMEEVENT),
     mEvent(Invalid),
     mIntensity(1.0f),
     mPlayerId(-1),
     mExtraType(Constants::ExtraBomb),
     mX(-1),
     mY(-1)
{
   mPacketName = PACKETNAME;
}

//-----------------------------------------------------------------------------
/*!
   \return game event intensity
*/
float GameEventPacket::getIntensity() const
{
   return mIntensity;
}


//-----------------------------------------------------------------------------
/*!
   \return game event type
*/
GameEventPacket::GameEvent GameEventPacket::getGameEvent() const
{
   return mEvent;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void GameEventPacket::enqueue(QDataStream & out)
{
   // write members
   out
      << mEvent
      << mIntensity
      << mPlayerId
      << mExtraType
      << mX
      << mY;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void GameEventPacket::dequeue(QDataStream & in)
{
   qint32 event = 0;
   qint32 extra = 0;

   // read members
   in
      >> event
      >> mIntensity
      >> mPlayerId
      >> extra
      >> mX
      >> mY;

   mEvent = (GameEvent)event;
   mExtraType = (Constants::ExtraType)extra;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void GameEventPacket::debug()
{
   // output packet members
   qDebug(
      "GameEventPacket: type: %d, intensity: %f",
      mEvent,
      mIntensity
   );
}


//-----------------------------------------------------------------------------
/*!
   \param id player id
*/
void GameEventPacket::setPlayerId(qint32 id)
{
   mPlayerId = id;
}



//-----------------------------------------------------------------------------
/*!
   \param extra extra type
*/
void GameEventPacket::setExtraType(Constants::ExtraType extra)
{
   mExtraType = extra;
}


//-----------------------------------------------------------------------------
/*!
   \return player id
*/
qint32 GameEventPacket::getPlayerId() const
{
   return mPlayerId;
}


//-----------------------------------------------------------------------------
/*!
   \return extra type
*/
Constants::ExtraType GameEventPacket::getExtraType() const
{
   return mExtraType;
}


//-----------------------------------------------------------------------------
/*!
   \return x position of event
*/
int GameEventPacket::getX() const
{
   return mX;
}


//-----------------------------------------------------------------------------
/*!
   \return y position of event
*/
int GameEventPacket::getY() const
{
   return mY;
}

