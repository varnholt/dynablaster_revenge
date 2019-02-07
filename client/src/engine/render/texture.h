#pragma once

#include "tools/referenced.h"

class Texture : public Referenced
{
public:
   Texture();
   Texture(unsigned int textureId);
   Texture(const Texture& texture);
   ~Texture();

   Texture& operator = (const Texture& texture);

   operator unsigned int () const
   {
      return mTextureId;
   }

   unsigned int getTexture() const
   {
      return mTextureId;
   }

private:
   unsigned int mTextureId;
};
