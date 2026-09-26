#ifndef MAPCREATEREQUESTPACKET_H
#define MAPCREATEREQUESTPACKET_H

// base
#include "packet.h"

// Qt
#include <QList>

#include "point.h"

class MapCreateRequestPacket : public Packet
{
public:
   //! write constructor
   MapCreateRequestPacket(
      int width,
      int height,
      int stoneCount,
      int extraBombCount,
      int extraFlameCount,
      const QList<Point>& startPositions
   );

   //! read constructor
   MapCreateRequestPacket();

   //! destructor
   virtual ~MapCreateRequestPacket();

   //! debugs the member variables
   void debug();

   //! enqueues the member variables to datastream
   void enqueue(BinaryWriter&);

   //! dequeues the member variables from datastream
   void dequeue(BinaryReader&);

private:
   //! map width
   int32_t mWidth;

   //! map height
   int32_t mHeight;

   //! number of stones
   int32_t mStoneCount;

   //! number of bomb extras
   int32_t mExtraBombCount;

   //! number of flame extras
   int32_t mExtraFlameCount;

   //! player start positions
   QList<Point> mStartPositions;
};

#endif  // MAPCREATEREQUESTPACKET_H
