#include "texturepool.h"
#include "../materials/material.h"
#include "framework/renderdevice.h"
#include "image/imagepool.h"

TexturePool::TexturePool() : mBlock(false), mMemory(0)
{
   ImagePool* pool = ImagePool::Instance();
   connect(pool, SIGNAL(updateImage(Image*)), this, SLOT(updateImage(Image*)));
}

TexturePool::~TexturePool()
{
}

Texture TexturePool::getTexture(Image* image, int flags)
{
   unsigned int textureId = 0;
   textureId = Material::uploadMap(*image, flags);

   // keep track of consumed memory
   int size = image->getWidth() * image->getHeight();
   mMemory += size * 4;
   if (flags & 2)  // approx. mipmaps
      mMemory += size * 4 / 3;

   //  printf("total texture memory: %d kb \n", mMemory >> 10);

   return Texture(textureId);
}

Texture TexturePool::getTexture(const char* filename, int flags)
{
   if (filename)
   {
      auto it = mPool.find(filename);
      if (it != mPool.end())
         return it->second;
      else
      {
         Image* image = new Image(filename);
         Texture texture = getTexture(image, flags);
         mPool[std::string(filename)] = texture;
         delete image;
         return texture;
      }
   }

   return Texture();
}

void TexturePool::updateImage(Image* /*image*/)
{
}

void TexturePool::update()
{
   while (mRemoval.size() > 0)
   {
      unsigned int textureId = mRemoval.takeLast();
      activeDevice->deleteTexture(textureId);
   }
}

void TexturePool::remove(const Texture& texture)
{
   if (mBlock)
      return;

   mBlock = true;
   bool found = false;
   // remove from pool
   for (auto it = mPool.begin(); it != mPool.end();)
   {
      const Texture& pool = it->second;
      if (pool.getTexture() == texture.getTexture())
      {
         if (texture.getRefCount() <= 2)
         {
            it = mPool.erase(it);
            found = true;
         }
      }
      else
         it++;
   }

   if (found)
      mRemoval.add(texture.getTexture());
   else if (texture.getRefCount() == 1)
      mRemoval.add(texture.getTexture());

   mBlock = false;
}
