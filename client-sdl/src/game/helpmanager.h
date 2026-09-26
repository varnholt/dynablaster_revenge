#ifndef HELPMANAGER_H
#define HELPMANAGER_H

#include <string>

// Qt
#include <QObject>

#include "constants.h"

class HelpManager : public QObject
{
   Q_OBJECT

public:

   static HelpManager* getInstance();


signals:

   void messageAdded(
      const std::string& page,
      const std::string& message,
      Constants::HelpSeverity severity,
      Constants::HelpLocation location,
      int delay
   );


public slots:

   void addMessage(
      const std::string& page,
      const std::string& message,
      Constants::HelpSeverity severity,
      Constants::HelpLocation location = Constants::HelpLocationTopRight,
      int delay = 0
   );


protected:

   HelpManager();

   static HelpManager* sInstance;
};

#endif // HELPMANAGER_H
