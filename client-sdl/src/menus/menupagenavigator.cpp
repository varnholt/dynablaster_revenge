#include "menupagenavigator.h"

#include "game/bombermanclient.h"
#include "game/gamesettings.h"
#include "levels/level.h"
#include "gameinformation.h"

#include "menu.h"
#include "menupage.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupagepixmapitem.h"
#include "menupagetextedit.h"

#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>

namespace
{

// Mirrors client/src/menus/gamemenudefines.h's page/action string constants. Not #include-d
// directly - that header also defines networking-only action names this class deliberately
// doesn't handle, and pulling it in would imply more coverage than actually exists here.
const char* const kMainMenu = "data/menus/mainmenu.psd";
const char* const kMainMenuActionOptions = "button_options_active";
const char* const kMainMenuActionAbout = "button_about_active";
const char* const kMainMenuActionQuit = "button_quit_active";

const char* const kGameSelect = "data/menus/selectgame.psd";
const char* const kGameSelectActionCreate = "button_create_active";
const char* const kGameSelectActionBack = "button_back_active";

const char* const kGameCreate = "data/menus/creategame.psd";
const char* const kGameCreateActionCancel = "button_cancel_active";

const char* const kOptionsVideo = "data/menus/options_video.psd";
const char* const kOptionsAudio = "data/menus/options_audio.psd";
const char* const kOptionsControls = "data/menus/options_controls.psd";
const char* const kOptionsGame = "data/menus/options_game.psd";
const char* const kOptionsActionOk = "button_ok_active";
const char* const kOptionsActionCancel = "button_cancel_active";
const char* const kOptionsActionVideo = "button_video_active";
const char* const kOptionsActionAudio = "button_audio_active";
const char* const kOptionsActionControls = "button_controls_active";
const char* const kOptionsActionGame = "button_game_active";

const char* const kAbout = "data/menus/about.psd";
const char* const kAboutActionBack = "button_back_active";
const char* const kLounge = "data/menus/lounge.psd";
const char* const kLoungeActionStart = "button_start_active";
const char* const kLoungeActionBack = "button_leave_active";
const char* const kLoungeActionAddPlayer = "button_addplayer_active";
const char* const kLoungeLineeditSay = "lineedit_say";

const char* const kMainMenuActionSingle = "button_single_active";
const char* const kMainMenuActionMulti = "button_multi_active";
const char* const kMainMenuActionPouet = "button_pouet_active";
const char* const kMainMenuActionFacebook = "button_facebook_active";
const char* const kMainMenuActionHome = "button_home_active";
const char* const kGameSelectActionJoin = "button_join_active";
const char* const kGameCreateActionOk = "button_ok_active";

bool isOptionsPage(const QString& page)
{
   return page == kOptionsVideo || page == kOptionsAudio || page == kOptionsControls || page == kOptionsGame;
}

// matches GameMenuWorkflow::isHostLocal() - true if the configured host resolves to one of this
// machine's own network interfaces, in which case MULTI also hosts an in-process server (same as
// SINGLE always does), rather than only connecting out to a remote one.
bool isHostLocal(const QString& hostName)
{
   QHostAddress hostAddress(hostName);

   for (const QHostAddress& address : QNetworkInterface::allAddresses())
   {
      if (hostAddress == address)
         return true;
   }

   return false;
}

bool needsBrowser(const QString& action)
{
   return action == kMainMenuActionPouet || action == kMainMenuActionFacebook || action == kMainMenuActionHome;
}

void logUnhandled(const QString& page, const QString& action)
{
   if (action.isEmpty())
      return;

   if (needsBrowser(action))
      qDebug("MenuPageNavigator: page=%s action=%s needs opening an external browser (not implemented)", qPrintable(page), qPrintable(action));
   // else: not a real menu action (e.g. an editablecombobox's own internal layer-name emission) -
   // the real GameMenuWorkflow doesn't log these either, so neither do we.
}

}  // namespace

