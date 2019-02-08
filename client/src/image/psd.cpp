#include "psd.h"
#include "tools/filestream.h"
#include "image/image.h"
#include <memory.h>
#include <stdlib.h>

// PSD Header -----------------------------------------------------------------

#define VISIBILITY_FLAG 0x02

//! get width of image
int PSD::Header::getWidth() const
{
   return mWidth;
}

//! get height of image
int PSD::Header::getHeight() const
{
   return mHeight;
}

//! load psd header from stream
void PSD::Header::load(Stream *stream)
{
	stream->getData(mSign, 4);
	mVersion= stream->getWord();
	stream->getData(mReserved, 6);
	mChannels= stream->getWord();
	mHeight= stream->getInt();
	mWidth= stream->getInt();
	mDepth= stream->getWord();
	mMode= (ColorMode)stream->getWord();
}

// Layer ----------------------------------------------------------------------

//! construct empty layer
PSD::Layer::Layer()
 : mTop(0),
   mLeft(0),
   mBottom(0),
   mRight(0),
   mChannelCount(0),
   mImage(0),
   mChannels(0),
   mOpacity(0),
   mClipping(0),
   mFlags(0),
   mName(0)
{
   mBlendMode[0]=0;
   mBlendMode[1]=0;
   mBlendMode[2]=0;
   mBlendMode[3]=0;
}

//! destroy layer
PSD::Layer::~Layer()
{
   if (mChannels)
      delete[] mChannels;
   if (mName)
      delete[] mName;

   delete mImage;
}

const char* PSD::Layer::getName() const
{
   return mName;
}

int PSD::Layer::getBottom() const
{
   return mBottom;
}

int PSD::Layer::getTop() const
{
   return mTop;
}

int PSD::Layer::getLeft() const
{
   return mLeft;
}

int PSD::Layer::getWidth() const
{
   return mRight - mLeft;
}

int PSD::Layer::getHeight() const
{
   return mBottom - mTop;
}

void PSD::Layer::move(int x, int y)
{
   mRight += x;
   mLeft += x;

   mTop += y;
   mBottom +=y;
}

void PSD::Layer::setX(int x)
{
   int width = mRight - mLeft;

   mLeft = x;
   mRight = x + width;
}

void PSD::Layer::setY(int y)
{
   int height = mBottom - mTop;

   mTop = y;
   mBottom = y + height;
}

void PSD::Layer::setBottom(int v)
{
   mBottom = v;
}

void PSD::Layer::setTop(int v)
{
   mTop = v;
}

Image* PSD::Layer::getImage() const
{
    return mImage;
}

int PSD::Layer::getOpacity() const
{
   return mOpacity;
}

bool PSD::Layer::isVisible() const
{
   return ((mFlags & VISIBILITY_FLAG) != 2);
}

void PSD::Layer::setVisible(bool visible)
{
   if (visible)
      mFlags &= ~VISIBILITY_FLAG;
   else
      mFlags |= VISIBILITY_FLAG;
}

const PSD::Layer::Channel* PSD::Layer::getChannel(int id) const
{
   for (int i=0; i<mChannelCount ;i++)
   {
       if (mChannels[i].getID() == id)
         return &mChannels[i];
   }

   return 0;
}

void PSD::Layer::setOpacity(int opacity)
{
   mOpacity = opacity;
}

//! load layer parameters from stream
void PSD::Layer::load(Stream *stream)
{
   char sig[4];
   int size;

   mTop= stream->getInt();
   mLeft= stream->getInt();
   mBottom= stream->getInt();
   mRight= stream->getInt();

   mChannelCount= stream->getWord();
   mChannels= new PSD::Layer::Channel[4];

   for (int i=0;i<mChannelCount;i++)
   {
      mChannels[i].load(stream);
   }

   stream->getData(sig, 4);
   // TODO: drop out if sig doesn't fit
   stream->getData(mBlendMode, 4);
   mOpacity= stream->getByte();
   mClipping= stream->getByte();
   mFlags= stream->getByte();
   stream->getByte(); // filler

   int totalsize= stream->getInt();
   int curpos= stream->pos();

   // Layer mask / adjust layer data
   size= stream->getInt();
   stream->skip(size); // skip block

   // Layer blending ranges data
   size= stream->getInt();
   stream->skip(size); // skip block

   mName= PSD::loadString(stream);

   int blockHeader= 0;
   while (totalsize - (stream->pos() - curpos) > 4)
   {
      blockHeader= (blockHeader << 8) | stream->getByte();
//      char* test= (char*)&blockHeader;
      if (blockHeader == '8BIM')
      {
         int blockId= stream->getInt();
         int blockSize= stream->getInt();
         int curPos= stream->pos();
         if (blockId == 'luni')
         {
            delete[] mName;
            unsigned int length= stream->getInt();
            mName= new char[length+1];
            for (unsigned int i=0; i<length; i++)
            {
               mName[i]= stream->getWord() & 255;
            }
            mName[length]= 0;
         }
         // skip rest of block
         stream->skip(blockSize - (stream->pos() - curPos));
         blockHeader= 0;
      }
   }


   // skip rest of data
   stream->skip( totalsize - (stream->pos() - curpos) );
}


