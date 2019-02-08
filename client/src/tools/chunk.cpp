#include "stdafx.h"
#include "chunk.h"
#include <string.h>
#include <stdio.h>

#define BUFFERSIZE 3111

Chunk::Chunk(Stream *stream)
: Stream()
, mStream( stream )
, mMode( chunkRead )
, mID( 0 )
, mSize( 0 )
, mChunkPos( 0 )
, mBuffer(0)
, mBufferPos(0)
{
  mID= mStream->getInt();
  if (mID!=0xffff)
  {
    ObjectName::load(mStream);
    mSize= mStream->getInt();
    if (mSize<0)
       printf("break!\n");
  }
  else
  {
    mSize=0;
  }

  mChunkPos= mStream->pos();
}

Chunk::Chunk(Stream *stream, int id, const String& name)
: Stream()
, ObjectName(name)
, mStream( stream )
, mMode( chunkWrite )
, mID( id )
, mSize( 0 )
, mChunkPos( 0 )
, mBuffer( 0 )
, mBufferPos( 0 )
{
}

Chunk::~Chunk()
{
   if (mMode == chunkWrite)
   {
      mStream->writeInt( mID );
      ObjectName::write(mStream);
      int size= 0;
      for (int i=0; i<mBuffers.size(); i++)
      {
         // char* buffer= mBuffers[i];
         if (i != mBuffers.size()-1)
            size += BUFFERSIZE;
         else
            size += mBufferPos;
      }
      if (size<0)
         printf("break!\n");
      mStream->writeInt( size );
      for (int i=0; i<mBuffers.size(); i++)
      {
         char* buffer= mBuffers[i];
         if (i != mBuffers.size()-1)
            mStream->writeData( buffer, BUFFERSIZE );
         else
            mStream->writeData( buffer, mBufferPos );
         delete[] buffer;
      }
   }
}

int Chunk::id() const
{
  return mID;
}

int Chunk::dataLeft() const
{
   return mSize - mStream->pos() + mChunkPos;
}

void Chunk::skip()
{
  mStream->skip(mSize - mStream->pos() + mChunkPos);
}

void Chunk::getData(void *src, int size)
{
   mStream->getData(src, size);
   mPosition += size;
}

void Chunk::writeData(void *data, int size)
{
   char *src= (char*)data;
   while (size > 0)
   {
      if (!mBuffer)
      {
         mBuffer= new char[ BUFFERSIZE ];
         mBufferPos= 0;
         mBuffers.add( mBuffer );
      }

      int len= size;
      int avail= BUFFERSIZE - mBufferPos;
      if (len > avail) len= avail;
      memcpy(mBuffer + mBufferPos, src, len);
      src+=len;
      mBufferPos += len;

      if (mBufferPos >= BUFFERSIZE)
      {
         mBuffer= 0;
         mBufferPos= 0;
      }

      size-= len;
   }
}