MenuPageNavigator::MenuPageNavigator(QObject* parent) : QObject(parent)
{
   // matches GameMenuInterfaceCreate's constructor.
   mSortedLevelNames.push_back(Level::getLevelName(Level::LevelCastle));
   mSortedLevelNames.push_back(Level::getLevelName(Level::LevelMansion));
   mSortedLevelNames.push_back(Level::getLevelName(Level::LevelSpace));

   mSortedLevelDirNames.push_back(Level::getLevelDirectoryName(Level::LevelCastle));
   mSortedLevelDirNames.push_back(Level::getLevelDirectoryName(Level::LevelMansion));
   mSortedLevelDirNames.push_back(Level::getLevelDirectoryName(Level::LevelSpace));

   // BombermanClient must already be constructed+initialize()'d by main.cpp before this runs -
   // getInstance() doesn't self-construct (matches the real client/src/game/bombermanclientgui.cpp
   // construction order).
   connect(BombermanClient::getInstance(), SIGNAL(loginResponse(bool)), this, SLOT(onLoginResponse(bool)));
   connect(BombermanClient::getInstance(), SIGNAL(createGameResponse(bool, int, bool)), this, SLOT(onCreateGameResponse(bool, int, bool)));
   connect(BombermanClient::getInstance(), SIGNAL(joinGameResponse(bool)), this, SLOT(onJoinGameResponse(bool)));
   connect(BombermanClient::getInstance(), SIGNAL(gameStarted()), this, SLOT(onGameStarted()));
}

void MenuPageNavigator::onActionRequest(const QString& page, const QString& action)
{
   if (page == kMainMenu)
   {
      if (action == kMainMenuActionOptions)
         emit pageChangeRequest(kOptionsVideo);
      else if (action == kMainMenuActionAbout)
         emit pageChangeRequest(kAbout);
      else if (action == kMainMenuActionQuit)
         emit quitRequest();
      else if (action == kMainMenuActionSingle)
      {
         // matches GameMenuWorkflow's MAINMENU_ACTION_SINGLE handler exactly: single player
         // always hosts an in-process server and logs into it over loopback.
         BombermanClient::getInstance()->setGameMode(Constants::GameModeSinglePlayer);
         BombermanClient::getInstance()->host();
         BombermanClient::getInstance()->loginRequest("127.0.0.1", GameSettings::getInstance()->getLoginSettings()->getNick());
      }
      else if (action == kMainMenuActionMulti)
      {
         // matches GameMenuWorkflow's MAINMENU_ACTION_MULTI handler: only hosts if the
         // configured host is actually this machine, always logs in to whatever host is set.
         BombermanClient::getInstance()->setGameMode(Constants::GameModeMultiPlayer);

         const QString host = GameSettings::getInstance()->getLoginSettings()->getHost();

         if (isHostLocal(host))
            BombermanClient::getInstance()->host();

         BombermanClient::getInstance()->loginRequest(host, GameSettings::getInstance()->getLoginSettings()->getNick());
      }
      else
         logUnhandled(page, action);
   }
   else if (page == kGameSelect)
   {
      if (action == kGameSelectActionCreate)
         emit pageChangeRequest(kGameCreate);
      else if (action == kGameSelectActionBack)
         emit pageChangeRequest(kMainMenu);
      else if (action == kGameSelectActionJoin)
      {
         // the real GAME_SELECT_ACTION_JOIN reads the operator-selected row out of the game
         // table (GameMenuInterfaceSelect::getSelectedGame()) - that table selection isn't wired
         // in this port yet, so this joins the first known game instead. Real enough to prove the
         // join->lounge chain; picking a specific game is a later refinement.
         const QList<GameInformation>* games = BombermanClient::getInstance()->getGames();
         if (games && !games->isEmpty())
            BombermanClient::getInstance()->joinGame(games->first().getId());
         else
            qDebug("MenuPageNavigator: JOIN clicked with no games known yet");
      }
      else
         logUnhandled(page, action);
   }
   else if (page == kGameCreate)
   {
      if (action == kGameCreateActionCancel)
      {
         // the real GameMenuWorkflow picks GAME_SELECT vs MAINMENU here based on
         // BombermanClient::getGameMode() (single- vs multiplayer) - that state doesn't exist in
         // this port yet, so this always goes back to GAME_SELECT, the multiplayer flow's own
         // back target and the only way to have reached GAME_CREATE at all right now.
         emit pageChangeRequest(kGameSelect);
      }
      else if (action == kGameCreateActionOk)
      {
         createGame();
      }
      else
      {
         logUnhandled(page, action);
      }
   }
   else if (isOptionsPage(page))
   {
      if (action == kOptionsActionOk || action == kOptionsActionCancel)
      {
         // the real workflow also stores/restores option values here (via
         // mGameMenuInterfaceOptions) - not ported, since there's no persisted GameSettings
         // backing these controls in this port yet. Just navigates back.
         emit pageChangeRequest(kMainMenu);
      }
      else if (action == kOptionsActionVideo)
         emit pageChangeRequest(kOptionsVideo);
      else if (action == kOptionsActionAudio)
         emit pageChangeRequest(kOptionsAudio);
      else if (action == kOptionsActionControls)
         emit pageChangeRequest(kOptionsControls);
      else if (action == kOptionsActionGame)
         emit pageChangeRequest(kOptionsGame);
      else
         logUnhandled(page, action);
   }
   else if (page == kAbout)
   {
      if (action == kAboutActionBack)
         emit pageChangeRequest(kMainMenu);
      else
         logUnhandled(page, action);
   }
   else if (page == kLounge)
   {
      if (action == kLoungeActionStart)
      {
         BombermanClient::getInstance()->startGame(BombermanClient::getInstance()->getGameId());
      }
      else if (action == kLoungeActionBack)
      {
         BombermanClient::getInstance()->leaveGameRequest();
         emit pageChangeRequest(kGameSelect);
      }
      else if (action == kLoungeActionAddPlayer || action == kLoungeLineeditSay)
      {
         // real headless-player / chat handling - not ported yet, not required to prove the
         // login->create->join->lounge chain works.
         logUnhandled(page, action);
      }
      else
         logUnhandled(page, action);
   }
   else
   {
      logUnhandled(page, action);
   }
}

