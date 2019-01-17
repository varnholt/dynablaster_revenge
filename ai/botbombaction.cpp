#include "botbombaction.h"

//-----------------------------------------------------------------------------
/*!
*/
BotBombAction::BotBombAction()
{
   mActionType = ActionBomb;
}


bool BotBombAction::isCombinable() const
{
   return false;
}


