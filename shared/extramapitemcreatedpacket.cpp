
// header
#include "extramapitemcreatedpacket.h"

// shared
#include "extramapitem.h"

// Qt
#include <QDataStream>

// defines
#define PACKETNAME "ExtraMapItemCreated"


//-----------------------------------------------------------------------------
/*!
   \param item extra-mapitem
*/
ExtraMapItemCreatedPacket::ExtraMapItemCreatedPacket(
   ExtraMapItem *item
)
   : MapItemCreatedPacket(item),
     mExtraType(item->getExtraType())
{
   for (int i = 0; i < 6; i++)
      mSkullFaces.push_back(Constants::SkullReset);

   mPacketType = Packet::EXTRAMAPITEMCREATED;
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   read constructor
*/
ExtraMapItemCreatedPacket::ExtraMapItemCreatedPacket()
   : MapItemCreatedPacket(),
     mExtraType(-1)
{
   for (int i = 0; i < 6; i++)
      mSkullFaces.push_back(Constants::SkullReset);

   mPacketType = Packet::EXTRAMAPITEMCREATED;
   mPacketName = PACKETNAME;
}


//-----------------------------------------------------------------------------
/*!
   \return the extra map item's type
*/
qint32 ExtraMapItemCreatedPacket::getExtraType() const
{
   return mExtraType;
}


//-----------------------------------------------------------------------------
/*!
   \param sides skull sides
*/
void ExtraMapItemCreatedPacket::setSkullFaces(
   const QList<Constants::SkullType> &faces
)
{
   mSkullFaces = faces;
}


//-----------------------------------------------------------------------------
/*!
   \return enabled skull sides
*/
QList<Constants::SkullType> ExtraMapItemCreatedPacket::getSkullFaces() const
{
   return mSkullFaces;
}


//-----------------------------------------------------------------------------
/*!
   \param out datastream to write members to
*/
void ExtraMapItemCreatedPacket::enqueue(QDataStream& out)
{
   quint32 faces =
        (mSkullFaces[0] << 20)
      | (mSkullFaces[1] << 16)
      | (mSkullFaces[2] << 12)
      | (mSkullFaces[3] <<  8)
      | (mSkullFaces[4] <<  4)
      | (mSkullFaces[5]      );

   MapItemCreatedPacket::enqueue(out);

   // write members
   out << mExtraType;
   out << faces;
}


//-----------------------------------------------------------------------------
/*!
   \param in datastream read members from
*/
void ExtraMapItemCreatedPacket::dequeue(QDataStream& in)
{
   MapItemCreatedPacket::dequeue(in);

   // read members
   qint32 extraType = 0;
   quint32 faces = 0;

   in >> extraType;
   in >> faces;

   mExtraType = (Constants::ExtraType)extraType;
   mSkullFaces[0] = (Constants::SkullType)((faces >> 20) & 0x0f);
   mSkullFaces[1] = (Constants::SkullType)((faces >> 16) & 0x0f);
   mSkullFaces[2] = (Constants::SkullType)((faces >> 12) & 0x0f);
   mSkullFaces[3] = (Constants::SkullType)((faces >>  8) & 0x0f);
   mSkullFaces[4] = (Constants::SkullType)((faces >>  4) & 0x0f);
   mSkullFaces[5] = (Constants::SkullType)((faces      ) & 0x0f);
}

