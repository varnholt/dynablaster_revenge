#include "blockmapitem.h"

BlockMapItem::BlockMapItem(
   int id,
   int x,
   int y
)
   : MapItem(Block, id, true, false, x, y)
{
}
