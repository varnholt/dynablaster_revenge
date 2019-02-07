// stream class
// performs cached file access
// "stream" is the central management object for getting data from the disk

// todo:
// - handle eof
// - get data from compressed archive

#pragma once

#include "stream.h"
#include "string.h"
#include <stdio.h>

class FileStream : public Stream
{
public:
  FileStream();
  ~FileStream();

  static void addPath(const String& path);
  static void removePath(const String& path);

  int   open(const String& name, bool write= false);        //! open file "name"
  void  close();                       //! close stream
  
  const String& getPath() const;

  // implementation of actual reading and writing functions
  void  getData(void *dst, int size);  //! get "size" bytes and store in "dst"
  void  writeData(void *dst, int size);  //! write "size" bytes

  void  skip(int size);                //! skip "size" bytes
  int   size() const;                  //! total size of the input file
  int   pos() const;                   //! current position in the input file

private:
  int   refill();                      //! refill buffer

// members
  FILE*          mFile;             //! source file
  unsigned char* mCacheBuf;         //! input cache
  int            mCachePos;         //! current position in cache
  int            mCacheLeft;        //! data left in cache
  int            mGlobalPos;        //! absolute position in file (including cache position)
  int            mSize;             //! size of the file
  String         mPath;
};
