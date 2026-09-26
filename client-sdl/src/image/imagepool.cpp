#include "imagepool.h"

ImagePool::ImagePool()
{
}

ImagePool::~ImagePool()
{
   auto it = mPool.begin();
   while (it != mPool.end())
   {
      Image* image = it->second;
      it = mPool.erase(it);
      delete image;
   }
}

void ImagePool::remove(Image* image)
{
   for (auto it = mPool.begin(); it != mPool.end();)
   {
      if (it->second == image)
         it = mPool.erase(it);
      else
         it++;
   }
}

Image* ImagePool::getImage(const char* filename, int /*preprocessingFlags*/)
{
   Image* image;
   std::string name(filename);
   auto it = mPool.find(name);
   if (it != mPool.end())
      image = it->second;
   else
   {
      image = new Image(filename);
      //      image->premultiplyAlpha();
      //      image->setDate( QDateTime::currentDateTime() );
      mPool[name] = image;
   }
   return image;
}
