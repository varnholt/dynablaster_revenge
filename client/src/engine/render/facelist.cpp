#include "stdafx.h"
#include "facelist.h"
#include "tools/stream.h"

void FaceList::load(Stream *stream)
{
  mCount= mSize= stream->getInt();
  mData= new unsigned short[mSize];

  for (int i=0;i<mSize;i++)
  {
    mData[i]= stream->getWord();
  }
}

void FaceList::write(Stream *stream)
{
  stream->writeInt(mCount);

  for (int i=0;i<mSize;i++)
  {
     stream->writeWord( mData[i] );
  }
}

