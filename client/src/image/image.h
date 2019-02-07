#ifndef IMAGE_H
#define IMAGE_H

#include "tools/referenced.h"
#include "tools/string.h"

// TODO: add reference counter

class Image : public Referenced
{
public:
   Image();
   Image(const char *filename);
   Image(int x, int y);
//   Image(unsigned int *data, int x, int y, bool owner= false);
   Image(const Image& image);
   ~Image();

   const Image& operator = (const Image& image);

   void discard();
   void clear(unsigned int argb);

   void save(const char* filename);

   int getWidth() const;
   int getHeight() const;
   unsigned int *getScanline(int y) const;
   unsigned int* getData() const;
   Image downsample() const;
   void scaled(const Image& image) const;
   void load(const char *filename);
   const char* getFilename() const;
   void copy(int x, int y, const Image& source, int clamp = 0);
   void premultiplyAlpha();
   void minimum(const Image& image);
   unsigned int getPixel(float u, float v) const;
   void buildNormalMap(int z);
   void buildDeltaMap();
   const String& path() const;
   const String& filename() const;

private:
   unsigned int *mData;
   int mWidth;
   int mHeight;
   String mPath;
   String mFilename;
};

#endif
