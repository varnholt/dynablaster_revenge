#ifndef GAMEMENUINTERFACEMAIN_H
#define GAMEMENUINTERFACEMAIN_H

// base
#include "gamemenuinterface.h"

// menus
#include "hosthistory.h"

class MenuPageEditableComboBoxItem;
class MenuPageTextEditItem;

class GameMenuInterfaceMain : public GameMenuInterface
{
   Q_OBJECT

public:

   GameMenuInterfaceMain(QObject* parent = 0);

   // main menu

   //! getter for login data
   void updateLoginData();

   //! serialize login data
   void serializeLoginData();

   //! deserialize login data
   void deserializeLoginData();

   //! update mainmenu led status
   void updateLedStatus();

   //! deserialize version
   void deserializeVersion();


public slots:

   //! open pouet page
   void openPouet();

   //! open facebook page
   void openFacebook();

   //! open home page
   void openHome();


protected:

   MenuPageTextEditItem * getHostTextEdit() const;

   MenuPageTextEditItem* getNickTextEdit() const;

   MenuPageEditableComboBoxItem* getHostComboBox() const;

   //! history of hostnames
   HostHistory mHistory;
};

#endif // GAMEMENUINTERFACEMAIN_H
