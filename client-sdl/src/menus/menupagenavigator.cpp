#include "menupagenavigator.h"

#include "game/bombermanclient.h"
#include "game/gamesettings.h"
#include "game/soundmanager.h"
#include "game/wordwrap.h"
#include "gameinformation.h"
#include "levels/level.h"

#include "hosthistory.h"
#include "menu.h"
#include "menupage.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupageeditablecomboboxitem.h"
#include "menupagelabelitem.h"
#include "menupagelistitem.h"
#include "menupagepixmapitem.h"
#include "menupageslideritem.h"
#include "menupagetextedit.h"

#include "playerinfo.h"

#include <QDebug>
#include <QTimer>

#include <SDL3_net/SDL_net.h>

#include <algorithm>

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
const char* const kOptionsAudioActionRestoreDefaults = "button_default_active";
const char* const kOptionsAudioSliderMusic = "slider_music";
const char* const kOptionsAudioSliderSfx = "slider_game";

const char* const kAbout = "data/menus/about.psd";
const char* const kAboutActionBack = "button_back_active";
const char* const kLounge = "data/menus/lounge.psd";
const char* const kLoungeActionStart = "button_start_active";
const char* const kLoungeActionBack = "button_leave_active";
const char* const kLoungeActionAddPlayer = "button_addplayer_active";
const char* const kLoungeLineeditSay = "lineedit_say";
const char* const kLoungeTableMain = "table_lounge_main";

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
   bool local = false;

   int count = 0;
   NET_Address** addresses = NET_GetLocalAddresses(&count);

   if (addresses)
   {
      for (int i = 0; i < count && !local; i++)
      {
         const char* address = NET_GetAddressString(addresses[i]);

         if (address && hostName == QString(address))
            local = true;
      }

      NET_FreeLocalAddresses(addresses);
   }

   return local;
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
      qDebug(
         "MenuPageNavigator: page=%s action=%s needs opening an external browser (not implemented)", qPrintable(page), qPrintable(action)
      );
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
   BombermanClient::getInstance()->loginResponseSignal.connect([this](bool granted) { onLoginResponse(granted); });
   BombermanClient::getInstance()->createGameResponseSignal.connect([this](bool granted, int gameId, bool owner)
                                                                    { onCreateGameResponse(granted, gameId, owner); });
   BombermanClient::getInstance()->joinGameResponseSignal.connect([this](bool success) { onJoinGameResponse(success); });
   BombermanClient::getInstance()->gameStartedSignal.connect([this]() { onGameStarted(); });

   // matches GameMenuInterfaceLounge's constructor connection - keeps the lounge's player rows
   // (nick/wins/rank/owner-icon) live-updated whenever the player set changes (join/leave/bot
   // added). Without this, bots that join after the lounge page is already showing never appear.
   BombermanClient::getInstance()->playerInfoMapUpdatedSignal.connect([this](QMap<int, PlayerInfo*>* infoMap)
                                                                      { onPlayerInfoMapUpdated(infoMap); });

   // matches GameMenuWorkflow's own connection to BombermanClient::messageReceived - lounge chat.
   BombermanClient::getInstance()->messageReceivedSignal.connect([this](int senderId, const QString& message, bool finished)
                                                                 { onMessageReceived(senderId, message, finished); });

   // matches MenuWorkflow::initialize() calling deserializeLoginData() once at startup - the
   // main menu is already the current page by construction time (no pageChanged() fires for it
   // the very first time), so this can't wait for onPageChanged()'s own kMainMenu branch below.
   deserializeLoginData();
}

