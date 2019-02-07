// header
#include "gamemenuinterfaceselect.h"

// Qt
#include <QTimer>

// game
#include "bombermanclient.h"
#include "levels/level.h"

// menu
#include "gamemenudefines.h"
#include "menu.h"
#include "menupageitem.h"
#include "menupagelabelitem.h"
#include "menupagelistitem.h"
#include "menupagepixmapitem.h"
#include "menupagetextedit.h"

// settings
#include "gamesettings.h"


GameMenuInterfaceSelect::GameMenuInterfaceSelect(QObject* parent)
 : GameMenuInterface(parent),
   mDummyMoveCounter(0),
   mDummyMoveDirection(1)
{
   // prepareUnitTests();
}


void GameMenuInterfaceSelect::gamesListUpdated(const QList<GameInformation>& list)
{
   // const QList<GameInformation> list = generateTestList();

   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_games_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      // init elements
      int selectedGame = -1;
      int focussedElement = -1;

      // store focussed and active element
      focussedElement = tableListItem->getFocussedElement();

      if (mGameMapIndexGameId.contains(tableListItem->getActiveElement()))
      {
         selectedGame =
            mGameMapIndexGameId[tableListItem->getActiveElement()];
      }

      int i = 0;

      tableListItem->clear();

      foreach(GameInformation info, list)
      {
         tableListItem->appendItem(info.getGameName());

         mGameMapIndexGameId.insert(i, info.getId());
         mGameMapGameIdIndex.insert(info.getId(), i);

         i++;
      }

      // restore focussed and active element
      if (mGameMapIndexGameId.contains(tableListItem->getActiveElement()))
      {
         tableListItem->setActiveElement(mGameMapGameIdIndex[selectedGame]);
         tableListItem->setElementActive(
            mGameMapGameIdIndex[selectedGame],
            true
         );
      }

      if (focussedElement < tableListItem->getElements()->count())
      {
         tableListItem->setFocussedElement(focussedElement);
         tableListItem->setElementFocussed(
            focussedElement,
            true
         );
      }
   }
}


void GameMenuInterfaceSelect::updateServerProperties()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      QList<GameInformation>* gameList =
         BombermanClient::getInstance()->getGames();

      // update all relevant information items on the page
      int serverPlayerCount = 0;
      int serverChannelCount = 0;

      // init server-related data
      foreach (const GameInformation& info, *gameList)
         serverPlayerCount += info.getPlayerCount();

      serverChannelCount = gameList->size();

      MenuPageLabelItem* serverChannelCountItem =
         (MenuPageLabelItem*)selectPage->getPageItem("label_server_games");

      MenuPageLabelItem* serverPlayerCountItem =
         (MenuPageLabelItem*)selectPage->getPageItem("label_server_players");

      serverChannelCountItem->setText(QString("%1").arg(serverChannelCount));
      serverPlayerCountItem->setText(QString("%1").arg(serverPlayerCount));
   }
}


