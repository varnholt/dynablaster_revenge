#ifndef EXTRAMAPITEMCREATEDPACKET_H
#define EXTRAMAPITEMCREATEDPACKET_H

// base
#include "mapitemcreatedpacket.h"

// forward declarations
class ExtraMapItem;

class ExtraMapItemCreatedPacket : public MapItemCreatedPacket
{

   public:

      //! write constructor
      ExtraMapItemCreatedPacket(ExtraMapItem*);

      //! read constructor
      ExtraMapItemCreatedPacket();

      //! enqueues the member variables to datastream
      virtual void enqueue(QDataStream&);

      //! dequeues the member variables from datastream
      virtual void dequeue(QDataStream&);

      //! getter for extra type
      qint32 getExtraType() const;

      //! setter for different skull sides
      void setSkullFaces(const QList<Constants::SkullType>& faces);

      //! getter for skull sides
      QList<Constants::SkullType> getSkullFaces() const;


   private:

      //! extra type
      qint32 mExtraType;

      //! skull sides
      QList<Constants::SkullType> mSkullFaces;
};

#endif // EXTRAMAPITEMCREATEDPACKET_H


