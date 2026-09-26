#ifndef IMAGEPOOL_H
#define IMAGEPOOL_H

#include "image.h"
#include "tools/singleton.h"

#include <QObject>

#include <string>
#include <unordered_map>

class ImagePool : public QObject, public Singleton<ImagePool>
{
   Q_OBJECT

public:
   ImagePool();
   ~ImagePool();

   Image* getImage(const char* filename, int flags = 0);

   void remove(Image* image);

signals:
   void updateImage(Image*);

private:
   std::unordered_map<std::string, Image*> mPool;
};

#endif
