#include "memorystream.h"
#include "array.h"
#include <memory.h>
#include <malloc.h>
#include <string.h>

//! construct empty stream
MemoryStream::MemoryStream()
   : Stream()
   , mBuffer(0)
   , mSize(0)
{
}

//! close stream
void MemoryStream::close()
{
   mBuffer= 0;
   mPosition= 0;
   mSize= 0;
}


//! open file
int MemoryStream::open(void* data, int size)
{
   mBuffer= (unsigned char*)data;
   mSize= size;
   mPosition= 0;

   return 1;
}


//! get file size
int MemoryStream::size() const
{
   return mSize;
}


//! get current position in file
int MemoryStream::pos() const
{
   return mPosition;
}


//! read buffer
void MemoryStream::getData(void *buf, int size)
{
   unsigned char *dst= (unsigned char*)buf;
   int avail= mSize - mPosition;
   if (size > avail)
      size= avail;
   memcpy(buf, mBuffer + mPosition, size);
   mPosition += size;
}

//! write buffer
void MemoryStream::writeData(void *buf, int size)
{
   memcpy(mBuffer + mPosition, buf, size);
   mPosition += size;
}


//! skip number of bytes
void MemoryStream::skip(int size)
{
   mPosition += size;
}

