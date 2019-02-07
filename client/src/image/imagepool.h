#ifndef IMAGEPOOL_H
#define IMAGEPOOL_H

#include <QMap>
#include <QString>
#include <QDateTime>
#include <QFileInfoList>
#include "image.h"
#include "tools/singleton.h"

class QTimer;

class ImagePool : public QObject, public Singleton<ImagePool>
{
   Q_OBJECT

public:
   ImagePool();
   ~ImagePool();

   Image* getImage(const char *filename, int flags = 0);

   void remove(Image* image);

signals:
   void updateImage(Image*);

private slots:
   void refresh();

private:
   QDateTime getFileDate(const QFileInfoList& list, const QString& filename) const;

   QMap<QString,Image*> mPool;
   QTimer *mRefresh;
};

#endif
