#include "keybase.h"
#include "tools/stream.h"

KeyBase::KeyBase(int time)
: mTime( time )
{
}

KeyBase::~KeyBase()
{
}

int KeyBase::time() const
{
   return mTime;
}

void KeyBase::load(Stream *stream)
{
   mTime= stream->getInt();
}

void KeyBase::write(Stream *stream)
{
   stream->writeInt( mTime );
}
