#include "menupagenavigator.h"

#include <QDebug>

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

bool isOptionsPage(const QString& page)
{
   return page == kOptionsVideo || page == kOptionsAudio || page == kOptionsControls || page == kOptionsGame;
}

void logUnhandled(const QString& page, const QString& action)
{
   if (!action.isEmpty())
   {
      qDebug(
         "MenuPageNavigator: page=%s action=%s received but not wired up yet (needs real "
         "networking/browser support this port doesn't have)",
         qPrintable(page),
         qPrintable(action)
      );
   }
}

}  // namespace

MenuPageNavigator::MenuPageNavigator(QObject* parent) : QObject(parent)
{
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
      else
         logUnhandled(page, action);
   }
   else if (page == kGameSelect)
   {
      if (action == kGameSelectActionCreate)
         emit pageChangeRequest(kGameCreate);
      else if (action == kGameSelectActionBack)
         emit pageChangeRequest(kMainMenu);
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
   else
   {
      // LOUNGE and anything else: every real action there needs live game/network state
      // (BombermanClient, GameStateMachine) that doesn't exist in this port yet.
      logUnhandled(page, action);
   }
}
