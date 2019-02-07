#include "image.h"
#include "tga.h"
#include "image/imagepool.h"
#include "tools/filestream.h"
#include <memory.h>
#include <algorithm>
#include <math.h>

// construct null image
Image::Image()
: Referenced()
, mData(0)
, mWidth(0)
, mHeight(0)
{
}

// construct empty image x*y
Image::Image(int x, int y)
: Referenced()
, mData(0)
, mWidth(x)
, mHeight(y)
{
   mData= new unsigned int[mWidth*mHeight];
   memset(mData, 0, x*y*4);
}

/*
Image::Image(unsigned int *data, int x, int y)
: Referenced()
, mData(data)
, mWidth(x)
, mHeight(y)
{
}
*/

// construct image from file
Image::Image(const char *filename)
: Referenced()
, mData(0)
, mWidth(0)
, mHeight(0)
{
   load(filename);
}

// construct references
Image::Image(const Image& image)
: Referenced(image)
, mData(image.getData())
, mWidth(image.getWidth())
, mHeight(image.getHeight())
{
}

// destructor
Image::~Image()
{
   // if this is the last instance to the referenced data: delte it
   if (getRefCount()==1)
       discard();
}

// assignment operator: create reference
const Image& Image::operator = (const Image& image)
{
   if (this != &image)
   {
       // if this is the last instance to the referenced data: delte it
       if (getRefCount() == 1)
       {
          discard();
          delete mReferences;
       }

       mReferences= image.getRef();
       addRef();
       mWidth= image.getWidth();
       mHeight= image.getHeight();
       mData= image.getData();
   }

   return *this;
}

void Image::discard()
{
   if (mData)
      delete[] mData;
   ImagePool::Instance()->remove(this);
}

// save image to file
void Image::save(const char* filename)
{
   savetga(filename, getData(), getWidth(), getHeight());
}

// load image from file
void Image::load(const char *filename)
{
   // data is not referenced by another object? delete it.
   if (!copyRef())
      discard();


   if (loadtga(filename, (void**)&mData, &mWidth, &mHeight))
   {
      char name[256];
      strcpy(name, filename);
      strcat(name, ".tga");
      FileStream stream;
      if (stream.open(name))
      {
         mPath= stream.getPath();
         mFilename= filename;
         stream.close();
      }
   }
}

// get image width
int Image::getWidth() const
{
   return mWidth;
}

// get image height
int Image::getHeight() const
{
   return mHeight;
}

// get scanline
unsigned int* Image::getScanline(int y) const
{
   return mData + y*mWidth;
}

// get data
unsigned int* Image::getData() const
{
   return mData;
}

const String& Image::path() const
{
   return mPath;
}

const String& Image::filename() const
{
   return mFilename;
}

unsigned int Image::getPixel(float u, float v) const
{
   int x= (int) floor(u*(mWidth-1));
   int y= (int) floor(v*(mHeight-1));
   return mData[y*mWidth+x];
}

// halve resolution
Image Image::downsample() const
{
   int nx= mWidth>>1;
   int ny= mHeight>>1;

   Image image(nx, ny);

   for (int y=0;y<ny;y++)
   {
      unsigned int *dst= image.getScanline(y);

      unsigned int *src1= getScanline(y*2);
      unsigned int *src2= getScanline(y*2+1);

      for (int x=0;x<nx;x++)
      {
         unsigned int c1= *src1++;
         unsigned int c2= *src1++;
         unsigned int c3= *src2++;
         unsigned int c4= *src2++;

         int a= ((c1>>24&0xff) + (c2>>24&0xff) + (c3>>24&0xff) + (c4>>24&0xff)) >> 2;
         int r= ((c1>>16&0xff) + (c2>>16&0xff) + (c3>>16&0xff) + (c4>>16&0xff)) >> 2;
         int g= ((c1>> 8&0xff) + (c2>> 8&0xff) + (c3>> 8&0xff) + (c4>> 8&0xff)) >> 2;
         int b= ((c1    &0xff) + (c2    &0xff) + (c3    &0xff) + (c4    &0xff)) >> 2;

         *dst++= (a<<24)+(r<<16)+(g<<8)+b;
      }
   }

   return image;
}