//! load channels
void PSD::Layer::loadChannels(Stream *stream)
{
   int height= mBottom - mTop;
   int width= mRight - mLeft;

   for (int i=0; i<mChannelCount; i++)
   {
      unsigned short compress= stream->getWord();

      switch (compress)
      {
      case 0: // Raw
         mChannels[i].loadRaw(width, height, stream);
         break;

      case 1: // RLE
         mChannels[i].loadRLE(width, height, stream);
         break;

      case 2: // Zip
         break;

      case 3: // Zip Prediction
         break;

      default:
         break;
      }
   }

   if (mChannelCount==3)
   {
      mChannels[mChannelCount].init(-1, width, height);
      mChannelCount++;
   }

   mImage= new Image(width, height);
   for (int y=0; y<height; y++)
   {
      unsigned int *dst= mImage->getScanline(y);

      unsigned char *red=   getChannel(0)->getScanline(y);
      unsigned char *green= getChannel(1)->getScanline(y);
      unsigned char *blue=  getChannel(2)->getScanline(y);
      unsigned char *alpha= getChannel(-1)->getScanline(y);

      for (int x=0; x<width; x++)
      {
         unsigned char a,r,g,b;

         a= alpha[x];
         if (a>0)
         {
            r= red[x];
            g= green[x];
            b= blue[x];
         }
         else
         {
            r= 0;
            g= 0;
            b= 0;
         }
         dst[x] = (a << 24) | (r << 16) | (g << 8) | b;
      }
   }

   delete[] mChannels;
   mChannels= 0;
}

/*
unsigned int* PSD::Layer::flatten(int w, int h) const
{
   if (w==0) w= getWidth();
   if (h==0) h= getHeight();

   unsigned int* data= new unsigned int[w*h];

   int dstX= w;
   int dstY= h;

   if (w > getWidth()) w= getWidth();
   if (h > getHeight()) h= getHeight();

   for (int y=0; y<h; y++)
   {
      unsigned int *dst= data + y*dstX;

      unsigned char *red=   getChannel(0)->getScanline(y);
      unsigned char *green= getChannel(1)->getScanline(y);
      unsigned char *blue=  getChannel(2)->getScanline(y);
      unsigned char *alpha= getChannel(-1)->getScanline(y);

      for (int x=0; x<w; x++)
      {
         unsigned char a= alpha[x];
         unsigned char r= red[x];
         unsigned char g= green[x];
         unsigned char b= blue[x];

         dst[x] = (a << 24) | (r << 16) | (g << 8) | b;
      }

      for (int x=w; x<dstX; x++)
      {
         unsigned char a= alpha[w-1];
         unsigned char r= red[w-1];
         unsigned char g= green[w-1];
         unsigned char b= blue[w-1];

         dst[x] = (a << 24) | (r << 16) | (g << 8) | b;
      }
   }

   for (int y=h; y<dstY; y++)
   {
      unsigned int *src= data + (h-1)*dstX;
      unsigned int *dst= data + y*dstX;
      memcpy(dst, src, dstX*4);
   }

   return data;
}
*/

// Channel --------------------------------------------------------------------

//! construct empty channel
PSD::Layer::Channel::Channel()
: mID(0)
, mSize(0)
, mWidth(0)
, mData(0)
{
}

//! destroy channel
PSD::Layer::Channel::~Channel()
{
   if (mData)
      delete[] mData;
}

short PSD::Layer::Channel::getID() const
{
   return mID;
}

unsigned char* PSD::Layer::Channel::data() const
{
   return mData;
}

//! load channel from stream
void PSD::Layer::Channel::load(Stream *stream)
{
   mID= stream->getWord();
   mSize= stream->getInt();
}

