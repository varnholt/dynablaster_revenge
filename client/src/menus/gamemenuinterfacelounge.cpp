// header
#include "gamemenuinterfacelounge.h"

// game
#include "bombermanclient.h"
#include "gamejoystickintegration.h"
#include "headlessintegration.h"
#include "wordwrap.h"

// joystick
#include "joystickinterface.h"

// menu
#include "gamemenudefines.h"
#include "menu.h"
#include "menupagebuttonitem.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupageitem.h"
#include "menupagelabelitem.h"
#include "menupagetextedit.h"
#include "menupagepixmapitem.h"

// settings
#include "gamesettings.h"

GameMenuInterfaceLounge::GameMenuInterfaceLounge(QObject *parent)
  : GameMenuInterface(parent)
{
}


void GameMenuInterfaceLounge::clearLoungeMessage()
{
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);
   MenuPageItem* sayItem = page->getPageItem(LOUNGE_LINEEDIT_SAY);
   MenuPageTextEditItem* sayTextEdit = (MenuPageTextEditItem*)sayItem;
   sayTextEdit->setText("");
}


void GameMenuInterfaceLounge::clearLoungeMessages()
{
   MenuPage* loungePage = Menu::getInstance()->getPageByName(LOUNGE);
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();

   if (loungePage == currentPage)
   {
      MenuPageItem* table = currentPage->getPageItem("table_lounge_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;
      tableListItem->clear();
   }
}


QString GameMenuInterfaceLounge::getLoungeMessage() const
{
   QString message;

   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   MenuPageItem* sayItem = page->getPageItem(LOUNGE_LINEEDIT_SAY);
   MenuPageTextEditItem* sayTextEdit = (MenuPageTextEditItem*)sayItem;

   message = sayTextEdit->getText();

   return message;
}


void GameMenuInterfaceLounge::addLoungeMessage(
   int senderId,
   QString message
)
{
   MenuPage* loungePage = Menu::getInstance()->getPageByName(LOUNGE);
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();

   QString nick;

   if (loungePage == currentPage)
   {
      MenuPageItem* sayItem = loungePage->getPageItem(LOUNGE_LINEEDIT_SAY);
      MenuPageTextEditItem* sayTextEdit = (MenuPageTextEditItem*)sayItem;

      int startIndex = message.indexOf(": ");
      if (startIndex != -1)
         nick = message.left(startIndex);

      Constants::Color playerColor =BombermanClient::getInstance()->getColor(senderId);
      QColor color = GameSettings::getInstance()->getStyleSettings()->getColor(playerColor);
      QColor outlineColor = QColor(0, 0, 0, 255);

      if (playerColor == Constants::ColorBlack)
      {
         color = QColor(128, 128, 128, 255);
         // outlineColor = QColor(192, 192, 192, 255);
      }

      // qDebug("chosen color is: %s", qPrintable(color.name()));

      MenuPageItem* table = currentPage->getPageItem("table_lounge_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;

      QStringList lines =
         WordWrap::wrap(
            message,
            sayTextEdit->getFieldWidth()
         );

      int i = 0;
      QString text;
      QString trimmed;
      foreach (const QString line, lines)
      {
         if (i == 0)
         {
            text = line;
         }
         else
         {
            if (nick.isEmpty())
               text = line;
            else
               text = QString("%1: %2").arg(nick).arg(line);
         }

         trimmed = line.trimmed();
         if (
               trimmed != QString("%1:").arg(nick)
            && !trimmed.isEmpty()
         )
         {
            tableListItem->appendItem(text, color, true, outlineColor);
         }

         i++;
      }

      // scroll down
      tableListItem->scrollToPercentage(100.0f, false);
   }
}


void GameMenuInterfaceLounge::updatePlayerTyping(int senderId, bool typingFinished)
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   if (currentPage == page)
   {
      if (mPlayerIdToIndexMap.contains(senderId))
      {
         int index = mPlayerIdToIndexMap[senderId];

         MenuPageItem* typingBoxItem
            = currentPage->getPageItem(QString("p%1_box_type").arg(index));

         if (typingBoxItem)
            typingBoxItem->setVisible(!typingFinished);

         // remove typing icon after a while
         if (!typingFinished)
         {
            if (!mPlayersTyping.contains(index))
            {
               QTimer* timer = new QTimer();
               timer->start(6000);
               timer->setProperty("index", index);

               mPlayersTyping.insert(index, timer);

               connect(
                  timer,
                  SIGNAL(timeout()),
                  this,
                  SLOT(removePlayerTyping())
               );
            }
            else
            {
               QTimer* timer = mPlayersTyping[index];
               timer->start(6000);
            }
         }
      }
   }
}


void GameMenuInterfaceLounge::removePlayerTyping()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   if (currentPage == page)
   {
      QTimer* timer = dynamic_cast<QTimer*>(sender());

      int index = timer->property("index").toInt();
      mPlayersTyping.take(index)->deleteLater();

      MenuPageItem* typingBoxItem
         = currentPage->getPageItem(QString("p%1_box_type").arg(index));

      if (typingBoxItem)
      {
         typingBoxItem->setVisible(false);
      }
   }
}


