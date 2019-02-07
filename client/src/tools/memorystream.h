// stream class
// streams from memory resource

#pragma once

#include "stream.h"
#include <stdio.h>

class MemoryStream : public Stream
{
public:
   MemoryStream();

   int   open(void* buffer, int size);
   void  close();

   // implementation of actual reading and writing functions
   void  getData(void *dst, int size);  //! get "size" bytes and store in "dst"
   void  writeData(void *dst, int size);  //! write "size" bytes

   void  skip(int size);                //! skip "size" bytes
   int   size() const;                  //! total size of the input file
   int   pos() const;                   //! current position in the input file

private:
   unsigned char* mBuffer;         //! input data
   int            mSize;             //! size of the file
};
