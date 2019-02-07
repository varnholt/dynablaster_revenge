#ifndef GAMEMENUINTERFACE_H
#define GAMEMENUINTERFACE_H

// base
#include "menuinterface.h"

// Qt
#include <QMap>
#include <QSet>

// forward declarations
class MenuPage;

class GameMenuInterface : public MenuInterface
{
   Q_OBJECT

   public:

      //! constructor
      GameMenuInterface(QObject* parent = 0);

      // menu operation

      //! update background color for different pages
      void updateBackgroundColor(const QString& pageName);


   protected:

      //! map containing page init flags
      QSet<MenuPage*> mPagesInitialized;
};

#endif // GAMEMENUINTERFACE_H
