#include "stdafx.h"
#include "chunk.h"
#include "stream.h"

Chunk::Chunk(Stream *stream)
{
  mStream= stream;
  mID= mStream->getInt();
  if (mID!=0xffff)
  {
    mName= mStream->getString();
    mSize= mStream->getInt();
  }
  else
  {
    mName=NULL;
    mSize=0;
  }
  mChunkPos= mStream->pos();
}

Chunk::~Chunk()
{
  if (mName)
     delete[] mName;
}

int Chunk::id() const
{
  return mID;
}

const char* Chunk::name() const
{
  return mName;
}

void Chunk::skip()
{
  mStream->skip(mSize - mStream->pos() + mChunkPos);
}

