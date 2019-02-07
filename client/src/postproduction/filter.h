// base class for post production filters

#pragma once

#include "tools/string.h"

class Filter
{
public:
   Filter(const String& name);
   virtual ~Filter();

   virtual bool init() = 0;
   virtual void process(unsigned int texture, float u, float v) = 0;

   const String& getName() const;

private:
   String mName;
};