void MenuPageNavigator::onActionRequest(const QString& page, const QString& action)
{
   if (page == kMainMenu)
   {
      // matches GameMenuWorkflow's "extract login data from menu" calls before SINGLE/MULTI/
      // OPTIONS/ABOUT - captures whatever's currently in the nick/host fields into GameSettings
      // + HostHistory. Harmless to run for every mainmenu action (facebook/pouet/home/quit too).
      updateLoginData();

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
         // audio: matches GameMenuInterfaceOptions::storeOptions()/restoreAudioOptions() - OK
         // persists the (already live-applied) SoundManager volume to disk, Cancel reverts
         // SoundManager back to whatever was last persisted, discarding an unsaved drag.
         // video/controls/game options aren't backed by live-applied state yet, so they stay
         // a plain navigate-back.
         if (page == kOptionsAudio)
         {
            GameSettings::AudioSettings* audioSettings = GameSettings::getInstance()->getAudioSettings();

            if (action == kOptionsActionOk)
            {
               audioSettings->setVolumeMusic(SoundManager::getInstance()->getVolumeMusic());
               audioSettings->setVolumeSfx(SoundManager::getInstance()->getVolumeSfx());
               audioSettings->serialize();
            }
            else
            {
               SoundManager::getInstance()->setVolumeMusic(audioSettings->getVolumeMusic());
               SoundManager::getInstance()->setVolumeSfx(audioSettings->getVolumeSfx());
            }
         }

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
      else if (page == kOptionsAudio && action == kOptionsAudioActionRestoreDefaults)
         restoreAudioDefaults();
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
      else if (action == kLoungeLineeditSay)
      {
         // matches GameMenuWorkflow::onActionRequest()'s LOUNGE_LINEEDIT_SAY branch - MenuPage::
         // keyPressed() (menupage.cpp) only emits this actionRequest on Return/Enter, so this is
         // the "finished typing, send it" path. Per-keystroke "still typing" notifications would
         // need MenuPage::actionKeyPressed() wired up too - not done here, matches the existing
         // "typing bubble never shown" simplification (see updateLoungePlayerList()).
         MenuPage* loungePage = Menu::getInstance()->getPageByName(kLounge);
         auto* sayItem = dynamic_cast<MenuPageTextEditItem*>(loungePage->getPageItem(kLoungeLineeditSay));

         if (sayItem)
         {
            const QString message = sayItem->getText();

            if (!message.trimmed().isEmpty())
            {
               // lounge chat always broadcasts to everyone (receiverId -1)
               BombermanClient::getInstance()->sendMessage(message, true);
               sayItem->setText("");
            }
         }
      }
      else if (action == kLoungeActionAddPlayer)
      {
         // real headless-player handling - not ported yet, not required to prove the
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
   {
      emit pageChangeRequest(kLounge);

      // matches GameMenuWorkflow::pageChanged()'s LOUNGE branch (real GameMenuWorkflow isn't
      // ported - this is the only trigger for BombermanClient::initializeBots(), which was
      // otherwise fully wired to BotFactory but never called from anywhere in this port).
      QTimer::singleShot(1000, BombermanClient::getInstance(), SLOT(initializeBots()));
   }
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
   // re-enabled only for the page actually being shown.
   setMonitorCreateGameOptionsEnabled(false);
   setMonitorAudioSettingsEnabled(false);

   if (page == kMainMenu)
   {
      // matches GameMenuWorkflow::pageChanged()'s MAINMENU branch calling
      // mGameMenuInterfaceMain->deserializeLoginData() - repopulates the host combobox/nick
      // field from the saved settings + host history every time the main menu is (re-)shown,
      // not just on first app startup.
      deserializeLoginData();
   }
   else if (page == kGameCreate)
   {
      deserializeCreateGameData();
      initializeCreateGameOptions();
      setMonitorCreateGameOptionsEnabled(true);
   }
   else if (page == kLounge)
   {
      // matches GameMenuWorkflow::pageChanged()'s LOUNGE branch calling
      // mGameMenuInterfaceLounge->playerInfoMapUpdated(...) directly once, on top of the live
      // signal connection - populates the rows immediately instead of waiting for the next
      // join/leave to trigger a redraw.
      updateLoungePlayerList(BombermanClient::getInstance()->getPlayerInfoMap());
   }
   else if (page == kOptionsAudio)
   {
      deserializeAudioSettings();
      setMonitorAudioSettingsEnabled(true);
   }
}

void MenuPageNavigator::onPlayerInfoMapUpdated(QMap<int, PlayerInfo*>* playerInfo)
{
   updateLoungePlayerList(playerInfo);
}

void MenuPageNavigator::updateLoungePlayerList(QMap<int, PlayerInfo*>* playerInfo)
{
   // matches GameMenuInterfaceLounge::playerInfoMapUpdated() - only touches the UI while the
   // lounge page is actually the one showing (mirrors the original's own currentPage == page
   // guard, since this can also fire while some other page, e.g. main menu after leaving, is up).
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   MenuPage* page = Menu::getInstance()->getPageByName(kLounge);

   if (currentPage != page || !playerInfo)
      return;

   mPlayerIdToIndexMap.clear();

   struct ScoreEntry
   {
      PlayerInfo* player;
      int score;
   };

   QList<ScoreEntry> scoreList;
   for (PlayerInfo* info : *playerInfo)
      scoreList.append({info, static_cast<int>(info->getOverallStats().getWins())});

   std::sort(scoreList.begin(), scoreList.end(), [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });

   // initially hide all rows
   for (int i = 1; i <= 10; i++)
   {
      auto* nickItem = dynamic_cast<MenuPageLabelItem*>(currentPage->getPageItem(QString("label_p%1").arg(i)));
      auto* activeBoxItem = currentPage->getPageItem(QString("p%1_box_active").arg(i));
      auto* ownerItem = currentPage->getPageItem(QString("p%1_leader_icon").arg(i));
      auto* playerItem = currentPage->getPageItem(QString("p%1_icon").arg(i));
      auto* rankItem = currentPage->getPageItem(QString("label_rank_%1").arg(i));
      auto* winsItem = dynamic_cast<MenuPageLabelItem*>(currentPage->getPageItem(QString("label_p%1_wins").arg(i)));
      // matches GameMenuInterfaceLounge::initializeLoungeStates() - only ever shown by the
      // typing-indicator feature (updatePlayerTyping()/removePlayerTyping()), which isn't ported
      // (see the deferred chat handling below) - so it must be force-hidden here instead, or the
      // PSD's raw default visibility leaks through unmodified for every row.
      auto* typingBoxItem = currentPage->getPageItem(QString("p%1_box_type").arg(i));

      if (nickItem)
         nickItem->setText("");
      if (activeBoxItem)
         activeBoxItem->setVisible(false);
      if (ownerItem)
         ownerItem->setVisible(false);
      if (playerItem)
         playerItem->setVisible(false);
      if (rankItem)
         rankItem->setVisible(false);
      if (winsItem)
         winsItem->setVisible(false);
      if (typingBoxItem)
         typingBoxItem->setVisible(false);
   }

   int counter = 0;
   for (const ScoreEntry& entry : scoreList)
   {
      counter++;

      PlayerInfo* player = entry.player;
      const int color = static_cast<int>(player->getColor());
      GameInformation* gameInfo = BombermanClient::getInstance()->getCurrentGameInformation();
      const bool owner = gameInfo && (player->getId() == gameInfo->getCreatorId());

      auto* nickItem = dynamic_cast<MenuPageLabelItem*>(currentPage->getPageItem(QString("label_p%1").arg(counter)));
      auto* winsItem = dynamic_cast<MenuPageLabelItem*>(currentPage->getPageItem(QString("label_p%1_wins").arg(counter)));
      auto* rankItem = currentPage->getPageItem(QString("label_rank_%1").arg(counter));
      auto* activeBoxItem = currentPage->getPageItem(QString("p%1_box_active").arg(counter));
      auto* ownerItem = currentPage->getPageItem(QString("p%1_leader_icon").arg(counter));
      auto* playerItem = currentPage->getPageItem(QString("p%1_icon").arg(color));

      // note: the original also nudges playerItem's active-layer Y to align with activeBoxItem's
      // top (playerItem->getCurrentLayer()->setY(...)) - PSDLayer::setY() isn't ported in this
      // port (only getters + setOpacity exist), so that pixel-alignment tweak is skipped; the row
      // still shows correctly, just not pixel-perfect vertically.

      mPlayerIdToIndexMap.insert(player->getId(), counter);

      if (winsItem)
      {
         winsItem->setText(QString("%1").arg(entry.score));
         winsItem->setColor(QColor(counter <= 3 ? "#fbfe00" : "#3b7d9d"));
         winsItem->setVisible(true);
      }

      if (nickItem)
         nickItem->setText(player->getNick());
      if (activeBoxItem)
         activeBoxItem->setVisible(true);
      if (ownerItem)
         ownerItem->setVisible(owner);
      if (playerItem)
         playerItem->setVisible(true);
      if (rankItem)
         rankItem->setVisible(true);
   }
}

void MenuPageNavigator::onMessageReceived(int senderId, const QString& message, bool finished)
{
   // matches GameMenuWorkflow::messageReceived() - typing-in-progress notifications
   // (finished == false) would drive the "typing bubble" via updatePlayerTyping(), which isn't
   // ported (see the comment in updateLoungePlayerList()); only finished messages are displayed.
   if (finished)
   {
      addLoungeMessage(senderId, message);
   }
}

void MenuPageNavigator::addLoungeMessage(int senderId, const QString& message)
{
   // matches GameMenuInterfaceLounge::addLoungeMessage() - only touches the UI while the lounge
   // page is actually showing.
   MenuPage* loungePage = Menu::getInstance()->getPageByName(kLounge);
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();

   if (loungePage != currentPage)
      return;

   auto* sayItem = dynamic_cast<MenuPageTextEditItem*>(loungePage->getPageItem(kLoungeLineeditSay));
   auto* tableItem = dynamic_cast<MenuPageListItem*>(currentPage->getPageItem(kLoungeTableMain));

   if (!sayItem || !tableItem)
      return;

   // the server already prepends "nick: " to the message (see Game::processPacket's MESSAGE
   // case) - split it back out here only to avoid repeating the nick on wrapped continuation
   // lines, matching the original's own formatting.
   QString nick;
   const int nickEnd = message.indexOf(": ");
   if (nickEnd != -1)
      nick = message.left(nickEnd);

   const Constants::Color playerColor = BombermanClient::getInstance()->getColor(senderId);
   QColor color = GameSettings::getInstance()->getStyleSettings()->getColor(playerColor);
   const QColor outlineColor(0, 0, 0, 255);

   if (playerColor == Constants::ColorBlack)
      color = QColor(128, 128, 128, 255);

   const QStringList lines = WordWrap::wrap(message, sayItem->getFieldWidth());

   int i = 0;
   for (const QString& line : lines)
   {
      QString text;
      if (i == 0 || nick.isEmpty())
         text = line;
      else
         text = QString("%1: %2").arg(nick).arg(line);

      const QString trimmed = line.trimmed();
      if (trimmed != QString("%1:").arg(nick) && !trimmed.isEmpty())
         tableItem->appendItem(text, color, true, outlineColor);

      ++i;
   }

   tableItem->scrollToPercentage(100.0f, false);
}

void MenuPageNavigator::deserializeLoginData()
{
   // matches GameMenuInterfaceMain::deserializeLoginData().
   MenuPage* page = Menu::getInstance()->getPageByName(kMainMenu);

   auto* hostCombo = dynamic_cast<MenuPageEditableComboBoxItem*>(page->getPageItem("editablecombobox_host_table"));
   auto* nickItem = dynamic_cast<MenuPageTextEditItem*>(page->getPageItem("lineedit_nick"));

   if (!hostCombo)
      return;

   hostCombo->clear();

   const QString savedHost = GameSettings::getInstance()->getLoginSettings()->getHost();

   const QStringList hosts = mHostHistory.load(savedHost);
   for (const QString& host : hosts)
      hostCombo->appendItem(host);

   if (nickItem)
      nickItem->setText(GameSettings::getInstance()->getLoginSettings()->getNick());

   // there is no use to deserialize the saved host when there's already a valid value set up
   // (the game has been started before and a valid hostname restored) - don't overwrite whatever
   // the user already typed.
   auto* hostTextEdit = hostCombo->getTextEditItem();
   if (hostTextEdit && hostTextEdit->getText().isEmpty())
      hostTextEdit->setText(savedHost);
}

void MenuPageNavigator::updateLoginData()
{
   // matches GameMenuInterfaceMain::updateLoginData().
   MenuPage* page = Menu::getInstance()->getPageByName(kMainMenu);

   auto* hostCombo = dynamic_cast<MenuPageEditableComboBoxItem*>(page->getPageItem("editablecombobox_host_table"));
   auto* nickItem = dynamic_cast<MenuPageTextEditItem*>(page->getPageItem("lineedit_nick"));

   if (!hostCombo || !nickItem)
      return;

   auto* hostTextEdit = hostCombo->getTextEditItem();
   if (!hostTextEdit)
      return;

   const QString host = hostTextEdit->getText();
   const QString nick = nickItem->getText();

   GameSettings::getInstance()->getLoginSettings()->setHost(host);
   GameSettings::getInstance()->getLoginSettings()->setNick(nick);

   mHostHistory.add(host);
}

void MenuPageNavigator::deserializeCreateGameData()
{
   // matches GameMenuInterfaceCreate::deserializeCreateGameData() - reads from
   // getCreateGameSettingsSingle() unconditionally, unlike initializeCreateGameOptions() below
   // (which does branch on isSinglePlayer()) - this is the original's own behavior, not a typo
   // introduced by the port, so left as-is rather than "fixed" to branch.
   MenuPage* page = Menu::getInstance()->getPageByName(kGameCreate);
   auto* gameNameItem = dynamic_cast<MenuPageTextEditItem*>(page->getPageItem("lineedit_name"));

   gameNameItem->setText(GameSettings::getInstance()->getCreateGameSettingsSingle()->getGameName());
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

   GameSettings::CreateGameSettings* cgs = BombermanClient::getInstance()->isSinglePlayer()
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

void MenuPageNavigator::deserializeAudioSettings()
{
   // matches GameMenuInterfaceOptions::deserializeAudioSettings() - slider positions come from
   // SoundManager's live volume, not GameSettings directly (SoundManager itself was seeded from
   // GameSettings at startup).
   MenuPage* page = Menu::getInstance()->getPageByName(kOptionsAudio);

   auto* musicSlider = dynamic_cast<MenuPageSliderItem*>(page->getPageItem(kOptionsAudioSliderMusic));
   auto* sfxSlider = dynamic_cast<MenuPageSliderItem*>(page->getPageItem(kOptionsAudioSliderSfx));

   musicSlider->setValue(SoundManager::getInstance()->getVolumeMusic());
   sfxSlider->setValue(SoundManager::getInstance()->getVolumeSfx());
}

void MenuPageNavigator::setMonitorAudioSettingsEnabled(bool enabled)
{
   // matches GameMenuInterfaceOptions::setMonitorAudioSettingsEnabled() - only the sfx slider
   // gets a tick sound (audible feedback of the new sfx volume itself); the music slider doesn't.
   MenuPage* page = Menu::getInstance()->getPageByName(kOptionsAudio);

   auto* musicSlider = dynamic_cast<MenuPageSliderItem*>(page->getPageItem(kOptionsAudioSliderMusic));
   auto* sfxSlider = dynamic_cast<MenuPageSliderItem*>(page->getPageItem(kOptionsAudioSliderSfx));

   if (enabled)
   {
      connect(musicSlider, SIGNAL(valueChanged(float)), this, SLOT(applyVolumeMusic(float)));
      connect(sfxSlider, SIGNAL(valueChanged(float)), this, SLOT(applyVolumeSfx(float)));
      connect(sfxSlider, SIGNAL(valueChanged(float)), SoundManager::getInstance(), SLOT(playSoundTick()));
   }
   else
   {
      disconnect(musicSlider, SIGNAL(valueChanged(float)), this, SLOT(applyVolumeMusic(float)));
      disconnect(sfxSlider, SIGNAL(valueChanged(float)), this, SLOT(applyVolumeSfx(float)));
      disconnect(sfxSlider, SIGNAL(valueChanged(float)), SoundManager::getInstance(), SLOT(playSoundTick()));
   }
}

void MenuPageNavigator::applyVolumeMusic(float volume)
{
   SoundManager::getInstance()->setVolumeMusic(volume);
}

void MenuPageNavigator::applyVolumeSfx(float volume)
{
   SoundManager::getInstance()->setVolumeSfx(volume);
}

void MenuPageNavigator::restoreAudioDefaults()
{
   // matches GameMenuInterfaceOptions::restoreAudioDefaults(): reset GameSettings, push the
   // (now-default) values into SoundManager, then re-seed the sliders' visual positions.
   GameSettings::AudioSettings* audioSettings = GameSettings::getInstance()->getAudioSettings();
   audioSettings->restoreDefaults();

   SoundManager::getInstance()->setVolumeMusic(audioSettings->getVolumeMusic());
   SoundManager::getInstance()->setVolumeSfx(audioSettings->getVolumeSfx());

   deserializeAudioSettings();
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

   GameSettings::CreateGameSettings* cgs = BombermanClient::getInstance()->isSinglePlayer()
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
