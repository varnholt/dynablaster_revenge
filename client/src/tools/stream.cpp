#include "stream.h"
#include <memory.h>
#include "string.h"

int Stream::mMachineEndian=-1;

Stream::Stream()
: mEndian(0)
, mPosition(0)
{
   if (mMachineEndian<0)
   {
      // test endian coding
      unsigned int test=0x01020304;
      unsigned char *data= (unsigned char*)&test;

      if (data[0]==4 && data[1]==3 && data[2]==2 && data[3]==1) mMachineEndian=0; // little endian
      if (data[0]==1 && data[1]==2 && data[2]==3 && data[3]==4) mMachineEndian=1; // big endian
   }
}

//! destructor
Stream::~Stream()
{
}

const String& Stream::getPath() const
{
   static String dummy;
   return dummy;
}

void operator << (float& v, Stream& stream)
{
   v= stream.getFloat();
}

void operator >> (float& f, Stream& stream)
{
   stream.writeFloat(f);
}

//! endian conversion
void Stream::swap16(void *data)
{
   unsigned char *src= (unsigned char*)data;
   unsigned char t;
   t= src[0]; src[0]= src[1]; src[1]= t;
}


//! endian conversion
void Stream::swap32(void *data)
{
   unsigned char *src= (unsigned char*)data;
   unsigned char t;
   t= src[0]; src[0]= src[3]; src[3]= t;
   t= src[1]; src[1]= src[2]; src[2]= t;
}


//! get current endian mode
int Stream::getEndian() const
{
   return mEndian;
}


//! set endian mode
void Stream::setEndian(int big)
{
   mEndian= big;
}

//! get char
char Stream::getChar()
{
   char c;
   getData(&c,sizeof(char));
   return c;
}


//! get byte
unsigned char Stream::getByte()
{
   unsigned char c;
   getData(&c,sizeof(char));
   return c;
}


//! get int
int Stream::getInt()
{
   int i;
   getData(&i,sizeof(int));
   if (mEndian!=mMachineEndian) swap32(&i);
   return i;
}


//! get word
int Stream::getWord()
{
   unsigned short w;
   getData(&w,sizeof(short));
   if (mEndian!=mMachineEndian) swap16(&w);
   return (int)w;
}


//! get short
short Stream::getShort()
{
   short w;
   getData(&w,sizeof(short));
   if (mEndian!=mMachineEndian) swap16(&w);
   return (int)w;
}


//! get float
float Stream::getFloat()
{
   float v;
   getData(&v, sizeof(float));
   if (mEndian!=mMachineEndian) swap32(&v);
   return v;
}


//! get string, pascal style (leading number of bytes) 
char* Stream::getString()
{
   // strings in .hjb are garanteed to be <=256 byte
   char temp[256];
   char c;
   int  size=0;

   // read chars until (inclusive) 0-terminator
   do
   {
      c= getChar();
      temp[size++]=c;
   } while (c!=0);

   // allocate and copy string
   char *str= new char[size];
   memcpy(str, temp, size);

   return str;
}


void Stream::skip(int size)
{
   char dummy[256];
   while (size>0)
   {
      int len= size;
      if (len > 256) len=256;
      getData(dummy, len);
      size-=len;
   }
}

int Stream::pos() const
{
   return mPosition;
}

void Stream::writeChar(char c)
{
   writeData(&c, sizeof(char));
}

void Stream::writeByte(unsigned char b)
{
   writeData(&b, sizeof(unsigned char));
}

void Stream::writeWord(unsigned short w)
{
   if (mEndian!=mMachineEndian) swap16(&w);
   writeData(&w,sizeof(unsigned short));
}

void Stream::writeShort(short w)
{
   if (mEndian!=mMachineEndian) swap16(&w);
   writeData(&w,sizeof(short));
}

void Stream::writeInt(int i)
{
   if (mEndian!=mMachineEndian) swap32(&i);
   writeData(&i,sizeof(int));
}

void Stream::writeFloat(float f)
{
   if (mEndian!=mMachineEndian) swap32(&f);
   writeData(&f,sizeof(float));
}

void Stream::writeString(const char* data)
{
   if (data)
   {
      int pos= 0;
      while (data[pos])
      {
         writeChar(data[pos]);
         pos++;
      };
   }
   writeChar(0);
}

