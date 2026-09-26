#include "imagepool.h"
#include <QDir>

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
   std::map<QString, Image*>::iterator it;
   for (it = mPool.begin(); it != mPool.end();)
   {
      if (it->second == image)
         it = mPool.erase(it);
      else
         it++;
   }
}

QDateTime ImagePool::getFileDate(const QFileInfoList& /*list*/, const QString& /*filename*/) const
{
   /*
      for (QFileInfoList::ConstIterator it= list.constBegin(); it!= list.constEnd(); it++)
      {
         const QFileInfo& info= *it;
         QString test= info.baseName();
         if (test == filename)
            return info.lastModified();
      }
   */
   return QDateTime();
}

void ImagePool::refresh()
{
   /*
      QStringList filters;
      filters.append("*.tga");
      QDir dir("data/game");
      QFileInfoList list= dir.entryInfoList(filters, QDir::Files);

      for (QMap<QString,Image*>::ConstIterator it= mPool.constBegin(); it!= mPool.constEnd(); it++)
      {
         Image *image= it.value();
         QString rawname= it.key();
         QString filename= rawname;// +".tga";
         QDateTime date= getFileDate(list, filename);
         if (date.isValid())
         {
            QString d1= image->getDate().toString();
            QString d2= date.toString();
            if (image->getDate() < date)
            {
               QByteArray str= rawname.toLatin1();
               image->load(str.data());
               image->setDate( QDateTime::currentDateTime() );

               emit updateImage(image);
            }
         }
      }
   */
}

Image* ImagePool::getImage(const char* filename, int /*preprocessingFlags*/)
{
   Image* image;
   QString name(filename);
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
