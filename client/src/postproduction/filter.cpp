#include "filter.h"

Filter::Filter(const String& name)
: mName(name)
{
}

Filter::~Filter()
{
}

const String& Filter::getName() const
{
   return mName;
}

