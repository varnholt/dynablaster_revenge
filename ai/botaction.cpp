// header
#include "botaction.h"

//-----------------------------------------------------------------------------
/*!
*/
BotAction::BotAction()
   : mActionType(ActionIdle)
{
}


//-----------------------------------------------------------------------------
/*!
*/
BotAction::~BotAction()
{
}


//-----------------------------------------------------------------------------
/*!
   \return action type
*/
BotAction::ActionType BotAction::getActionType() const
{
   return mActionType;
}
