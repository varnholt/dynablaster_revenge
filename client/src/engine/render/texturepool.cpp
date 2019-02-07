#include "texturepool.h"
#include "image/imagepool.h"
#include "../materials/material.h"
#include "framework/renderdevice.h"

TexturePool::TexturePool()
: mBlock(false)
, mMemory(0)
{
   ImagePool *pool= ImagePool::Instance();
   connect(pool, SIGNAL(updateImage(Image*)), this, SLOT(updateImage(Image*)));
}


TexturePool::~TexturePool()
{
}


Texture TexturePool::getTexture(Image* image, int flags)
{
   unsigned int textureId= 0;
   textureId= Material::uploadMap(*image, flags);

   // keep track of consumed memory
   int size= image->getWidth() * image->getHeight();
   mMemory += size * 4;
   if (flags & 2) // approx. mipmaps
      mMemory += size * 4 / 3;

   //  printf("total texture memory: %d kb \n", mMemory >> 10);

   return Texture(textureId);
}


Texture TexturePool::getTexture(const char *filename, int flags)
{
   if (filename)
   {
      QMap<QString,Texture>::ConstIterator it= mPool.constFind(filename);
      if (it != mPool.constEnd())
         return it.value();
      else
      {
         Image *image= new Image(filename);
         Texture texture= getTexture(image, flags);
         mPool.insert( QString(filename), texture );
         delete image;
         return texture;
      }
   }

   return Texture();
}


void TexturePool::updateImage(Image * /*image*/)
{
/*
   QMap<Image*,unsigned int>::ConstIterator it= mPool.constFind(image);
   if (it != mPool.constEnd())
   {
      unsigned int texture= it.value();
      Material::updateMap(texture, *image);
   }
*/
}


void TexturePool::update()
{
   while (mRemoval.size() > 0)
   {
      unsigned int textureId= mRemoval.takeLast();
      activeDevice->deleteTexture(textureId);
   }
}

void TexturePool::remove(const Texture& texture)
{
   if (mBlock)
       return;

   mBlock= true;
   bool found= false;
   // remove from pool
   QMap<QString,Texture>::Iterator it;
   for (it= mPool.begin(); it!= mPool.end(); )
   {
      // const QString& fileName= it.key();
      const Texture& pool= it.value();
      if (pool.getTexture() == texture.getTexture())
      {
         if (texture.getRefCount() <= 2)
         {
            it= mPool.erase(it);
            found= true;
         }
      }
      else
         it++;
   }

   if (found)
      mRemoval.add( texture.getTexture() );
   else
      if (texture.getRefCount() == 1)
          mRemoval.add( texture.getTexture() );

   mBlock= false;
}
