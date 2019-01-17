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
void BotWalkAction::setWalkKeys(qint8 direction)
{
   mWalkKeys = direction;
}


//-----------------------------------------------------------------------------
/*!
   \return walk direction
*/
qint8 BotWalkAction::getWalkKeys() const
{
   return mWalkKeys;
}
