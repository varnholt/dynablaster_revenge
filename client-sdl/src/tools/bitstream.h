#pragma once

class BitStream
{
public:
   BitStream();
   BitStream(const void* data, int size);

   unsigned int getBits(int count);
   void         skipBits(int count);
   void         cache();

private:
   unsigned char* mData;
   int            mSize;
   int            mPos;
   unsigned char  mBits;
   int            mAvail;
};