void GameMenuInterfaceSelect::updateGameProperties()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      int playerCountPerGame = 0;
      int playerMaximumPerGame = 0;
      bool extraBombEnabled = false;
      bool extraFlameEnabled = false;
      bool extraSpeedUpEnabled = false;
      bool extraKickEnabled = false;
      bool extraSkullEnabled = false;
      int gameDuration = 0;
      QString level;

      int gameId = getSelectedGame();

      int currentRound = 0;
      int roundCount = 0;

      GameInformation* info =
         BombermanClient::getInstance()->getGameInformation(gameId);

      int alpha = 40;

      if (info)
      {
         playerCountPerGame   = info->getPlayerCount();
         playerMaximumPerGame = info->getPlayerMaximumCount();

         extraBombEnabled     = info->getExtras() & Constants::ExtraBomb;
         extraFlameEnabled    = info->getExtras() & Constants::ExtraFlame;
         extraSpeedUpEnabled  = info->getExtras() & Constants::ExtraSpeedup;
         extraKickEnabled     = info->getExtras() & Constants::ExtraKick;
         extraSkullEnabled    = info->getExtras() & Constants::ExtraSkull;

         gameDuration         = info->getDuration();
         level                = info->getLevelName();

         currentRound         = info->getCurrentRound() + 1;
         roundCount           = info->getRoundCount();

         alpha = 255;
      }

      // general
      MenuPageLabelItem* playerCountPerGameItem =
         (MenuPageLabelItem*)selectPage->getPageItem("label_selected_level_players");

      MenuPageLabelItem* gameDurationItem =
         (MenuPageLabelItem*)selectPage->getPageItem("label_selected_level_time");

      playerCountPerGameItem->setText(
            QString("%1/%2")
               .arg(playerCountPerGame)
               .arg(playerMaximumPerGame)
         );

      gameDurationItem->setText(QString("%1").arg(gameDuration));

      // extras
      MenuPageItem* extraBombItem = currentPage->getPageItem("bomb_extra_visible");
      MenuPageItem* extraFlameItem = currentPage->getPageItem("flame_extra_visible");
      MenuPageItem* extraSpeedUpItem = currentPage->getPageItem("speed_extra_visible");
      MenuPageItem* extraKickItem = currentPage->getPageItem("kick_extra_visible");
      MenuPageItem* extraSkullItem = currentPage->getPageItem("skull_extra_visible");

      extraBombItem->setVisible(extraBombEnabled);
      extraFlameItem->setVisible(extraFlameEnabled);
      extraSpeedUpItem->setVisible(extraSpeedUpEnabled);
      extraKickItem->setVisible(extraKickEnabled);
      extraSkullItem->setVisible(extraSkullEnabled);

      // round status
      MenuPageLabelItem* currentRoundItem =
         (MenuPageLabelItem*)selectPage->getPageItem("label_current_round");

      MenuPageLabelItem* roundCountItem =
         (MenuPageLabelItem*)selectPage->getPageItem("label_final_round");

      currentRoundItem->setText(QString("%1").arg(currentRound));
      roundCountItem->setText(QString("%1").arg(roundCount));

      currentRoundItem->setCenterWidth(
         currentRoundItem->getLayer()->getWidth()
      );

      roundCountItem->setCenterWidth(
         roundCountItem->getLayer()->getWidth()
      );

      // enabled/disabled labels
      playerCountPerGameItem->setAlpha(alpha);
      gameDurationItem->setAlpha(alpha);
      currentRoundItem->setAlpha(alpha);
      roundCountItem->setAlpha(alpha);

      // level previews
      MenuPagePixmapItem* previewCastle =
        (MenuPagePixmapItem*)currentPage->getPageItem("pixmap_level_preview_castle");
      MenuPagePixmapItem* previewMansion =
        (MenuPagePixmapItem*)currentPage->getPageItem("pixmap_level_preview_mansion");
      MenuPagePixmapItem* previewPirateship =
        (MenuPagePixmapItem*)currentPage->getPageItem("pixmap_level_preview_pirateship");
      MenuPagePixmapItem* previewSpace =
        (MenuPagePixmapItem*)currentPage->getPageItem("pixmap_level_preview_space");

      previewCastle->setVisible(level == Level::getLevelDirectoryName(Level::LevelCastle));
      previewMansion->setVisible(level == Level::getLevelDirectoryName(Level::LevelMansion));
      previewPirateship->setVisible(false);
      previewSpace->setVisible(level == Level::getLevelDirectoryName(Level::LevelSpace));
   }
}


//-----------------------------------------------------------------------------
/*!
   \return dummy data
*/
QList<GameInformation> GameMenuInterfaceSelect::generateTestList()
{
   if (mDummyList.isEmpty())
   {
      int testCount = 23;
      QString meh = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

      for (int i = 0; i < testCount; i++)
      {
         QString gameName;
         for (int x = 0; x < 8; x++)
            gameName += meh.at(qrand() % meh.length());

         GameInformation tmp(
            i,
            qrand() % 5,   // playercount
            qrand() % 5,   // player maximum count
            gameName,      // game name
            "castle",      // level
            0,             // creator id
            Constants::Dimension13x11,
            Constants::ExtraBomb | Constants::ExtraFlame | Constants::ExtraKick | Constants::ExtraSpeedup,
            qrand() % 360, // duration
            qrand() % 10,  // rounds played
            qrand() % 10,  // current round
            qrand() % 10,
            false
         );

         mDummyList << tmp;
      }
   }

   return mDummyList;
}



//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceSelect::prepareUnitTests()
{
   QTimer::singleShot(
      10000,
      this,
      SLOT(unitTest4())
   );

//   QTimer::singleShot(
//      25000,
//      this,
//      SLOT(unitTest2())
//   );

//   QTimer::singleShot(
//      30000,
//      this,
//      SLOT(unitTest3())
//   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceSelect::unitTest1()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_games_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      tableListItem->scrollSmoothToIndex(22);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceSelect::unitTest2()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_games_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      tableListItem->scrollSmoothToIndex(15);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceSelect::unitTest3()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_games_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      tableListItem->scrollSmoothToIndex(0);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuInterfaceSelect::unitTest4()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_games_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      tableListItem->setFocussedElement(mDummyMoveCounter);

      int y = tableListItem->scrollSmoothToIndex(mDummyMoveCounter);
      qDebug("GameMenuInterfaceSelect::unitTest4(): y: %d", y);

      mDummyMoveCounter += mDummyMoveDirection;

      int timeout = 1000;

      if (mDummyMoveCounter == 0)
      {
         mDummyMoveDirection = 1;
      }
      else if (mDummyMoveCounter == tableListItem->getElementCount() - 1)
      {
         mDummyMoveDirection = -1;
      }

      QTimer::singleShot(timeout, this, SLOT(unitTest4()));
   }
}


//-----------------------------------------------------------------------------
/*!
*/
int GameMenuInterfaceSelect::getSelectedGame()
{
   int selectedGame = -1;

   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* selectPage = Menu::getInstance()->getPageByName(GAME_SELECT);

   if (selectPage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_games_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      if (mGameMapIndexGameId.contains(tableListItem->getActiveElement()))
      {
         selectedGame =
            mGameMapIndexGameId[tableListItem->getActiveElement()];
      }
   }

   return selectedGame;
}