void MenuPageNavigator::onLoginResponse(bool granted)
{
   // matches GameMenuWorkflow::loginResponse(): single player goes straight to GAME_CREATE
   // (create-and-join is automatic from there), multiplayer goes to GAME_SELECT to pick/create.
   if (granted)
   {
      const Constants::GameMode mode = BombermanClient::getInstance()->getGameMode();
      emit pageChangeRequest(mode == Constants::GameModeMultiPlayer ? kGameSelect : kGameCreate);
   }
   else
   {
      qDebug("MenuPageNavigator: login denied");
   }
}

void MenuPageNavigator::onCreateGameResponse(bool granted, int gameId, bool owner)
{
   // matches GameMenuWorkflow::createGameResponse(): the creator auto-joins the game they just
   // made; everyone else (broadcast of the same response) just sees the updated game list.
   if (granted && owner)
      BombermanClient::getInstance()->joinGame(gameId);
   else if (granted)
      emit pageChangeRequest(kGameSelect);
}

void MenuPageNavigator::onJoinGameResponse(bool success)
{
   if (success)
      emit pageChangeRequest(kLounge);
}

void MenuPageNavigator::onGameStarted()
{
   // real gameplay handoff (level loading, HUD, in-game rendering/input) is the separate,
   // not-yet-scoped "Phase 5" - this proves the network state machine reaches GameActive for
   // real, nothing more.
   qDebug("MenuPageNavigator: gameStarted() - real gameplay handoff not implemented yet (Phase 5)");
}

void MenuPageNavigator::onPageChanged(const QString& page)
{
   // matches GameMenuWorkflow::pageChanged(): monitoring is disabled unconditionally first, then
   // re-enabled only for the page actually being shown - only GAME_CREATE's monitoring is ported
   // (video/audio/controls/game options monitoring is still out of scope).
   setMonitorCreateGameOptionsEnabled(false);

   if (page == kGameCreate)
   {
      initializeCreateGameOptions();
      setMonitorCreateGameOptionsEnabled(true);
   }
}