void GameMenuInterfaceLounge::playerInfoMapUpdated(QMap<int, PlayerInfo*>* playerInfo)
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   if (currentPage == page)
   {
      mPlayerIdToIndexMap.clear();

      QMapIterator<int, PlayerInfo*> it(*playerInfo);
      int counter = 0;

      // sort players by score
      QList< QPair<PlayerInfo*, int> > scoreList;
      while (it.hasNext())
      {
         it.next();
         scoreList.append(
            QPair<PlayerInfo*, int>(it.value(), it.value()->getOverallStats().getWins())
         );
      }

      qSort(scoreList.begin(), scoreList.end(), QPairSecondComparer());
      PlayerInfo* player = 0;
      int score = 0;

      MenuPageLabelItem* nickItem = 0;
      MenuPageLabelItem* winsItem = 0;
      MenuPageItem* activeBoxItem = 0;
      MenuPageItem* ownerItem = 0;
      MenuPageItem* playerItem = 0;
      MenuPageItem* rankItem = 0;
      bool owner = false;
      int color = 0;

      // initially hide all
      for (int i = 1; i <= 10; i++)
      {
         nickItem
            = (MenuPageLabelItem*)currentPage->getPageItem(QString("label_p%1").arg(i));

         activeBoxItem
            = currentPage->getPageItem(QString("p%1_box_active").arg(i));

         ownerItem =
            currentPage->getPageItem(QString("p%1_leader_icon").arg(i));

         playerItem =
            currentPage->getPageItem(QString("p%1_icon").arg(i));

         rankItem =
            currentPage->getPageItem(QString("label_rank_%1").arg(i));

         winsItem =
            (MenuPageLabelItem*)currentPage->getPageItem(QString("label_p%1_wins").arg(i));

         nickItem->setText("");
         activeBoxItem->setVisible(false);
         ownerItem->setVisible(false);
         playerItem->setVisible(false);
         rankItem->setVisible(false);
         winsItem->setVisible(false);
      }

      for (int i = 0; i < scoreList.size(); i++)
      {
         counter++;

         player = scoreList[i].first;
         score = scoreList[i].second;

         color = (int)player->getColor();
         owner = (
               player->getId()
            == BombermanClient::getInstance()->getCurrentGameInformation()->getCreatorId()
         );

         nickItem =
            (MenuPageLabelItem*)currentPage->getPageItem(
               QString("label_p%1").arg(counter)
            );

         winsItem =
            (MenuPageLabelItem*)currentPage->getPageItem(
               QString("label_p%1_wins").arg(counter)
            );

         rankItem =
            currentPage->getPageItem(
               QString("label_rank_%1").arg(counter)
            );

         activeBoxItem =
            currentPage->getPageItem(QString("p%1_box_active").arg(counter));

         ownerItem =
            currentPage->getPageItem(QString("p%1_leader_icon").arg(counter));

         playerItem =
            currentPage->getPageItem(QString("p%1_icon").arg(color));

         playerItem->getCurrentLayer()->setY(
            activeBoxItem->getCurrentLayer()->getTop() - 4
         );

         mPlayerIdToIndexMap.insert(player->getId(), counter);

         winsItem->setText(QString("%1").arg(score));
         winsItem->setColor((i < 3) ? "#fbfe00" : "#3b7d9d");

         nickItem->setText(player->getNick());
         activeBoxItem->setVisible(true);
         ownerItem->setVisible(owner);
         winsItem->setVisible(true);
         playerItem->setVisible(true);
         rankItem->setVisible(true);
      }
   }
}


