#ifndef MENU_H
#define MENU_H

// Qt
#include <QList>
#include <QObject>
#include <QSettings>

// menu
#include "menupage.h"

// forward declarations
class MenuWorkflow;


class Menu : public QObject, public QList<MenuPage*>
{
   Q_OBJECT

   public:

       Menu();

       Menu(const Menu&);

       virtual ~Menu();

       void initialize();

       static Menu* getInstance();


    public slots:

       void mouseMoved(int x, int y);

       void mousePressed(int x, int y);

       void mouseReleased();

       void keyPressed(int key, const QString&);

       void paste(const QString& text);

       MenuPage* getCurrentPage();

       void setCurrentPage(MenuPage* page);

       MenuPage* getBackground();

       MenuPage* getPageByName(const QString&);

       MenuWorkflow* getMenuWorkflow() const;


       //!
       void setMenuWorkflow(MenuWorkflow*);


      // workflow

      //! action response
      void actionResponse(
         const QString& page,
         const QString& action,
         bool ok
      );


   signals:

       // workflow

      //! action request
      void actionRequest(
          const QString& page,
          const QString& action
      );

      void pageChangeRequest(
         MenuPage* previous,
         MenuPage* current
      );

      //! a key was pressed while an item was focussed
      void actionKeyPressed(
         const QString& page,
         const QString& itemName,
         int key
      );

      //! an item was focussed
      void layerFocussed(
         const QString& page,
         const QString& itemName
      );


   private:

      QSettings* mSettings;

      MenuPage* mCurrentPage;

      MenuPage* mBackground;

      MenuWorkflow* mMenuWorkflow;

      static Menu* lInstance;

};

#endif // MENU_H
