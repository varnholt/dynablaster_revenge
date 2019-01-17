#ifndef STONEMAPITEM_H
#define STONEMAPITEM_H

// base
#include "mapitem.h"

// forward declarations
class ExtraMapItem;

class StoneMapItem : public MapItem
{
   public:

      //! constructor
      StoneMapItem(
         int id,
         int x,
         int y
      );


      //! setter for extra map item
      void setExtraMapItem(ExtraMapItem*);

      //! getter for extra map item
      ExtraMapItem* getExtraMapItem();


   private:

      //! extra map item
      ExtraMapItem* mExtraMapItem;
};

#endif // STONEMAPITEM_H