// linear blend between c1 & c2
unsigned int blend(unsigned int c1, unsigned int c2, unsigned char f)
{
   unsigned char a= (c1>>24&0xff) + (( (c2>>24&0xff) - (c1>>24&0xff) ) * f >> 8);
   unsigned char r= (c1>>16&0xff) + (( (c2>>16&0xff) - (c1>>16&0xff) ) * f >> 8);
   unsigned char g= (c1>>8 &0xff) + (( (c2>>8 &0xff) - (c1>>8 &0xff) ) * f >> 8);
   unsigned char b= (c1    &0xff) + (( (c2    &0xff) - (c1    &0xff) ) * f >> 8);

   return (a<<24)|(r<<16)|(g<<8)|b;
}

// create scaled version of given image
void Image::scaled(const Image& image) const
{
   int w= image.getWidth();
   int h= image.getHeight();

   int dx= (w << 16) / mWidth;
   int dy= (h << 16) / mHeight;

   int iy= 0;
   for (int dstY=0; dstY<mHeight; dstY++)
   {
      int y= iy >> 16;
      int sy= iy >> 8 & 0xff;

      unsigned int *dst= getScanline(dstY);
      unsigned int *src1, *src2;
      src1= image.getScanline(y);
      if (y==h-1)
         src2= image.getScanline(y);
      else
         src2= image.getScanline(y+1); // don't exceed image boundaries

      int ix= 0;
      for (int dstX=0; dstX<mWidth-1; dstX++)
      {
         int x= ix >> 16;
         int sx= ix >> 8 & 0xff;

         unsigned int c1= src1[x];
         unsigned int c2= src1[x+1];
         unsigned int c3= src2[x];
         unsigned int c4= src2[x+1];

         c1= blend(c1, c2, sx);
         c2= blend(c3, c4, sx);

         dst[dstX]= blend(c1, c2, sy);

         ix+=dx;
      }
      dst[mWidth-1]= blend(src1[w-1], src2[w-1], sy);

      iy+=dy;
   }
}

void Image::premultiplyAlpha()
{
   for (int y=0;y<mHeight;y++)
   {
      unsigned int *dst= getScanline(y);

      for (int x=0;x<mWidth;x++)
      {
         unsigned int c1= dst[x];

         unsigned char a= (c1>>24&0xff);
         if (a != 255)
         {
            unsigned char r= (c1>>16&0xff);
            unsigned char g= (c1>> 8&0xff);
            unsigned char b= (c1    &0xff);

            r= (r*a)>>8;
            g= (g*a)>>8;
            b= (b*a)>>8;

            dst[x]= (a<<24)+(r<<16)+(g<<8)+b;
         }
      }
   }
}

void Image::minimum(const Image& image)
{
   int width= std::min<int>(mWidth, image.getWidth());
   int height= std::min<int>(mHeight, image.getHeight());

   for (int y=0; y<height; y++)
   {
      unsigned int *dst= getScanline(y);
      unsigned int *src= image.getScanline(y);

      for (int x=0; x<width; x++)
      {
         unsigned int c1= src[x];
         unsigned int c2= dst[x];

         unsigned char a1= (c1>>24&0xff);
         unsigned char r1= (c1>>16&0xff);
         unsigned char g1= (c1>> 8&0xff);
         unsigned char b1= (c1    &0xff);

         unsigned char a2= (c2>>24&0xff);
         unsigned char r2= (c2>>16&0xff);
         unsigned char g2= (c2>> 8&0xff);
         unsigned char b2= (c2    &0xff);

         if (a2<a1) a1= a2;
         if (r2<r1) r1= r2;
         if (g2<g1) g1= g2;
         if (b2<b1) b1= b2;

         dst[x]= (a1<<24)+(r1<<16)+(g1<<8)+b1;
      }
   }
}

