// header
#include "gamemenuinterface.h"

// menus
#include "gamemenudefines.h"
#include "menu.h"
#include "menupagebackgrounditem.h"


GameMenuInterface::GameMenuInterface(QObject *parent)
 : MenuInterface(parent)
{
}


void GameMenuInterface::updateBackgroundColor(const QString & /*pageName*/)
{
   MenuPage* bg = Menu::getInstance()->getPageByName(BACKGROUND);

   MenuPageBackgroundItem* bgi =
      dynamic_cast<MenuPageBackgroundItem*>(bg->getPageItems()->first());

   if (bgi)
   {
      QString currentName = Menu::getInstance()->getCurrentPage()->getFilename();

      MenuPageBackgroundItem::BackgroundColor color =
         MenuPageBackgroundItem::BackgroundColorBlue;

      if (currentName == GAME_SELECT)
      {
         color = MenuPageBackgroundItem::BackgroundColorGreen;
      }
      else if (currentName == GAME_CREATE)
      {
         color = MenuPageBackgroundItem::BackgroundColorGreen;
      }
      else if (currentName == LOUNGE)
      {
         color = MenuPageBackgroundItem::BackgroundColorRed;
      }

      bgi->setBackgroundColor(color);
   }
}