void MenuPageNavigator::initializeCreateGameOptions()
{
   // matches GameMenuInterfaceCreate::initializeCreateGameOptions().
   MenuPage* page = Menu::getInstance()->getPageByName(kGameCreate);

   auto* timeCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_time_table"));
   auto* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_maxplayers_table"));
   auto* botCountCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_bots_table"));
   auto* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_level_table"));
   auto* roundsCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_rounds_table"));

   auto* bombExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_bomb"));
   auto* flameExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_flame"));
   auto* speedUpExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_speedup"));
   auto* kickExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_kick"));
   auto* skullsExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_skull"));

   if (!mCreateGamePagesInitialized.contains(page))
   {
      timeCombo->appendItem("2");
      timeCombo->appendItem("3");
      timeCombo->appendItem("5");
      timeCombo->appendItem("7");

      for (int i = 0; i < 5; i++)
         roundsCombo->appendItem(QString("%1").arg(i + 1));

      for (int i = 2; i <= 10; i++)
         maxPlayersCombo->appendItem(QString("%1").arg(i));

      for (const QString& name : mSortedLevelNames)
         levelCombo->appendItem(name);

      mCreateGamePagesInitialized.insert(page);
   }

   GameSettings::CreateGameSettings* cgs =
      BombermanClient::getInstance()->isSinglePlayer()
         ? GameSettings::getInstance()->getCreateGameSettingsSingle()
         : GameSettings::getInstance()->getCreateGameSettingsMulti();

   timeCombo->setValue(QString("%1").arg(cgs->getDuration()));
   maxPlayersCombo->setValue(QString("%1").arg(cgs->getMaxPlayers()));
   botCountCombo->setValue(QString("%1").arg(cgs->getBotCount()));

   int levelIndex = cgs->getLevelIndex();
   if (levelIndex >= mSortedLevelNames.size())
      levelIndex = 0;

   levelCombo->setValue(mSortedLevelNames[levelIndex]);
   levelCombo->setActiveElement(levelIndex);
   levelCombo->setFocussedElement(levelIndex);

   roundsCombo->setValue(QString("%1").arg(cgs->getRounds()));
   bombExtrasCb->setChecked(cgs->isExtraBombsEnabled());
   flameExtrasCb->setChecked(cgs->isExtraFlamesEnabled());
   speedUpExtrasCb->setChecked(cgs->isExtraSpeedUpsEnabled());
   kickExtrasCb->setChecked(cgs->isExtraKicksEnabled());
   skullsExtrasCb->setChecked(cgs->isExtraSkullsEnabled());

   updateCreateGamePlayerCounts();
   updateCreateGameLevelPreview();
}

void MenuPageNavigator::updateCreateGamePlayerCounts()
{
   // matches GameMenuInterfaceCreate::updateCreateGamePlayerCounts().
   MenuPage* page = Menu::getInstance()->getPageByName(kGameCreate);

   auto* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_maxplayers_table"));
   auto* botCountCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_bots_table"));

   const int maxPlayers = maxPlayersCombo->getValue().toInt();
   const int botCount = botCountCombo->getValue().toInt();
   const int maxBots = maxPlayers - 1;

   botCountCombo->clear();

   for (int i = 0; i <= maxBots; i++)
      botCountCombo->appendItem(QString("%1").arg(i));

   if (botCount > maxBots)
      botCountCombo->setValue(QString("%1").arg(maxBots));
}

void MenuPageNavigator::updateCreateGameLevelPreview()
{
   // matches GameMenuInterfaceCreate::updateCreateGameLevelPreview().
   MenuPage* page = Menu::getInstance()->getPageByName(kGameCreate);

   auto* previewCastle = dynamic_cast<MenuPagePixmapItem*>(page->getPageItem("pixmap_preview_castle"));
   auto* previewMansion = dynamic_cast<MenuPagePixmapItem*>(page->getPageItem("pixmap_preview_mansion"));
   auto* previewSpace = dynamic_cast<MenuPagePixmapItem*>(page->getPageItem("pixmap_preview_space"));
   auto* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_level_table"));

   previewCastle->setVisible(levelCombo->getFocussedElement() <= 0);
   previewMansion->setVisible(levelCombo->getFocussedElement() == 1);
   previewSpace->setVisible(levelCombo->getFocussedElement() == 2);
}

