#include "globaltime.h"

GlobalTime* GlobalTime::mInstance = 0;

GlobalTime::GlobalTime()
{
    mInstance= this;
}

GlobalTime::~GlobalTime()
{
    mInstance= 0;
}

GlobalTime* GlobalTime::Instance()
{
   return mInstance;
}

