#include "bmp.h"
#include "tools/filestream.h"

#pragma pack(push, 1)

struct BMPInfoHeader
{
   BMPInfoHeader(int x, int y)
      : mSize( sizeof(BMPInfoHeader) )
      , mWidth( x )
      , mHeight( y )
      , mPlanes( 1 )
      , mBitCount( 24 )
      , mCompression( 0 )
      , mImageSize( x*y*3 )
      , mXPelsPerMeter( 0 )
      , mYPelsPerMeter( 0 )
      , mClrUsed( 0 )
      , mClrImportant( 0 )
   {
   }

   unsigned int   mSize;
   int            mWidth;
   int            mHeight;
   unsigned short mPlanes;
   unsigned short mBitCount;
   unsigned int   mCompression;
   unsigned int   mImageSize;
   int            mXPelsPerMeter;
   int            mYPelsPerMeter;
   unsigned int   mClrUsed;
   unsigned int   mClrImportant;
};

struct BMPFileHeader
{
   BMPFileHeader()
      : mSize(0)
      , mReserved(0)
      , mOffset(0)
   {
      mHead[0]= 'B';
      mHead[1]= 'M';
      mOffset= sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
   }

   char           mHead[2];
   unsigned int   mSize;
   unsigned int   mReserved;
   unsigned int   mOffset;
};

#pragma pack(pop)

int saveBmp(char *filename, unsigned int *data, int width, int height)
{
   FileStream stream;
   stream.open( filename, true );

   BMPFileHeader head;
   stream.writeData(&head, sizeof(BMPFileHeader));

   BMPInfoHeader info(width, height);
   stream.writeData(&info, sizeof(BMPInfoHeader));

   int padding= (4-width*3) & 3;

   for (int y=0; y<height; y++)
   {
      unsigned int *src= data + y*width;
      for (int x=0; x<width; x++)
      {
         unsigned int col= src[x];

         unsigned char r= col >> 16 & 255;
         unsigned char g= col >> 8 & 255;
         unsigned char b= col & 255;

         stream.writeByte( b );
         stream.writeByte( g );
         stream.writeByte( r );
      }

      for (int p=0; p<padding; p++)
         stream.writeByte( 0 );
   }

   stream.close();

   return 1;
}
