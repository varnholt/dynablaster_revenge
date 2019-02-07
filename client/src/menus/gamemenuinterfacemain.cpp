// header
#include "gamemenuinterfacemain.h"

// client
#include "bombermanclient.h"

// game
#include "gameversion.h"

// menu
#include "gamemenudefines.h"
#include "menu.h"
#include "menupageeditablecomboboxitem.h"
#include "menupageitem.h"
#include "menupagelabelitem.h"
#include "menupagetextedit.h"

// settings
#include "gamesettings.h"

// Qt
#include <QDesktopServices>
#include <QUrl>


//-----------------------------------------------------------------------------
/*!
   \parent parent object
*/
GameMenuInterfaceMain::GameMenuInterfaceMain(QObject* parent)
  : GameMenuInterface(parent)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::updateLoginData()
{
   QString host = getHostTextEdit()->getText();
   QString nick = getNickTextEdit()->getText();

   GameSettings::getInstance()->getLoginSettings()->setHost(host);
   GameSettings::getInstance()->getLoginSettings()->setNick(nick);

   mHistory.add(host);
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to nick textedit
*/
MenuPageTextEditItem * GameMenuInterfaceMain::getNickTextEdit() const
{
   MenuPage* page = Menu::getInstance()->getPageByName(MAINMENU);

   MenuPageItem* nickItem = page->getPageItem("lineedit_nick");
   MenuPageTextEditItem* nickTextEdit = (MenuPageTextEditItem*)nickItem;

   return nickTextEdit;
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to host combo box
*/
MenuPageEditableComboBoxItem * GameMenuInterfaceMain::getHostComboBox() const
{
   MenuPage* page = Menu::getInstance()->getPageByName(MAINMENU);

   MenuPageItem* hostItem = page->getPageItem("editablecombobox_host_table");

   MenuPageEditableComboBoxItem* hostEditableCombo
      = (MenuPageEditableComboBoxItem*)hostItem;

   return hostEditableCombo;
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to host text edit
*/
MenuPageTextEditItem * GameMenuInterfaceMain::getHostTextEdit() const
{
   MenuPageTextEditItem* hostTextEdit  = getHostComboBox()->getTextEditItem();
   return hostTextEdit;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::deserializeLoginData()
{
   MenuPageEditableComboBoxItem* hostEditableCombo = getHostComboBox();

   hostEditableCombo->clear();

   QString savedHost = GameSettings::getInstance()->getLoginSettings()->getHost();

   QStringList hosts = mHistory.load(savedHost);
   foreach (const QString& host, hosts)
   {
      hostEditableCombo->appendItem(host);
   }

   /*
   hostEditableCombo->appendItem("dynablaster.titandemo.de");
   hostEditableCombo->appendItem("132.234.120.49");
   hostEditableCombo->appendItem("127.0.0.1");
   */

   getNickTextEdit()->setText(
      GameSettings::getInstance()->getLoginSettings()->getNick()
   );

   // there is no use to deserialize the saved host when there's already
   // a valid value set up. this is the case when the game has been started
   // and a valid hostname has been restored. we get here whenever the main
   // menu is opened, so we shouldn't overwrite any input the user made before.
   if (getHostTextEdit()->getText().isEmpty())
      getHostTextEdit()->setText(savedHost);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::updateLedStatus()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(MAINMENU);

   if (currentPage == page)
   {
      MenuPageItem* ledGreen =
         currentPage->getPageItem("led_green");

      MenuPageItem* ledOrange =
         currentPage->getPageItem("led_orange");

      bool hosting = BombermanClient::getInstance()->isHosting();
      bool loggedIn = BombermanClient::getInstance()->isConnected();

      ledGreen->setVisible(loggedIn);
      ledOrange->setVisible(hosting);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::deserializeVersion()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(MAINMENU);

   if (currentPage == page)
   {
      MenuPageLabelItem* version =
         (MenuPageLabelItem*)currentPage->getPageItem("label_version");

      version->setText(tr("version %1, revision %2").arg(GAME_VERSION).arg(GAME_REVISION));
      version->setAlpha(100);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::openPouet()
{
   QString pouet =
      GameSettings::getInstance()->getDevelopmentSettings()->getPagePouet();

   QDesktopServices::openUrl(pouet);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::openFacebook()
{
   QString fb =
      GameSettings::getInstance()->getDevelopmentSettings()->getPageFacebook();

   QDesktopServices::openUrl(fb);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceMain::openHome()
{
   QString home =
      GameSettings::getInstance()->getDevelopmentSettings()->getPageHome();

   QDesktopServices::openUrl(home);
}


