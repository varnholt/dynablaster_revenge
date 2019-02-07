#ifndef HELPMANAGER_H
#define HELPMANAGER_H

// Qt
#include <QObject>

#include "helpelement.h"

class HelpManager : public QObject
{
   Q_OBJECT

public:

   static HelpManager* getInstance();
   

signals:

   void messageAdded(
      const QString& page,
      const QString& message,
      Constants::HelpSeverity severity,
      Constants::HelpLocation location,
      int delay
   );

   
public slots:

   void addMessage(
      const QString& page,
      const QString& message,
      Constants::HelpSeverity severity,
      Constants::HelpLocation location = Constants::HelpLocationTopRight,
      int delay = 0
   );


protected:

   HelpManager();

   static HelpManager* sInstance;
};

#endif // HELPMANAGER_H
