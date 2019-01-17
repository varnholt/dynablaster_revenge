//
// C++ Implementation: packet
//
// Description:
//
//
// Author: Matthias Varnholt <matto@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "packet.h"

// qt
#include <QDataStream>

// shared
#include "bombpacket.h"
#include "countdownpacket.h"
#include "creategamerequestpacket.h"
#include "creategameresponsepacket.h"
#include "detonationpacket.h"
#include "errorpacket.h"
#include "extrashakepacket.h"
#include "extramapitemcreatedpacket.h"
#include "gameeventpacket.h"
#include "gamestatspacket.h"
#include "joingamerequestpacket.h"
#include "joingameresponsepacket.h"
#include "keypacket.h"
#include "leavegamerequestpacket.h"
#include "leavegameresponsepacket.h"
#include "listgamesrequestpacket.h"
#include "listgamesresponsepacket.h"
#include "loginrequestpacket.h"
#include "loginresponsepacket.h"
#include "mapcreaterequestpacket.h"
#include "mapitemcreatedpacket.h"
#include "mapitemdestroyedpacket.h"
#include "mapitemmovepacket.h"
#include "mapitemremovedpacket.h"
#include "messagepacket.h"
#include "playerinfectedpacket.h"
#include "playerkilledpacket.h"
#include "playermodifiedpacket.h"
#include "playersynchronizepacket.h"
#include "positionpacket.h"
#include "startgamerequestpacket.h"
#include "startgameresponsepacket.h"
#include "stonedroppacket.h"
#include "stopgamerequestpacket.h"
#include "stopgameresponsepacket.h"
#include "timepacket.h"

//-----------------------------------------------------------------------------
/*!
   read constructor
*/
Packet::Packet()
   : mPacketSize(0),
     mPacketType(INVALID),
     mPacketName("INVALID")
{
   mTimestamp = QTime::currentTime();
}


//-----------------------------------------------------------------------------
/*!
   write constructor
*/
Packet::Packet(
   TYPE type
)
   : mPacketSize(0),
     mPacketType(type)
{
   mTimestamp = QTime::currentTime();
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
Packet::~Packet()
{
}

//-----------------------------------------------------------------------------
/*!
   \return packet size
*/
qint16 Packet::getSize()
{
   return mPacketSize;
}


//-----------------------------------------------------------------------------
/*!
   \return packet type
*/
Packet::TYPE Packet::getType()
{
   return mPacketType;
}


//-----------------------------------------------------------------------------
/*!
   \return packet timestamp
*/
const QTime Packet::getTimestamp()
{
   return mTimestamp;
}


//-----------------------------------------------------------------------------
/*!
   \param time new timestamp
*/
void Packet::setTimeStamp(const QTime &time)
{
   mTimestamp = time;
}


//-----------------------------------------------------------------------------
/*!
   \return packet name
*/
const QString& Packet::getPacketName() const
{
   return mPacketName;
}


//-----------------------------------------------------------------------------
/*!
   serialize a packet
*/
void Packet::serialize()
{
   // build outgoing data
   QDataStream out(this, QIODevice::WriteOnly);
   out.setVersion(QDataStream::Qt_4_6);

   // reserve 16 bits for the packet packetSize
   out << (quint16)0;

   // write packet packetType
   out << (quint8)mPacketType;

   // write timestamp to packet
   out << mTimestamp;

   enqueue(out);

   // write the blocksize at the beginning of the bytearray
   out.device()->seek(0);
   out << (quint16)(size() - sizeof(quint16));
}


/*!----------------------------------------------------------------------------
   deserialize a packet

   \param in input datastream
   \return a packet of the correct type with all member variables filled
*/
Packet* Packet::deserialize(QDataStream& in)
{
   qint8 pType;

   // read the serialized data
   in >> pType;

   Packet* packet = 0;

   switch (pType)
   {
      case Packet::BOMB:
         packet = new BombPacket();
         break;
      case Packet::COUNTDOWN:
         packet = new CountdownPacket();
         break;
      case Packet::CREATEGAMEREQUEST:
         packet = new CreateGameRequestPacket();
         break;
      case Packet::CREATEGAMERESPONSE:
         packet = new CreateGameResponsePacket();
         break;
      case Packet::DETONATION:
         packet = new DetonationPacket();
         break;
      case Packet::EXTRAMAPITEMCREATED:
         packet = new ExtraMapItemCreatedPacket();
         break;
      case Packet::EXTRASHAKE:
         packet = new ExtraShakePacket();
         break;
      case Packet::GAMEEVENT:
         packet = new GameEventPacket();
         break;
      case Packet::GAMESTATS:
         packet = new GameStatsPacket();
         break;
      case Packet::JOINGAMEREQUEST:
         packet = new JoinGameRequestPacket();
         break;
      case Packet::JOINGAMERESPONSE:
         packet = new JoinGameResponsePacket();
         break;
      case Packet::KEY:
         packet = new KeyPacket();
         break;
      case Packet::LEAVEGAMEREQUEST:
         packet = new LeaveGameRequestPacket();
         break;
      case Packet::LEAVEGAMERESPONSE:
         packet = new LeaveGameResponsePacket();
         break;
      case Packet::LISTGAMESREQUEST:
         packet = new ListGamesRequestPacket();
         break;
      case Packet::LISTGAMESRESPONSE:
         packet = new ListGamesResponsePacket();
         break;
      case Packet::LOGINREQUEST:
         packet = new LoginRequestPacket();
         break;
      case Packet::LOGINRESPONSE:
         packet = new LoginResponsePacket();
         break;
      case Packet::MAPCREATEREQUEST:
         packet = new MapCreateRequestPacket();
         break;
      case Packet::MAPITEMCREATED:
         packet = new MapItemCreatedPacket();
         break;
      case Packet::MAPITEMREMOVED:
         packet = new MapItemRemovedPacket();
         break;
      case Packet::MAPITEMDESTROYED:
         packet = new MapItemDestroyedPacket();
         break;
      case Packet::MESSAGE:
         packet = new MessagePacket();
         break;
      case Packet::MAPITEMMOVE:
         packet = new MapItemMovePacket();
         break;
      case Packet::PLAYERKILLED:
         packet = new PlayerKilledPacket();
         break;
      case Packet::PLAYERMODIFIED:
         packet = new PlayerModifiedPacket();
         break;
      case Packet::POSITION:
         packet = new PositionPacket();
         break;
      case Packet::STARTGAMEREQUEST:
         packet = new StartGameRequestPacket();
         break;
      case Packet::STARTGAMERESPONSE:
         packet = new StartGameResponsePacket();
         break;
      case Packet::STONEDROP:
         packet = new StoneDropPacket();
         break;
      case Packet::STOPGAMEREQUEST:
         packet = new StopGameRequestPacket();
         break;
      case Packet::STOPGAMERESPONSE:
         packet = new StopGameResponsePacket();
         break;
      case Packet::TIME:
         packet = new TimePacket();
         break;
      case Packet::PLAYERSYNCHRONIZEPACKET:
         packet = new PlayerSynchronizePacket();
         break;
      case Packet::PLAYERINFECTEDPACKET:
         packet = new PlayerInfectedPacket();
         break;
      case Packet::ERROR:
         packet = new ErrorPacket();
         break;
      default:
         break;
   }

   if (packet)
   {
      // read timestamp
      in >> packet->mTimestamp;

      // dequeue members
      packet->dequeue(in);
   }
   else
   {
      qWarning("unknown packet received");
   }

   return packet;
}

