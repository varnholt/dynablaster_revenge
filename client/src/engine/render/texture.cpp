#include "texture.h"
#include "texturepool.h"

Texture::Texture()
: Referenced()
, mTextureId(0)
{
}

Texture::Texture(unsigned int textureId)
: Referenced()
, mTextureId(textureId)
{
}

Texture::Texture(const Texture& texture)
: Referenced(texture)
, mTextureId( texture.getTexture() )
{
}

Texture::~Texture()
{
   // this is the last instance?
   // another instance is inside the pool!
   if (mTextureId && getRefCount()<=2)
   {
        TexturePool::Instance()->remove(*this);
   }
}

Texture& Texture::operator = (const Texture& texture)
{
   if (this != &texture)
   {
      // texture is not referenced: queue for deletion
      if (getRefCount() == 1)
      {
         if (mTextureId)
            TexturePool::Instance()->remove(mTextureId);
         delete mReferences;
      }

      mReferences= texture.getRef();
      addRef();
      mTextureId= texture.getTexture();
   }
   return *this;
}

