
// header
#include "stonemapitem.h"

// shared
#include "extramapitem.h"


//-----------------------------------------------------------------------------
/*!
  constructor
*/
StoneMapItem::StoneMapItem(
   int id,
   int x,
   int y
)
   : MapItem(Stone, id, true, true, x, y),
     mExtraMapItem(0)
{
}


//-----------------------------------------------------------------------------
/*!
   \param item extra map item
*/
void StoneMapItem::setExtraMapItem(ExtraMapItem* item)
{
   mExtraMapItem = item;
}


//-----------------------------------------------------------------------------
/*!
   \return extra map item
*/
ExtraMapItem* StoneMapItem::getExtraMapItem()
{
   return mExtraMapItem;
}

