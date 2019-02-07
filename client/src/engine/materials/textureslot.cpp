#include "textureslot.h"
#include "tools/stream.h"

TextureSlot::TextureSlot()
: mAmount(0.0f)
, mChannel(0)
{
}

TextureSlot::TextureSlot(Stream *stream)
{
   load(stream);
}

void TextureSlot::load(Stream* stream)
{
   mAmount= 1.0f;//stream->getFloat();
   mChannel= 1;//stream->getInt();
   ObjectName::load(stream);
}

void TextureSlot::write(Stream* stream)
{
//   stream->writeFloat(mAmount);
//   stream->writeInt(mChannel);
   ObjectName::write(stream);
}

float TextureSlot::amount() const
{
   return mAmount;
}

int TextureSlot::channel() const
{
   return mChannel;
}

