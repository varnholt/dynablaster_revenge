#include "stdafx.h"
#include "uvchannel.h"
#include "tools/stream.h"

UVChannel::UVChannel()
: mID(-1)
{
}

UVChannel::UVChannel(int id, UV* uv, int size)
: mID(id)
{
   mUV.init(size);
   for (int i=0;i<size;i++)
      mUV.add( uv[i] );
}

UVChannel::~UVChannel()
{
}

int UVChannel::id() const
{
   return mID;
}

UV* UVChannel::data() const
{
   return mUV.data();
}

const List<UV>& UVChannel::getUV() const
{
   return mUV;
}

void UVChannel::load(Stream *stream)
{
  mID= stream->getInt();

  mUV << *stream;
}

void UVChannel::write(Stream *stream)
{
  stream->writeInt(mID);

  mUV >> *stream;
}

void UVChannel::copy(const UVChannel& other)
{
   mID= other.id();
   mUV.copy( other.getUV() );
}
