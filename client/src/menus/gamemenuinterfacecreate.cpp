// header
#include "gamemenuinterfacecreate.h"

// game
#include "bombermanclient.h"
#include "levels/level.h"

// menu
#include "gamemenudefines.h"
#include "menu.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupageitem.h"
#include "menupagetextedit.h"
#include "menupagepixmapitem.h"

// settings
#include "gamesettings.h"


GameMenuInterfaceCreate::GameMenuInterfaceCreate(QObject *parent)
 : GameMenuInterface(parent)
{
   mSortedLevelNames.push_back(Level::getLevelName(Level::LevelCastle));
   mSortedLevelNames.push_back(Level::getLevelName(Level::LevelMansion));
   mSortedLevelNames.push_back(Level::getLevelName(Level::LevelSpace));

   mSortedLevelDirNames.push_back(Level::getLevelDirectoryName(Level::LevelCastle));
   mSortedLevelDirNames.push_back(Level::getLevelDirectoryName(Level::LevelMansion));
   mSortedLevelDirNames.push_back(Level::getLevelDirectoryName(Level::LevelSpace));
}


void GameMenuInterfaceCreate::initializeCreateGameOptions()
{
   MenuPage* page = Menu::getInstance()->getPageByName(GAME_CREATE);

   MenuPageItem* time = page->getPageItem("combobox_time_table");
   MenuPageItem* maxPlayers = page->getPageItem("combobox_maxplayers_table");
   MenuPageItem* botCount = page->getPageItem("combobox_bots_table");
   MenuPageItem* levels = page->getPageItem("combobox_level_table");
   MenuPageItem* rounds = page->getPageItem("combobox_rounds_table");
   MenuPageItem* bombExtras = page->getPageItem("checkbox_bomb");
   MenuPageItem* flameExtras= page->getPageItem("checkbox_flame");
   MenuPageItem* speedUpExtras = page->getPageItem("checkbox_speedup");
   MenuPageItem* kickExtras = page->getPageItem("checkbox_kick");
   MenuPageItem* skullsExtras = page->getPageItem("checkbox_skull");

   MenuPageComboBoxItem* timeCombo = dynamic_cast<MenuPageComboBoxItem*>(time);
   MenuPageComboBoxItem* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(maxPlayers);
   MenuPageComboBoxItem* botCountCombo = dynamic_cast<MenuPageComboBoxItem*>(botCount);
   MenuPageComboBoxItem* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(levels);
   MenuPageComboBoxItem* roundsCombo = dynamic_cast<MenuPageComboBoxItem*>(rounds);

   MenuPageCheckBoxItem* bombExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(bombExtras);
   MenuPageCheckBoxItem* flameExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(flameExtras);
   MenuPageCheckBoxItem* speedUpExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(speedUpExtras);
   MenuPageCheckBoxItem* kickExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(kickExtras);
   MenuPageCheckBoxItem* skullsExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(skullsExtras);

   if (!mPagesInitialized.contains(page))
   {
      timeCombo->appendItem(tr("2"));
      timeCombo->appendItem(tr("3"));
      timeCombo->appendItem(tr("5"));
      timeCombo->appendItem(tr("7"));

      for (int i = 0; i < 5; i++)
      {
         roundsCombo->appendItem(QString("%1").arg(i+1));
      }

      for (int i = 2; i <= 10; i++)
      {
         maxPlayersCombo->appendItem(QString("%1").arg(i));
      }

      foreach (const QString& name, mSortedLevelNames)
         levelCombo->appendItem(name);

      // levelCombo->appendItem("the lab");
      // levelCombo->appendItem("the pirateship");

      mPagesInitialized.insert(page);
   }

   GameSettings::CreateGameSettings* cgs = 0;

   if (BombermanClient::getInstance()->isSinglePlayer())
      cgs = GameSettings::getInstance()->getCreateGameSettingsSingle();
   else
      cgs = GameSettings::getInstance()->getCreateGameSettingsMulti();

   // deserialize create game settings
   timeCombo->setValue(QString("%1").arg(cgs->getDuration()));
   maxPlayersCombo->setValue(QString("%1").arg(cgs->getMaxPlayers()));
   botCountCombo->setValue(QString("%1").arg(cgs->getBotCount()));

   int levelIndex = cgs->getLevelIndex();
   if (levelIndex >= mSortedLevelNames.size())
      levelIndex = 0;

   // set the level's name
   levelCombo->setValue(mSortedLevelNames[levelIndex]);
   levelCombo->setActiveElement(levelIndex);

   // monitoring is not enabled yet, so we can do this safely
   levelCombo->setFocussedElement(levelIndex);

   roundsCombo->setValue(QString("%1").arg(cgs->getRounds()));
   bombExtrasCb->setChecked(cgs->isExtraBombsEnabled());
   flameExtrasCb->setChecked(cgs->isExtraFlamesEnabled());
   speedUpExtrasCb->setChecked(cgs->isExtraSpeedUpsEnabled());
   kickExtrasCb->setChecked(cgs->isExtraKicksEnabled());
   skullsExtrasCb->setChecked(cgs->isExtraSkullsEnabled());

   // limit bot count combo to max player count
   updateCreateGamePlayerCounts();

   // update level preview
   updateCreateGameLevelPreview();
}


