#ifndef MAPITEM_H
#define MAPITEM_H

// Qt
#include <QObject>

#include "constants.h"

// forward declarations
class MapItemCreatedPacket;

class MapItem : public QObject
{
   Q_OBJECT

   public:

      //! item types
      enum ItemType
      {
         Block,
         Bomb,
         Extra,
         Stone,
         Unknown
      };

      //! construct from data
      MapItem(
         ItemType type,
         int id,
         bool blocking,
         bool destroyable,
         int posX,
         int posY
      );

      //! construct from packet
      MapItem(MapItemCreatedPacket *packet);
      
      //! destructor
      virtual ~MapItem();

      //! getter for item type
      ItemType getType() const;

      //! setter for unique id
      void setUniqueId(int);

      //! getter for unique id
      int getUniqueId() const;

      //! getter for blocking flag
      bool isBlocking() const;

      //! getter for destroyable flag
      bool isDestroyable() const;

      //! setter for x position
      void setX(int x);

      //! setter for y position
      void setY(int y);

      //! getter for x position
      int getX() const;

      //! getter for y position
      int getY() const;

      //! getter for appearance
      int getAppearance() const;

      //! setter for "being destroyed" flag
      void setCurrentlyDestroyed(bool destroyed);

      //! getter for "being destroyed" flag
      bool isCurrentlyDestroyed() const;

      //! get direction from which the item was destroyed
      Constants::Direction getDestroyDirection() const;

      //! set direction from which the item was destroyed
      void setDestroyDirection(Constants::Direction direction);


   protected:

      //! initialize blocking flag
      void initializeBlocking();

      //! item type
      ItemType mType;

      //! unique number to identify the item
      int mUniqueId;

      //! item id (mapping between mesh/texture and id)
      int mAppearance;

      //! item is blocking
      bool mBlocking;

      //! item is destroyable
      bool mDestroyable;

      //! item's x position on the map
      int mX;

      //! item's y position on the map
      int mY;

      //! continuous counter for unique id
      static int mCurrentId;

      //! mapitem is currently being destroyed (avoid "double-destruction")
      bool mCurrentlyDestroyed;

      //! direction from which the item was destroyed
      Constants::Direction mDestroyDirection;
};

#endif
