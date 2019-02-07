#ifndef PSD_H
#define PSD_H

class Stream;
class Image;

/*
Header
Color Mode Data Block
Image Resource Block
Layer and Mask Information Block
Image Data
*/

class PSD
{
public:

	class Header
	{
	public:
		enum ColorMode
		{
			Bitmap = 0,
			Grayscale = 1,
			Indexed = 2,
			RGB = 3,
			CMYK = 4,
			Multichannel = 7,
			Duotone = 8,
			Lab = 9
		};

		void load(Stream *stream);
      int getWidth() const;
      int getHeight() const;

	private:
		unsigned char  mSign[4];        // File ID "8BPS"
		unsigned short mVersion;        // Version number, always 1
		unsigned char  mReserved[6];    // Reserved, must be zeroed
		unsigned short mChannels;       // Number of color channels (1-24) including alpha channels
		int            mHeight;         // Height of image in pixels (1-30000)
		int            mWidth;          // Width of image in pixels (1-30000)
		unsigned short mDepth;          // Number of bits per channel (1, 8 and 16)
		ColorMode      mMode;           // Color mode
	};

	class Layer
	{
	public:
      class Channel
      {
      public:
         Channel();
         ~Channel();

         void load(Stream *stream);
         void init(int id, int width, int height);
         void loadRLE(int width, int height, Stream *stream);
         void loadRaw(int width, int height, Stream *stream);
         unsigned char* getScanline(int y) const;

         short getID() const;
         unsigned char* data() const;

      private:
         short mID;
         int   mSize;
         int   mWidth;
         unsigned char *mData;
      };

         Layer();
         ~Layer();

         void load(Stream *stream);
         void loadChannels(Stream *stream);

         int getBottom() const;
         int getTop() const;
         int getLeft() const;
         int getWidth() const;
         int getHeight() const;
         void setOpacity(int opacity);
         int getOpacity() const;
         bool isVisible() const;
         void setVisible(bool visible);
         const char* getName() const;
         void move(int x, int y);
         void setX(int x);
         void setY(int y);
         void setBottom(int v);
         void setTop(int v);
         Image* getImage() const;

      private:
         const Channel* getChannel(int id) const;

         int mTop;
         int mLeft;
         int mBottom;
         int mRight;
         unsigned short mChannelCount;
         Image* mImage;
         Channel *mChannels;
         char mBlendMode[4];
         unsigned char mOpacity;
         unsigned char mClipping;
         unsigned char mFlags;
         char *mName;
	};

public:
	PSD();
	~PSD();

   int getWidth() const;
   int getHeight() const;
   int getLayerCount() const;
   Layer* getLayer(int index) const;
   Layer* getLayer(const char* name) const;

   bool load(const char *filename);
   bool load(Stream *stream);

   static char* loadString(Stream *stream);

private:
   void loadImageResourceSection(Stream *stream);
   void loadLayerInformation(Stream *stream);

   Header mHeader;
   int    mLayerCount;
   Layer  *mLayers;

};

#endif
