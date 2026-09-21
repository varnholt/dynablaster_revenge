#include "menupagenavigator.h"

#include "game/bombermanclient.h"
#include "game/gamesettings.h"
#include "gameinformation.h"

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
         // real GameMenuInterfaceCreate reads the page's own controls (name/level/rounds/extras
         // checkboxes) into a CreateGameRequestPacket - that UI readback isn't wired in this port
         // yet, so this uses BombermanClient::createGameAutomatic() (the same dev/test convenience
         // the original codebase already ships), not a reimplementation of the real defaults.
         BombermanClient::getInstance()->createGameAutomatic();
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