void MenuPageNavigator::setMonitorCreateGameOptionsEnabled(bool enabled)
{
   // matches GameMenuInterfaceCreate::setMonitorCreateGameOptionsEnabled().
   MenuPage* page = Menu::getInstance()->getPageByName(kGameCreate);

   auto* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_maxplayers_table"));
   auto* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_level_table"));

   if (enabled)
   {
      connect(maxPlayersCombo, SIGNAL(valueChanged(QString)), this, SLOT(updateCreateGamePlayerCounts()));
      connect(levelCombo, SIGNAL(valueChanged(QString)), this, SLOT(updateCreateGameLevelPreview()));
      connect(levelCombo, SIGNAL(elementFocussed(int)), this, SLOT(updateCreateGameLevelPreview()));
   }
   else
   {
      disconnect(maxPlayersCombo, SIGNAL(valueChanged(QString)), this, SLOT(updateCreateGamePlayerCounts()));
      disconnect(levelCombo, SIGNAL(valueChanged(QString)), this, SLOT(updateCreateGameLevelPreview()));
      disconnect(levelCombo, SIGNAL(elementFocussed(int)), this, SLOT(updateCreateGameLevelPreview()));
   }
}

void MenuPageNavigator::createGame()
{
   // matches GameMenuInterfaceCreate::createGame().
   MenuPage* page = Menu::getInstance()->getPageByName(kGameCreate);

   auto* gameNameItem = dynamic_cast<MenuPageTextEditItem*>(page->getPageItem("lineedit_name"));
   auto* timeCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_time_table"));
   auto* maxPlayersCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_maxplayers_table"));
   auto* botCountCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_bots_table"));
   auto* levelCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_level_table"));
   auto* roundsCombo = dynamic_cast<MenuPageComboBoxItem*>(page->getPageItem("combobox_rounds_table"));

   auto* bombExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_bomb"));
   auto* flameExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_flame"));
   auto* speedUpExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_speedup"));
   auto* kickExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_kick"));
   auto* skullsExtrasCb = dynamic_cast<MenuPageCheckBoxItem*>(page->getPageItem("checkbox_skull"));

   const QString gameName = gameNameItem->getText();

   int levelIndex = levelCombo->getActiveElement();
   QString levelDirName = (levelIndex >= mSortedLevelDirNames.size()) ? mSortedLevelDirNames[0] : mSortedLevelDirNames[levelIndex];

   const int durationMinutes = timeCombo->getValue().toInt();
   const int durationSeconds = durationMinutes * 60;
   const int maxPlayers = maxPlayersCombo->getValue().toInt();
   const int botCount = botCountCombo->getValue().toInt();
   const int rounds = roundsCombo->getValue().toInt();

   const bool extraBombs = bombExtrasCb->isChecked();
   const bool extraFlames = flameExtrasCb->isChecked();
   const bool extraSpeedUps = speedUpExtrasCb->isChecked();
   const bool extraKicks = kickExtrasCb->isChecked();
   const bool extraSkulls = skullsExtrasCb->isChecked();

   const Constants::Dimension dimension = (maxPlayers <= 5) ? Constants::Dimension13x11 : Constants::Dimension19x17;

   GameSettings::CreateGameSettings* cgs =
      BombermanClient::getInstance()->isSinglePlayer()
         ? GameSettings::getInstance()->getCreateGameSettingsSingle()
         : GameSettings::getInstance()->getCreateGameSettingsMulti();

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
   cgs->serialize();

   BombermanClient::getInstance()->createGame(
      gameName, levelDirName, rounds, durationSeconds, maxPlayers, extraBombs, extraFlames, extraSpeedUps, extraKicks, extraSkulls, dimension
   );
}
