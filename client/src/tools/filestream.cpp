#include "filestream.h"
#include "string.h"
#include "array.h"
#include <memory.h>
#include <string.h>

#ifndef __APPLE__
#include <malloc.h>
#else
#include <stdlib.h>
#endif

#define STREAMCACHESIZE (4096)

static Array<String> pathList;

//! construct empty stream
FileStream::FileStream()
: Stream()
, mFile(0)
, mCacheBuf(0)
, mCachePos(0)
, mCacheLeft(0)
, mGlobalPos(0)
, mSize(0)
{
   // allocate buffer for cache
   mCacheBuf= (unsigned char*)malloc(STREAMCACHESIZE);

   if (pathList.size()==0)
   {
      addPath(".");
   }
}


//! destroy stream
FileStream::~FileStream()
{
   close();
   if (mCacheBuf)
   {
      free(mCacheBuf);
      mCacheBuf= 0;
   }

}

const String& FileStream::getPath() const
{
   return mPath;
}

void FileStream::addPath(const String& path)
{
   pathList.add( path );
}

void FileStream::removePath(const String& path)
{
   for (int i=0;i<pathList.size();)
   {
      if (pathList[i] == path)
         pathList.erase(i);
      else
         i++;
   }
}

void FileStream::close()
{
   if (mFile)
   {
      fclose(mFile);
      mFile= 0;
   }
   
   mCachePos= 0;
   mCacheLeft= 0;
   mGlobalPos= 0;
   mSize= 0;
}


//! open file
int FileStream::open(const String& name, bool write)
{
//   printf("open: %s \n", name);

   mFile= 0;

   if (write)
   {
      mFile= fopen(name, "wb");
      return mFile != 0;
   }

   // alle pfade probieren (letzter zuerst)
   for (int i=pathList.size()-1; (i>=0 && !mFile); i--)
   {
      mPath= pathList[i]+"/"+name;
      mFile= fopen(mPath, "rb");
   }

   if (!mFile)
   {
      mPath= name;
      mFile= fopen(mPath, "rb");
      if (!mFile)
         return 0;
   }

   fseek(mFile, 0, SEEK_END);
   mSize= ftell(mFile);
   fseek(mFile, 0, SEEK_SET);

   mCachePos=0;
   mCacheLeft=0;
   mGlobalPos=0;
   refill();

   return 1;
}


//! get file size
int FileStream::size() const
{
   return mSize;
}


//! get current position in file
int FileStream::pos() const
{
  return mGlobalPos + mCachePos;
}



//! read buffer
void FileStream::getData(void *buf, int size)
{
   unsigned char *dst= (unsigned char*)buf;
   while (size>0)  
   {
      int len= size;
      if (len>mCacheLeft) len=mCacheLeft;
      memcpy(dst,mCacheBuf+mCachePos,len);
      dst+=len;
      mCachePos+=len;
      mCacheLeft-=len;
      if (mCacheLeft<=0) refill();
      size-=len;
   }
}

//! read buffer
void FileStream::writeData(void *buf, int size)
{
   fwrite(buf, 1, size, mFile);
}



//! skip number of bytes
void FileStream::skip(int size)
{
  while (size>0)
  {
    int len= mCacheLeft;
    if (len>size) len=size;
    mCachePos+=len;
    mCacheLeft-=len;
    size-=len;
    if (mCacheLeft<=0) refill();
  }
}


//! refill cache from file
int FileStream::refill()
{
  mGlobalPos+=mCachePos;

  if (mCacheLeft)
  {
    // copy remaining data to front
    memcpy(mCacheBuf, mCacheBuf+mCachePos, mCacheLeft);
    // fill up the remaining space
    mCacheLeft+=(int)fread(mCacheBuf+mCacheLeft,1,STREAMCACHESIZE-mCacheLeft,mFile);
  }
  else
  {
    // fill completely
    mCacheLeft= (int)fread(mCacheBuf,1,STREAMCACHESIZE,mFile);
  }

  mCachePos=0;

  return mCacheLeft;
}
