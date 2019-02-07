#include "weight.h"
#include "tools/stream.h"
#include "nodes/scenegraph.h"

Weight::Weight()
: mID(-1)
, mWeight(0.0f)
{
}

Weight::Weight(int id, float w)
: mID(id)
, mWeight(w)
{
}

Weight::~Weight()
{
}

void Weight::operator << (Stream& stream)
{
   load(&stream);
}

void Weight::operator >> (Stream& stream)
{
   write(&stream);
}

float Weight::weight() const
{
   return mWeight;
}

int Weight::id() const
{
   return mID;
}

void Weight::load(Stream *stream)
{
   mID=     stream->getWord();
   mWeight= stream->getFloat();
}

void Weight::write(Stream *stream)
{
   stream->writeWord( mID );
   stream->writeFloat( mWeight );
}