void GameMenuInterfaceLounge::gameInfoUpdated()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   if (currentPage == page)
   {
      MenuPageButtonItem* startButton =
         dynamic_cast<MenuPageButtonItem*>(
            currentPage->getPageItem("button_start")
         );

      MenuPageButtonItem* readyButton =
         dynamic_cast<MenuPageButtonItem*>(
            currentPage->getPageItem("button_ready")
         );

      MenuPageLabelItem* roundsItem =
         (MenuPageLabelItem*)currentPage->getPageItem(
            QString("label_games_counter")
         );

      MenuPageItem* roundsPlayedCenterItem =
         currentPage->getPageItem("boundinct_rect_after_n_matches");

      bool owner = BombermanClient::getInstance()->isPlayerOwner();

      if (startButton)
         startButton->setVisible(owner);

      if (readyButton)
         readyButton->setVisible(!owner);

      if (roundsItem)
      {
         GameInformation* info =
            BombermanClient::getInstance()->getCurrentGameInformation();

         if (info)
         {
            roundsItem->setCenterWidth(
               roundsPlayedCenterItem->getLayer()->getWidth()
            );

            roundsItem->setText(
               QString("%1").arg(info->getGamesPlayed())
            );
         }
      }
   }
}


void GameMenuInterfaceLounge::initializeLoungeStates()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   if (currentPage == page)
   {
      for (int i = 1; i <= 10; i++)
      {
         MenuPageItem* activeBoxItem
            = currentPage->getPageItem(QString("p%1_box_active").arg(i));

         if (activeBoxItem)
            activeBoxItem->setVisible(false);

         MenuPageItem* typingBoxItem
            = currentPage->getPageItem(QString("p%1_box_type").arg(i));

         if (typingBoxItem)
            typingBoxItem->setVisible(false);
      }

      // set player nick
      /*
       *
      17:12 <dstar> oh and remove the player name in lounge.psd

      MenuPageLabelItem* nick =
         (MenuPageLabelItem*)page->getPageItem("label_players_name");

      nick->setText(
         BombermanClient::getInstance()->getCurrentPlayerInfo()->getNick()
      );
      */

      // the lounge shall not be highlighted, scroll down
      MenuPageItem* table = currentPage->getPageItem("table_lounge_main");
      MenuPageListItem* tableListItem = (MenuPageListItem*)table;
      tableListItem->setHighlightingEnabled(false);
      tableListItem->setRowAlphas(0,0);
      tableListItem->scrollToPercentage(100.0f, false);
   }
}


void GameMenuInterfaceLounge::setLoungeConnectionsEnabled(bool enabled)
{
   if (enabled)
   {
      connect(
         BombermanClient::getInstance(),
         SIGNAL(gamesListUpdated(const QList<GameInformation>&)),
         this,
         SLOT(gameInfoUpdated())
      );
   }
   else
   {
      disconnect(
         BombermanClient::getInstance(),
         SIGNAL(gamesListUpdated(const QList<GameInformation>&)),
         this,
         SLOT(gameInfoUpdated())
      );
   }
}


void GameMenuInterfaceLounge::initializeLastPlayerMessage()
{
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);
   MenuPageItem* sayItem = page->getPageItem(LOUNGE_LINEEDIT_SAY);
   MenuPageTextEditItem* sayTextEdit = (MenuPageTextEditItem*)sayItem;

   // copy buffer from client to textedit
   QString message = BombermanClient::getInstance()->getMessage();

   if (!message.isEmpty())
   {
      sayTextEdit->setText(
         message
      );

      sayTextEdit->setFocus(true);
      sayTextEdit->setActive(true);
   }

   // clear old message
   // BombermanClient::getInstance()->setMessage("");
}


void GameMenuInterfaceLounge::updateAddHeadlessPlayerButton()
{
   bool enabled = false;

   // check if add headless can be successful
   int joystickCount = 0;

   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);

   if (gji)
   {
      joystickCount = gji->getJoystickInterface()->getJoystickCount();
   }

   int instanceCount = HeadlessIntegration::getInstanceCount();

   if (
         instanceCount < 8
      && instanceCount < joystickCount
      && BombermanClient::getInstance()->getGameId() != -1
   )
   {
      enabled = true;
   }

   // lookup button and set it to enabled/disabled
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(LOUNGE);

   if (currentPage == page)
   {
      MenuPageButtonItem* addPlayerButton =
         dynamic_cast<MenuPageButtonItem*>(
            currentPage->getPageItem("button_addplayer")
         );

      if (addPlayerButton)
      {
         addPlayerButton->setEnabled(enabled);
      }
   }
}

