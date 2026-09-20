// header
#include "filestreamrunnable.h"

// tools
#include "tools/filestream.h"


FilestreamRunnable::FilestreamRunnable(QObject *parent)
 : QObject(parent)
{
}


void FilestreamRunnable::run()
{
   FileStream stream;

   if (stream.open( qPrintable(mFilename) ))
   {
      char *buffer= new char[stream.size()];
      stream.getData(buffer, stream.size());

      emit loaded(buffer, stream.size());
   }
}


void FilestreamRunnable::setFilename(const QString& filename)
{
   mFilename = filename;
}


const QString& FilestreamRunnable::getFilename() const
{
   return mFilename;
}


