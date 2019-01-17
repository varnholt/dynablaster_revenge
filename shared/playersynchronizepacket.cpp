// header
#include "playersynchronizepacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "PlayerSynchronize"


//-----------------------------------------------------------------------------
/*!
   \param process process to sync
*/
PlayerSynchronizePacket::PlayerSynchronizePacket()
 : Packet(Packet::PLAYERSYNCHRONIZEPACKET),
   mSynchronizeProcess(Invalid)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \param process process to sync
*/
PlayerSynchronizePacket::PlayerSynchronizePacket(
    PlayerSynchronizePacket::SynchronizeProcess process
)
 : Packet(Packet::PLAYERSYNCHRONIZEPACKET),
   mSynchronizeProcess(process)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
*/
PlayerSynchronizePacket::~PlayerSynchronizePacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \param process synchronize process
*/
void PlayerSynchronizePacket::setSynchronizeProcess(
   PlayerSynchronizePacket::SynchronizeProcess process
)
{
   mSynchronizeProcess = process;
}


//-----------------------------------------------------------------------------
/*!
   \return synchronize process
*/
PlayerSynchronizePacket::SynchronizeProcess PlayerSynchronizePacket::getSynchronizeProcess() const
{
   return mSynchronizeProcess;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void PlayerSynchronizePacket::enqueue(QDataStream & out)
{
   out << (quint8)getSynchronizeProcess();
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void PlayerSynchronizePacket::dequeue(QDataStream & in)
{
   quint8 process = 0;

   in >> process;

   setSynchronizeProcess((SynchronizeProcess)process);
}


//-----------------------------------------------------------------------------
/*!
*/
void PlayerSynchronizePacket::debug()
{
   qDebug(
      "PlayerSynchronizePacket::debug(): process: %d",
      getSynchronizeProcess()
   );
}


