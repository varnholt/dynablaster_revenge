#include "botoption.h"

//-----------------------------------------------------------------------------
/*!
*/
BotOption::BotOption()
   : mAction(0),
     mScore(0),
     mCombinable(false)
{
}


//-----------------------------------------------------------------------------
/*!
*/
BotOption::~BotOption()
{
   delete mAction;
   mAction = 0;
}


//-----------------------------------------------------------------------------
/*!
   \param score score to set
*/
void BotOption::setScore(int score)
{
   mScore = score;
}


//-----------------------------------------------------------------------------
/*!
   \return score
*/
int BotOption::getScore() const
{
   return mScore;
}


//-----------------------------------------------------------------------------
/*!
   \param action bot action
*/
void BotOption::setAction(BotAction* action)
{
   mAction = action;
}


//-----------------------------------------------------------------------------
/*!
   \return action
*/
BotAction* BotOption::getAction() const
{
   return mAction;
}


//-----------------------------------------------------------------------------
/*!
   \param combinable combinable flag
*/
void BotOption::setCombinable(bool combinable)
{
   mCombinable = combinable;
}


//-----------------------------------------------------------------------------
/*!
   \return combinable state
*/
bool BotOption::isCombinable() const
{
   return mCombinable;
}



