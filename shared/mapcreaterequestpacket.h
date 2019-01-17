#ifndef MAPCREATEREQUESTPACKET_H
#define MAPCREATEREQUESTPACKET_H

// base
#include "packet.h"

// Qt
#include <QList>
#include <QPoint>


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
         const QList<QPoint>& startPositions
      );

      //! read constructor
      MapCreateRequestPacket();

      //! destructor
      virtual ~MapCreateRequestPacket();

      //! debugs the member variables
      void debug();

      //! enqueues the member variables to datastream
      void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      void dequeue(QDataStream&);


   private:

      //! map width
      qint32 mWidth;

      //! map height
      qint32 mHeight;

      //! number of stones
      qint32 mStoneCount;

      //! number of bomb extras
      qint32 mExtraBombCount;

      //! number of flame extras
      qint32 mExtraFlameCount;

      //! player start positions
      QList<QPoint> mStartPositions;
};

#endif // MAPCREATEREQUESTPACKET_H
