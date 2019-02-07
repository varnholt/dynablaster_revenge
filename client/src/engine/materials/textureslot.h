#pragma once

#include "tools/string.h"
#include "tools/objectname.h"

class Stream;

class TextureSlot : public ObjectName
{
public:
   TextureSlot();
   TextureSlot(Stream *stream);

   void load(Stream* stream);
   void write(Stream* stream);

   float amount() const;
   int channel() const;

private:
   float mAmount;
   int   mChannel;
};
