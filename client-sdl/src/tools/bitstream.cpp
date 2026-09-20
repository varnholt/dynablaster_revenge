#include "bitstream.h"

BitStream::BitStream()
: mData( 0 )
, mSize( 0 )
, mPos( 0 )
, mBits( 0 )
, mAvail( 0 )
{
}

BitStream::BitStream(const void* data, int size)
: mData( (unsigned char*)data )
, mSize( size )
, mPos( 0 )
, mBits( 0 )
, mAvail( 0 )
{
}


unsigned int BitStream::getBits(int count)
{
   unsigned int value= 0;
   while (count > 0)
   {
      cache();

      if (count >= mAvail)
      {
         value= (value << mAvail) | (mBits >> (8-mAvail));
         count -= mAvail;
         mAvail= 0;
         mBits= 0;
      }
      else
      {
         value= (value << count) | (mBits >> (8-count));
         mBits <<= count;
         mAvail -= count;
         count = 0;
      }
   }

   return value;
}

void BitStream::skipBits(int count)
{
   getBits(count);
}

void BitStream::cache()
{
   if (!mAvail)
   {
      mBits= mData[mPos];
      mPos++;
      mAvail= 8;
   }
}
