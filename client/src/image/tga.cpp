// basic tga loader

#include "stdafx.h"
#include "tools/filestream.h"
#include "tga.h"

class TGAHeader
{
public:
   TGAHeader()
    : identsize(0),
      cmaptype(0),
      imagetype(0),
      cmapstart(0),
      cmaplength(0),
      cmapformat(0),
      originx(0),
      originy(0),
      width(0),
      height(0),
      bpp(0),
      descr(0),
      rle(false)
   {
   }

   TGAHeader(int w, int h, int bits)
   {
      rle = false;

      identsize= 0;
      cmaptype= 0;
      imagetype= 2; // rgb + no rle

      cmapstart= 0;
      cmaplength= 0;
      cmapformat= 0;

      originx= 0;
      originy= 0;

      width= w;
      height= h;
      bpp= bits;
      descr= 0;//(1<<5);
   }

   TGAHeader(Stream& stream)
   {
      read(stream);
   }

   void read(Stream& stream)
   {
      identsize= stream.getByte();    // number of ident bytes after header, usually 0
      cmaptype= stream.getByte();     // type of colour map 0=none, 1=has palette
      imagetype= stream.getByte();    // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
      rle= imagetype>=8;              // rle?
      imagetype&=7;

      cmapstart= stream.getWord();    // first colour map entry in palette
      cmaplength= stream.getWord();   // number of colours in palette
      cmapformat= stream.getByte();   // number of bits per palette entry 15,16,24,32

      originx= stream.getWord();      // image x origin
      originy= stream.getWord();      // image y origin

      width= stream.getWord();
      height= stream.getWord();
      bpp= stream.getByte();
      descr= stream.getByte();

      // skip ident data
      stream.skip(identsize);
   }

   void write(Stream& stream)
   {
      stream.writeByte(identsize);
      stream.writeByte(cmaptype);
      stream.writeByte(imagetype);

      stream.writeWord(cmapstart);
      stream.writeWord(cmaplength);
      stream.writeByte(cmapformat);

      stream.writeWord(originx);
      stream.writeWord(originy);

      stream.writeWord(width);
      stream.writeWord(height);
      stream.writeByte(bpp);
      stream.writeByte(descr);

      for (int i=0;i<identsize;i++)
         stream.writeByte(0);
   }

   unsigned char  identsize;    // size of ID field that follows 18 byte header (0 usually)
   unsigned char  cmaptype;     // type of colour map 0=none, 1=has palette
   unsigned char  imagetype;    // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

   short          cmapstart;    // first colour map entry in palette
   short          cmaplength;   // number of colours in palette
   unsigned char  cmapformat;   // number of bits per palette entry 15,16,24,32

   short          originx;      // image x origin
   short          originy;      // image y origin

   unsigned short width;
   unsigned short height;
   unsigned char  bpp;
   unsigned char  descr;
   bool           rle;
};


void loadPal24(Stream *stream, unsigned int *dst, int size)
{
   int i;
   for (i=0;i<size;i++)
   {
      unsigned char r= stream->getByte();
      unsigned char g= stream->getByte();
      unsigned char b= stream->getByte();
      *dst++= (255<<24)+(b<<16)+(g<<8)+(r);
   } 
}

void loadPal32(Stream *stream, unsigned int *dst, int size)
{
   int i;
   for (i=0;i<size;i++)
   {
      unsigned char r= stream->getByte();
      unsigned char g= stream->getByte();
      unsigned char b= stream->getByte();
      unsigned char a= stream->getByte();
      *dst++= (a<<24)+(b<<16)+(g<<8)+(r);
   } 
}

unsigned int loadPixel8(Stream *stream, unsigned int *pal)
{
   unsigned char index= stream->getByte();
   return pal[index];
}


unsigned int loadPixel16(Stream *stream, unsigned int * /*pal*/)
{
   unsigned short rgb= stream->getWord();

   unsigned char a= (rgb >> 15 & 1) * 255;
   unsigned char r= (rgb & 31)<<3;
   unsigned char g= (rgb >> 5 & 31)<<3;
   unsigned char b= (rgb >> 10 & 31)<<3;

   return (a<<24) + (b<<16) + (g<<8) + (r);
}


unsigned int loadPixel24(Stream *stream, unsigned int * /*pal*/)
{
   unsigned char a= 255;
   unsigned char r= stream->getByte();
   unsigned char g= stream->getByte();
   unsigned char b= stream->getByte();

   return (a<<24) + (b<<16) + (g<<8) + (r);
}

