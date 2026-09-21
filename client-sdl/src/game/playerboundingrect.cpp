#include "playerboundingrect.h"

PlayerBoundingRect::PlayerBoundingRect()
 : mPlayerItem(0)
{
}


void PlayerBoundingRect::setPlayerItem(PlayerItem *item)
{
   mPlayerItem = item;
}


PlayerItem *PlayerBoundingRect::getPlayerItem() const
{
   return mPlayerItem;
}
