#include "stdafx.h"
#include "uv.h"
#include "tools/stream.h"

UV::UV()
: u(0.0f)
, v(0.0f)
{
}

UV::UV(float su, float sv)
: u(su)
, v(sv)
{
}

void UV::operator << (Stream& stream)
{
   load(&stream);
}

void UV::operator >> (Stream& stream)
{
   write(&stream);
}

void UV::load(Stream *stream)
{
  u= stream->getFloat();
  v= 1.0f - stream->getFloat();
  stream->getFloat();
}

void UV::write(Stream *stream)
{
  stream->writeFloat(u);
  stream->writeFloat(1.0f - v);
  stream->writeFloat(0.0f);
}

