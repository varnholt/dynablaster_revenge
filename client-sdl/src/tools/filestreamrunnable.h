#ifndef FILESTREAMRUNNABLE_H
#define FILESTREAMRUNNABLE_H

#include <QObject>
#include <QRunnable>

class FilestreamRunnable : public QObject, public QRunnable
{
   Q_OBJECT

public:

   //! constructor
   FilestreamRunnable(QObject *parent = 0);
   
   //! setter for filename
   void setFilename(const QString& filename);

   //! getter for filename
   const QString& getFilename() const;


signals:

   //! emitted when loading is finished
   void loaded(char* buffer, int size);
      

protected:

   //! run
   void run();

   //! filename
   QString mFilename;
   
};

#endif // FILESTREAMRUNNABLE_H
