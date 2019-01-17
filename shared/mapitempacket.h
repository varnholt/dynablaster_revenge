#ifndef MAPITEMPACKET_H
#define MAPITEMPACKET_H

#include "packet.h"
#include "mapitem.h"

class MapItemPacket : public Packet
{
   public:

      //! constructor
      MapItemPacket(Packet::TYPE type);

      //! alternative constructor
      MapItemPacket(Packet::TYPE packetType, MapItem *item);

      //! debug packet
      void debug();

      //! enqueue packet
      virtual void enqueue(QDataStream&);

      //! dequeue packet
      virtual void dequeue(QDataStream&);

      //! getter for item's x pos
      int getX() const;

      //! getter for item's y pos
      int getY() const;

      //! getter for item type
      MapItem::ItemType getItemType() const;

      //! getter for item's unique id
      int getUniqueId() const;

   private:

      int mX;
      int mY;
      int mUniqueId;
      MapItem::ItemType mItemType;
};

#endif // MAPITEMREMOVEDPACKET_H
