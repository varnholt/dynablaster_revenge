// header
#include "mapcreaterequestpacket.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "MapCreateRequest"

//-----------------------------------------------------------------------------
/*!
   write constructor

   \param width map's width
   \param height map's height
   \param stoneCount number of stones to place
   \param extraBombCount number of bomb extras to place
   \param extraFlameCount number of flame extras to place
*/
MapCreateRequestPacket::MapCreateRequestPacket(
   int width,
   int height,
   int stoneCount,
   int extraBombCount,
   int extraFlameCount,
   const QList<QPoint>& startPositions
)
 : Packet(Packet::MAPCREATEREQUEST),
   mWidth(width),
   mHeight(height),
   mStoneCount(stoneCount),
   mExtraBombCount(extraBombCount),
   mExtraFlameCount(extraFlameCount),
   mStartPositions(startPositions)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
MapCreateRequestPacket::MapCreateRequestPacket()
   : Packet(Packet::MAPCREATEREQUEST)
{
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
MapCreateRequestPacket::~MapCreateRequestPacket()
{
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void MapCreateRequestPacket::enqueue(QDataStream & out)
{
   out << mWidth;
   out << mHeight;
   out << mStoneCount;
   out << mExtraBombCount;
   out << mExtraFlameCount;
   out << mStartPositions;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void MapCreateRequestPacket::dequeue(QDataStream & in)
{
   in
      >> mWidth
      >> mHeight
      >> mStoneCount
      >> mExtraBombCount
      >> mExtraFlameCount
      >> mStartPositions;
}


//-----------------------------------------------------------------------------
/*!
   debug output of members
*/
void MapCreateRequestPacket::debug()
{
   qDebug(
      "MapCreateRequestPacket:debug: width: %d, height: %d, stones: %d, "
      "bombextras: %d, flamextras: %d",
      mWidth,
      mHeight,
      mStoneCount,
      mExtraBombCount,
      mExtraFlameCount
   );
}


