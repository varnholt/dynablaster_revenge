// header
#include "gamemenuinterfacecredits.h"

// menu
#include "gamemenudefines.h"
#include "menu.h"
#include "menupagescrollimageitem.h"


GameMenuInterfaceCredits::GameMenuInterfaceCredits(QObject *parent)
 : GameMenuInterface(parent)
{
}


void GameMenuInterfaceCredits::reset()
{
   // reset scroller
   MenuPage* page = Menu::getInstance()->getPageByName(ABOUT);
   MenuPageItem* gameNameItem = page->getPageItem("image_scroll_credits");

   ((MenuPageScrollImageItem*)gameNameItem)->reset();
}