void GameMenuInterfaceCreate::updateCreateGamePlayerCounts()
{
   MenuPage* page = Menu::getInstance()->getPageByName(GAME_CREATE);

   // check max players vs. bot count
   int maxPlayers = 0;
   int botCount = 0;

   MenuPageItem* maxPlayersItem = page->getPageItem("combobox_maxplayers_table");
   MenuPageItem* botCountItem = page->getPageItem("combobox_bots_table");

   MenuPageComboBoxItem* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(maxPlayersItem);
   MenuPageComboBoxItem* botCountCombo = dynamic_cast<MenuPageComboBoxItem*>(botCountItem);

   maxPlayers = maxPlayersCombo->getValue().toInt();
   botCount = botCountCombo->getValue().toInt();

   int maxBots = maxPlayers - 1;

   botCountCombo->clear();

   for (int i = 0; i <= maxBots; i++)
      botCountCombo->appendItem(QString("%1").arg(i));

   if (botCount > maxBots)
   {
      botCountCombo->setValue(QString("%1").arg(maxBots));
   }
}


void GameMenuInterfaceCreate::updateCreateGameLevelPreview()
{
   MenuPage* page = Menu::getInstance()->getPageByName(GAME_CREATE);

   MenuPagePixmapItem* previewCastle =
     (MenuPagePixmapItem*)page->getPageItem("pixmap_preview_castle");
   MenuPagePixmapItem* previewMansion =
     (MenuPagePixmapItem*)page->getPageItem("pixmap_preview_mansion");
   MenuPagePixmapItem* previewSpace =
     (MenuPagePixmapItem*)page->getPageItem("pixmap_preview_space");

   MenuPageItem* levelsItem = page->getPageItem("combobox_level_table");
   MenuPageComboBoxItem* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(levelsItem);

   previewCastle->setVisible(levelCombo->getFocussedElement() <= 0);
   previewMansion->setVisible(levelCombo->getFocussedElement() == 1);
   previewSpace->setVisible(levelCombo->getFocussedElement() == 2);

//   previewCastle->setVisible(levelCombo->getActiveElement() <= 0);
//   previewMansion->setVisible(levelCombo->getActiveElement() == 1);
//   previewPirateship->setVisible(levelCombo->getActiveElement() == 2);

}


void GameMenuInterfaceCreate::setMonitorCreateGameOptionsEnabled(bool enabled)
{
   MenuPage* page = Menu::getInstance()->getPageByName(GAME_CREATE);

   // player and bot count vs. number of players allowed
   MenuPageItem* maxPlayersItem = page->getPageItem("combobox_maxplayers_table");
   MenuPageComboBoxItem* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(maxPlayersItem);

   if (enabled)
   {
      connect(
         maxPlayersCombo,
         SIGNAL(valueChanged(QString)),
         this,
         SLOT(updateCreateGamePlayerCounts())
      );
   }
   else
   {
      disconnect(
         maxPlayersCombo,
         SIGNAL(valueChanged(QString)),
         this,
         SLOT(updateCreateGamePlayerCounts())
      );
   }

   // level selection
   MenuPageItem* levelItem = page->getPageItem("combobox_level_table");
   MenuPageComboBoxItem* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(levelItem);

   if (enabled)
   {
      connect(
         levelCombo,
         SIGNAL(valueChanged(QString)),
         this,
         SLOT(updateCreateGameLevelPreview())
      );

      connect(
         levelCombo,
         SIGNAL(elementFocussed(int)),
         this,
         SLOT(updateCreateGameLevelPreview())
      );
   }
   else
   {
      disconnect(
         levelCombo,
         SIGNAL(valueChanged(QString)),
         this,
         SLOT(updateCreateGameLevelPreview())
      );

      disconnect(
         levelCombo,
         SIGNAL(elementFocussed(int)),
         this,
         SLOT(updateCreateGameLevelPreview())
      );
   }
}


void GameMenuInterfaceCreate::deserializeCreateGameData()
{
   // extract information from ui
   MenuPage* page = Menu::getInstance()->getPageByName(GAME_CREATE);
   MenuPageItem* gameNameItem = page->getPageItem("lineedit_name");

   ((MenuPageTextEditItem*)gameNameItem)->setText(
      GameSettings::getInstance()->getCreateGameSettingsSingle()->getGameName()
   );
}


