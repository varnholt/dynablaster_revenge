#include "botwalkaction.h"

//-----------------------------------------------------------------------------
/*!
*/
BotWalkAction::BotWalkAction()
   : mWalkKeys(0)
{
   mActionType = ActionWalk;
}


//-----------------------------------------------------------------------------
/*!
   \param direction walk direction
*/
void BotWalkAction::setWalkKeys(int8_t direction)
{
   mWalkKeys = direction;
}


//-----------------------------------------------------------------------------
/*!
   \return walk direction
*/
int8_t BotWalkAction::getWalkKeys() const
{
   return mWalkKeys;
}
