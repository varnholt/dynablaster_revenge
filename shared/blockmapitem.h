#ifndef BLOCKMAPITEM_H
#define BLOCKMAPITEM_H

// base
#include "mapitem.h"

class BlockMapItem : public MapItem
{
   public:

      //! constructor
      BlockMapItem(
         int id,
         int x,
         int y
      );
};

#endif // BLOCKMAPITEM_H
