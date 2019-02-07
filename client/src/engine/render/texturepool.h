#ifndef TEXTUREPOOL_H
#define TEXTUREPOOL_H

#include <QObject>
#include <QMap>
#include "tools/singleton.h"
#include "tools/array.h"
#include "texture.h"

class Image;

class TexturePool : public QObject, public Singleton<TexturePool>
{
   Q_OBJECT

public:
   enum Filter
   {
      Nearest = 0,
      Linear = 1,
      MipMap = 2,
      Trilinear = Linear | MipMap,
      Clamp = 4,
      PremultipliedAlpha = 8
   };

   TexturePool();
   ~TexturePool();

   Texture getTexture(const char* filename, int filter = Trilinear);
   Texture getTexture(Image* image, int filter = Trilinear);

   void remove(const Texture& texture);

   void update();

private slots:
   void updateImage(Image*);

private:
   QMap<QString,Texture> mPool;
   Array<unsigned int>   mRemoval;
   bool                  mBlock;
   unsigned int          mMemory;
};

#endif