void GameMenuInterfaceCreate::createGame()
{
   // init
   QString gameName;
   QString levelDirName;
   int rounds = 1;
   int durationSeconds = 0;
   int durationMinutes = 0;
   int maxPlayers = 0;
   int botCount = 0;
   bool extraBombs = false;
   bool extraFlames = false;
   bool extraSpeedUps = false;
   bool extraKicks = false;
   bool extraSkulls = false;
   Constants::Dimension dimension = Constants::DimensionInvalid;

   // extract information from ui
   MenuPage* page = Menu::getInstance()->getPageByName(GAME_CREATE);

   // read items
   MenuPageItem* gameNameItem = page->getPageItem("lineedit_name");
   MenuPageItem* timeItem = page->getPageItem("combobox_time_table");
   MenuPageItem* maxPlayersItem = page->getPageItem("combobox_maxplayers_table");
   MenuPageItem* botCountItem = page->getPageItem("combobox_bots_table");
   MenuPageItem* levelsItem = page->getPageItem("combobox_level_table");
   MenuPageItem* roundsItem = page->getPageItem("combobox_rounds_table");

   MenuPageComboBoxItem* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(levelsItem);
   MenuPageComboBoxItem* roundsCombo = dynamic_cast<MenuPageComboBoxItem*>(roundsItem);

   MenuPageItem* bombExtras = page->getPageItem("checkbox_bomb");
   MenuPageItem* flameExtras= page->getPageItem("checkbox_flame");
   MenuPageItem* speedUpExtras = page->getPageItem("checkbox_speedup");
   MenuPageItem* kickExtras = page->getPageItem("checkbox_kick");
   MenuPageItem* skullsExtras = page->getPageItem("checkbox_skull");

   MenuPageCheckBoxItem* bombExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(bombExtras);
   MenuPageCheckBoxItem* flameExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(flameExtras);
   MenuPageCheckBoxItem* speedUpExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(speedUpExtras);
   MenuPageCheckBoxItem* kickExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(kickExtras);
   MenuPageCheckBoxItem* skullsExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(skullsExtras);

   // evaluate items
   gameName = ((MenuPageTextEditItem*)gameNameItem)->getText();

   int levelIndex = levelCombo->getActiveElement();
   if (levelIndex >= mSortedLevelDirNames.size())
      levelDirName = mSortedLevelDirNames[0];
   else
      levelDirName = mSortedLevelDirNames[levelIndex];

   durationMinutes = dynamic_cast<MenuPageComboBoxItem*>(timeItem)->getValue().toInt();
   durationSeconds = durationMinutes * 60;
   maxPlayers = dynamic_cast<MenuPageComboBoxItem*>(maxPlayersItem)->getValue().toInt();
   botCount = dynamic_cast<MenuPageComboBoxItem*>(botCountItem)->getValue().toInt();
   rounds = dynamic_cast<MenuPageComboBoxItem*>(roundsCombo)->getValue().toInt();

   extraBombs = bombExtrasCb->isChecked();
   extraFlames = flameExtrasCb->isChecked();
   extraSpeedUps = speedUpExtrasCb->isChecked();
   extraKicks = kickExtrasCb->isChecked();
   extraSkulls = skullsExtrasCb->isChecked();

   dimension =
      (maxPlayers <= 5)
         ? Constants::Dimension13x11
         : Constants::Dimension19x17;

   qDebug(
      "GameMenuInterfaceCreate::createGame():\n"
      "   game name: %s\n"
      "   level: %s (%d)\n"
      "   rounds: %d\n"
      "   duration: %d\n"
      "   max players: %d\n"
      "   extra bombs: %d\n"
      "   extra flames: %d\n"
      "   extra speedups: %d\n"
      "   extra kicks: %d\n"
      "   extra skulls: %d\n"
      "   dimension: %d\n"
      "   bots: %d",
      qPrintable(gameName),
      qPrintable(levelDirName),
      levelCombo->getActiveElement(),
      rounds,
      durationSeconds,
      maxPlayers,
      extraBombs,
      extraFlames,
      extraSpeedUps,
      extraKicks,
      extraSkulls,
      dimension,
      botCount
   );

   // update settings
   GameSettings::CreateGameSettings* cgs = 0;

   if (BombermanClient::getInstance()->isSinglePlayer())
      cgs = GameSettings::getInstance()->getCreateGameSettingsSingle();
   else
      cgs = GameSettings::getInstance()->getCreateGameSettingsMulti();

   cgs->setGameName(gameName);
   cgs->setLevelIndex(levelIndex);
   cgs->setRounds(rounds);
   cgs->setDuration(durationMinutes);
   cgs->setMaxPlayers(maxPlayers);
   cgs->setExtraBombsEnabled(extraBombs);
   cgs->setExtraFlamesEnabled(extraFlames);
   cgs->setExtraKicksEnabled(extraKicks);
   cgs->setExtraSpeedUpsEnabled(extraSpeedUps);
   cgs->setExtraSkullsEnabled(extraSkulls);
   cgs->setDimensions(dimension);
   cgs->setBotCount(botCount);

   // and store them
   cgs->serialize();

   BombermanClient::getInstance()->createGame(
      gameName,
      levelDirName,
      rounds,
      durationSeconds,
      maxPlayers,
      extraBombs,
      extraFlames,
      extraSpeedUps,
      extraKicks,
      extraSkulls,
      dimension
   );
}