unsigned int loadPixel32(Stream *stream, unsigned int * /*pal*/)
{
   unsigned char r= stream->getByte();
   unsigned char g= stream->getByte();
   unsigned char b= stream->getByte();
   unsigned char a= stream->getByte();

   return (a<<24) + (b<<16) + (g<<8) + (r);
}

int loadtga(const char *fname, void **buf, int *sizex, int *sizey)
{
   TGAHeader      info;
   char           name[256];
   unsigned int   pal[256];

   if (fname)
   {
      strcpy(name, fname);
      strcat(name, ".tga");
   }

   FileStream stream;

   if (!fname || !stream.open(name))
   {
      *sizex= 1;
      *sizey= 1;
      *buf= new unsigned int(0xffffffff);
      return 0;
   }

   info.read(stream);
   // read header
//   stream->getData(&info, sizeof(TGAHeader));

   // post some debug infos
//   printf("load tga: %s (%dx%dx%d) pal:%d\n", name, info.width, info.height, info.bpp, info.cmaplength);


   unsigned int *data= new unsigned int[info.width * info.height];

   // get palette data
   if (info.imagetype==1) // indexed colors
   {
      // load palette
      if (info.cmaplength<=256)
      switch (info.cmapformat)
      {
         case 24: loadPal24(&stream, pal, info.cmaplength); break;
         case 32: loadPal32(&stream, pal, info.cmaplength); break;
         default: break;
      }
   }
/*
   else
   if (info.imagetype==2) // rgb data
   {
      // there is no palette
   }
*/
   else
   if (info.imagetype==3) // grey-scale
   {
      // create grey palette, so we can handle greyscale just as 8bit data
      for (int i=0;i<256;i++) pal[i]= (255<<24)+(i<<16)+(i<<8)+(i);
   }

   bool topdown= (bool) (info.descr >> 5 & 1);
   unsigned int (*loadPixel)(Stream*, unsigned int*) = 0;
   switch(info.bpp)
   {
      case 8:  loadPixel= loadPixel8; break;
      case 16: loadPixel= loadPixel16;break;
      case 24: loadPixel= loadPixel24;break;
      case 32: loadPixel= loadPixel32;break;
      default: break;
   }

   if (info.rle)
   {
      int scan= 0;
      int count= 0;
      bool single;
      unsigned int col= 0;
      unsigned int *dst= 0;
      int y= 0;
      while (true)
      {
         // next scanline?
         if (scan==0)
         {
            scan= info.width;
            if (topdown)
               dst= data + y * info.width;
            else
               dst= data + (info.height-1-y) * info.width;
            y++;
            if (y>info.height)
               break;
         }

         if (count==0)
         {
            count= stream.getByte() + 1;
            if (count>128)
            {
               // repeat single pixel color
               single= true;
               count-=128;
               col= loadPixel(&stream, pal);
            }
            else
               single= false;
         }

         int len= scan < count ? scan : count;
         if (single)
         {
            for (int x=0; x<len; x++)
               *dst++= col;
         }
         else
         {
            for (int x=0; x<len; x++)
               *dst++= loadPixel(&stream, pal);
         }
         
         count-=len;
         scan-=len;
      }
   }
   else
   {
      for (int y=0; y<info.height; y++)
      {
         unsigned int *dst;
         if (topdown)
            dst= data + y * info.width;
         else
            dst= data + (info.height-1-y) * info.width;
         for (int x=0; x<info.width; x++)
            *dst++= loadPixel(&stream, pal);
      }
   }

   *buf= data;
   if (sizex)
      *sizex= info.width;
   if (sizey)
      *sizey= info.height;

   return 32;
}


int savetga(const char *fname, unsigned int *data, int width, int height)
{
   FileStream stream;
   TGAHeader      info(width, height, 32);

   if (!stream.open(fname, true))
      return 0;

   info.write(stream);
   for (int y=0; y<height; y++)
   {
      unsigned int *src= data + (height-1-y) * info.width;
      for (int x=0; x<info.width; x++)
      {
         unsigned int c= src[x];
         unsigned char b= c & 255;
         unsigned char g= c >> 8 & 255;
         unsigned char r= c >> 16 & 255;
         unsigned char a= c >> 24 & 255;

         stream.writeByte(b);
         stream.writeByte(g);
         stream.writeByte(r);
         stream.writeByte(a);
      }
   }

   stream.close();

   return 32;
}

