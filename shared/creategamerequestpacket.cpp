//
// C++ Implementation: CreateGameRequestPacket
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
#include "creategamerequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "CreateGameReqest"


//-----------------------------------------------------------------------------
/*!
   write constructor

   \param name game's name
*/
CreateGameRequestPacket::CreateGameRequestPacket(
   const QString& name,
   const QString& level,
   int rounds,
   int duration,
   int maxPlayers,
   bool extraBombEnabled,
   bool extraFlameEnabled,
   bool extraSpeedupEnabled,
   bool extraKickEnabled,
   bool extraSkullsEnabled,
   Constants::Dimension dimension
)
 : Packet(Packet::CREATEGAMEREQUEST)
{
   mPacketName = PACKETNAME;

   mData.mName = name;
   mData.mLevel = level;
   mData.mRounds = rounds;
   mData.mDuration = duration;
   mData.mMaxPlayers = maxPlayers;
   mData.mExtraBombEnabled = extraBombEnabled;
   mData.mExtraFlameEnabled = extraFlameEnabled;
   mData.mExtraSpeedupEnabled = extraSpeedupEnabled;
   mData.mExtraKickEnabled = extraKickEnabled;
   mData.mExtraSkullsEnabled = extraSkullsEnabled;
   mData.mDimension = dimension;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
CreateGameRequestPacket::CreateGameRequestPacket()
   : Packet(Packet::CREATEGAMEREQUEST)
{
   mPacketName = PACKETNAME;

   mData.mRounds = 1;
   mData.mDuration = 180;
   mData.mMaxPlayers = 5;
   mData.mExtraBombEnabled = false;
   mData.mExtraFlameEnabled = false;
   mData.mExtraSpeedupEnabled = false;
   mData.mExtraKickEnabled = false;
   mData.mExtraSkullsEnabled = false;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
CreateGameRequestPacket::~CreateGameRequestPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \return game's name
*/
const QString CreateGameRequestPacket::getName()
{
   return mData.mName;
}


//-----------------------------------------------------------------------------
/*!
   \return game's crate data
*/
CreateGameData CreateGameRequestPacket::getData() const
{
   return mData;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void CreateGameRequestPacket::enqueue(QDataStream & out)
{
   // write members
   out
      << mData.mName
      << mData.mLevel

      << mData.mRounds
      << mData.mDuration
      << mData.mMaxPlayers

      << mData.mExtraBombEnabled
      << mData.mExtraFlameEnabled
      << mData.mExtraSpeedupEnabled
      << mData.mExtraKickEnabled
      << mData.mExtraSkullsEnabled

      << mData.mDimension;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void CreateGameRequestPacket::dequeue(QDataStream & in)
{
   int dimension = 0;

   // read members
   in
      >> mData.mName
      >> mData.mLevel

      >> mData.mRounds
      >> mData.mDuration
      >> mData.mMaxPlayers

      >> mData.mExtraBombEnabled
      >> mData.mExtraFlameEnabled
      >> mData.mExtraSpeedupEnabled
      >> mData.mExtraKickEnabled
      >> mData.mExtraSkullsEnabled

      >> dimension;

   mData.mDimension = (Constants::Dimension)dimension;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void CreateGameRequestPacket::debug()
{
   // debug output login request
   qDebug(
      "CreateGameRequestPacket:debug:\n"
      "- name: %s\n"
      "- level: %s\n"
      "- rounds: %d\n"
      "- duration: %d\n"
      "- max players: %d\n"
      "- extra bomb enabled: %d\n"
      "- extra flame enabled: %d\n"
      "- extra speedup enabled: %d\n"
      "- extra kick enabled: %d"
      "- extra skulls enabled: %d"
      "- dimension: %d",
      qPrintable(mData.mName),
      qPrintable(mData.mLevel),
      mData.mRounds,
      mData.mDuration,
      mData.mMaxPlayers,
      mData.mExtraBombEnabled,
      mData.mExtraFlameEnabled,
      mData.mExtraSpeedupEnabled,
      mData.mExtraKickEnabled,
      mData.mExtraSkullsEnabled,
      mData.mDimension
   );
}


