#include "imagepool.h"
#include <QTimer>
#include <QDir>

ImagePool::ImagePool()
: mRefresh(0)
{
   mRefresh= new QTimer(this);
//   connect(mRefresh, SIGNAL(timeout()), this, SLOT(refresh()));
//   mRefresh->setInterval(1000);
//   mRefresh->start();
}

ImagePool::~ImagePool()
{
   QMap<QString,Image*>::Iterator it= mPool.begin();
   while (it != mPool.end())
   {
      Image* image= it.value();
      it= mPool.erase(it);
      delete image;
   }
}

void ImagePool::remove(Image* image)
{
   QMap<QString,Image*>::Iterator it;
   for (it=mPool.begin(); it!=mPool.end(); )
   {
      if (it.value() == image)
         it= mPool.erase(it);
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

Image* ImagePool::getImage(const char *filename, int /*preprocessingFlags*/)
{
   Image *image;
   QString name(filename);
   QMap<QString,Image*>::ConstIterator it= mPool.constFind(name);
   if (it != mPool.constEnd())
      image= it.value();
   else
   {
      image= new Image(filename);
//      image->premultiplyAlpha();
//      image->setDate( QDateTime::currentDateTime() );
      mPool.insert( QString(name), image);
   }
   return image;
}