//! load rle compressed channel
void PSD::Layer::Channel::loadRLE(int width, int height, Stream *stream)
{
   mWidth= width;
   unsigned short *scanSize= new unsigned short[height];
   for (int y=0;y<height;y++)
      scanSize[y]= stream->getWord();

   mData= new unsigned char[width * height];

   for (int y=0;y<height;y++)
   {
      unsigned char *dst= mData + y*width;

      int size= scanSize[y];

      while (size>0)
      {
         int start= stream->pos();

         char ctrl= stream->getChar();
         if (ctrl >= 0)
         {
            stream->getData(dst, ctrl+1);
            dst+= ctrl+1;
         }
         else if (ctrl > -128)
         {
            unsigned char col= stream->getByte();
            for (int x=0;x<(1-ctrl);x++) 
               *dst++= col;
         }

         int end= stream->pos();
         size-= (end - start);
      }
   }

   delete[] scanSize;
}

//! load uncompressed channel
void PSD::Layer::Channel::loadRaw(int width, int height, Stream *stream)
{
   mWidth= width;
   mData= new unsigned char[width * height];

   for (int y=0;y<height;y++)
   {
      unsigned char *dst= mData + y*width;
      stream->getData(dst, width);
   }
}

void PSD::Layer::Channel::init(int id, int width, int height)
{
   mID= id;
   mData= new unsigned char[width * height];
   memset(mData, 0xff, width*height);
}

unsigned char* PSD::Layer::Channel::getScanline(int y) const
{
   return mData + y * mWidth;
}

// PSD Interface --------------------------------------------------------------

//! construct empty psd
PSD::PSD()
: mHeader()
, mLayerCount(0)
, mLayers(0)
{
}

//! destroy psd
PSD::~PSD()
{
   delete[] mLayers;
}

//! get image width
int PSD::getWidth() const
{
   return mHeader.getWidth();
}

//! get image height
int PSD::getHeight() const
{
   return mHeader.getHeight();
}

//! get number of layers
int PSD::getLayerCount() const
{
   return mLayerCount;
}

//! get layer
PSD::Layer* PSD::getLayer(int index) const
{
   return &mLayers[index];
}

//! get layer by name
PSD::Layer* PSD::getLayer(const char* name) const
{
   for (int i=0; i<mLayerCount; i++)
   {
      const char* layerName= mLayers[i].getName();
      if (strcmp(layerName, name) == 0)
         return &mLayers[i];
   }
   return 0;
}

//! load padded string from stream
char* PSD::loadString(Stream *stream)
{
   unsigned char size= stream->getByte();
   char *name= new char[size+1];
   for (int i=0;i<size;i++)
      name[i]= stream->getChar();
   name[size]= 0;
   return name;
}

//! load image resource section
void PSD::loadImageResourceSection(Stream *stream)
{
	int total= stream->getInt();
   stream->skip( total );
   return;

	int pos= stream->pos();
	int size;

	do
	{
      size= 0;
      unsigned int sign= stream->getInt();
      if (sign == 0x3842494d)
      {
         /*unsigned short id=*/ stream->getWord();
		   char *name= loadString(stream);
		   // TODO: Check id; get useful information
		   size= stream->getInt();
		   stream->skip(size);
		   delete[] name;
      }
	} while (size);

   stream->skip( total - (stream->pos()-pos) );
}


//! layer informationen laden
void PSD::loadLayerInformation(Stream *stream)
{
   // total size of layer and mask block
   /*int total=*/ stream->getInt();

   // size of layer block
   /*int size=*/ stream->getInt();
	
   // layer allozieren
   mLayerCount= abs( stream->getShort() );
   mLayers= new Layer[mLayerCount];

   // load layer parameters
   for (int i=0; i<mLayerCount; i++)
      mLayers[i].load(stream);

   // load layer channels (bitmap data)
   for (int i=0;i<mLayerCount;i++)
      mLayers[i].loadChannels(stream);

   // TODO: skip mask block
}


//! load psd from file
bool PSD::load(const char *filename)
{
   bool res= false;
   FileStream stream;

   if (stream.open(filename))
      res= load(&stream);

   return res;
}


//! load psd from stream
bool PSD::load(Stream *stream)
{
   int endian= stream->getEndian();        // endianness des streams merken
   stream->setEndian(1);                   // auf big endian wechseln (photoshop kommt vom mac)

   mHeader.load(stream);                   // header laden

   // color mode data ueberspringen
   int size= stream->getInt();
   stream->skip(size);

   loadImageResourceSection(stream);      // Image Resource Section parsen

   loadLayerInformation(stream);          // Layer laden

   stream->setEndian(endian);             // endianness zuruecksetzen

   return true;
}

