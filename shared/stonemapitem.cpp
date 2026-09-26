
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
     mExtraMapItem(nullptr)
{
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
StoneMapItem::~StoneMapItem() = default;


//-----------------------------------------------------------------------------
/*!
   \param item extra map item, ownership transfers to this stone
*/
void StoneMapItem::setExtraMapItem(std::unique_ptr<ExtraMapItem> item)
{
   mExtraMapItem = std::move(item);
}


//-----------------------------------------------------------------------------
/*!
   \return extra map item
*/
ExtraMapItem* StoneMapItem::getExtraMapItem() const
{
   return mExtraMapItem.get();
}


//-----------------------------------------------------------------------------
/*!
   \return extra map item, ownership transfers to the caller
*/
std::unique_ptr<ExtraMapItem> StoneMapItem::releaseExtraMapItem()
{
   return std::move(mExtraMapItem);
}