void Image::clear(unsigned int argb)
{
   for (int y=0; y<mHeight; y++)
   {
      unsigned int *dst= getScanline(y);
      for (int x=0; x<mWidth; x++)
         dst[x]= argb;
   }
}
void Image::copy(int posX, int posY, const Image& image, int replicate)
{
   int width= std::min<int>(mWidth - posX, image.getWidth());
   int height= std::min<int>(mHeight - posY, image.getHeight());

   int endx= mWidth - posX;
   int endy= mHeight - posY;

   for (int y=0; y<height; y++)
   {
      unsigned int *dst= getScanline(y + posY) + posX;
      unsigned int *src= image.getScanline(y);

      for (int x=0; x<width; x++)
      {
         dst[x]= src[x];
      }

      // replicate last pixel
      for (int x=0; x<endx-width && x<replicate; x++)
         dst[width+x]= src[width-1];
   }

   // replicate last scanline
   if (replicate>0)
   {
       unsigned int *src= getScanline( height-1 );
       for (int y=0; y<endy-height && y<replicate; y++)
       {
          unsigned int *dst= getScanline(y+height) + posX;
          memcpy(dst, src, endx*4);
       }
    }
}


unsigned int calcNormal(int z, unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1)
{
   // height= red + green + blue;
   x0= (x0 >> 16 & 255) + (x0 >> 8 & 255) + (x0 & 255);
   x1= (x1 >> 16 & 255) + (x1 >> 8 & 255) + (x1 & 255);
   y0= (y0 >> 16 & 255) + (y0 >> 8 & 255) + (y0 & 255);
   y1= (y1 >> 16 & 255) + (y1 >> 8 & 255) + (y1 & 255);

   int x= (x0-x1);
   int y= (y0-y1);

   int mag= x*x + y*y + z*z;
   float t= 128.0f / sqrt( (double)mag );
   x= 128 + x*t; if (x<0) x= 0; if (x>255) x=255;
   y= 128 - y*t; if (y<0) y= 0; if (y>255) y=255;
   z= 128 + z*t; if (z<0) z= 0; if (z>255) z=255;

   return (255<<24) | (x<<16) | (y<<8) | z;
}

void Image::buildNormalMap(int z)
{
   unsigned int *src= mData;
   unsigned int *src0= mData + (mHeight-1)*mWidth;
   unsigned int *src1= mData;
   unsigned int *src2= mData + mWidth;
   unsigned int *dst= mData= new unsigned int[mWidth*mHeight];

   for (int y=0;y<mHeight;y++)
   {
      dst[0]= calcNormal(z, src1[mWidth-1], src1[1], src0[0], src2[0]);
      for (int x=1;x<mWidth-1;x++)
      {
         dst[x]= calcNormal(z, src1[x-1], src1[x+1], src0[x], src2[x]);
      }
      dst[mWidth-1]= calcNormal(z, src1[mWidth-2], src1[0], src0[mWidth-1], src2[mWidth-1]);

      dst+=mWidth;
      src0= src1;
      src1= src2;
      if (y<mHeight-2)
         src2+=mWidth;
      else
         src2=mData;

   }

   delete[] src;
}


void Image::buildDeltaMap()
{
   unsigned int *temp= mData;

   unsigned int *src0= mData + (mHeight-1)*mWidth;
   unsigned int *src1= mData;
   unsigned int *src2= mData + mWidth;
   unsigned int *dst= mData= new unsigned int[mWidth*mHeight];
   int s= 2;

   for (int y=0;y<mHeight;y++)
   {
      dst[0]= ((128 + (src1[mWidth-1]&0xff)*s - (src1[1]&0xff)*s) << 16) | ((128 + (src0[0]&0xff)*s - (src2[0]&0xff)*s) << 8);
      for (int x=1;x<mWidth-1;x++)
      {
         dst[x]= ((128 + (src1[x-1]&0xff)*s - (src1[x+1]&0xff)*s) << 16) | ((128 + (src0[x]&0xff)*s - (src2[x]&0xff)*s) << 8);
      }
      dst[mWidth-1]= ((128 + (src1[mWidth-2]&0xff)*s - (src1[0]&0xff)*s) << 16) | ((128 + (src0[mWidth-1]&0xff)*s - (src2[mWidth-1]&0xff)*s) << 8);

      dst+=mWidth;
      src0= src1;
      src1= src2;
      if (y<mHeight-2)
         src2+=mWidth;
      else
         src2=mData;

   }

   delete[] temp;
}
