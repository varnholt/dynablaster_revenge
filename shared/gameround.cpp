#include "gameround.h"

GameRound::GameRound()
  : mCurrent(0),
    mCount(0)
{
}


void GameRound::reset()
{
   // setCount(0);
   setCurrent(0);
}


void GameRound::next()
{
   setCurrent(getCurrent() + 1);
}


bool GameRound::isFinished() const
{
   return (getCurrent() >= getCount());
}


int GameRound::getCurrent() const
{
   return mCurrent;
}


int GameRound::getCount() const
{
   return mCount;
}


void GameRound::setCurrent(int current)
{
   mCurrent = current;
}


void GameRound::setCount(int count)
{
   mCount = count;
}
