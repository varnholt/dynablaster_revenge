// header
#include "gamemenuhelp.h"

// game
#include "bombermanclient.h"
#include "gamehelptexts.h"
#include "gamemenudefines.h"
#include "helpmanager.h"


GameMenuHelp::GameMenuHelp(QObject *parent) :
   QObject(parent),
   mEnabled(true)
{
}


bool GameMenuHelp::isEnabled() const
{
   return mEnabled;
}


void GameMenuHelp::pageChanged(const QString &page)
{
   if (page == MAINMENU)
   {
      HelpManager::getInstance()->addMessage(
         MAINMENU,
         TEXT_MAIN_SINGLE,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );

      HelpManager::getInstance()->addMessage(
         MAINMENU,
         TEXT_MAIN_MULTI,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );

      HelpManager::getInstance()->addMessage(
         MAINMENU,
         TEXT_MAIN_SERVER,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );
   }

   else if (page == GAME_SELECT)
   {
      HelpManager::getInstance()->addMessage(
         GAME_SELECT,
         TEXT_SELECT_GAME_SELECT,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );

      HelpManager::getInstance()->addMessage(
         GAME_SELECT,
         TEXT_SELECT_GAME_CREATE,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );
   }

   else if (page == GAME_CREATE)
   {
      HelpManager::getInstance()->addMessage(
         GAME_CREATE,
         TEXT_CREATE_GAME_DEFINE,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );
   }

   else if (page == LOUNGE)
   {
      // only do that if we're game owner
      if (BombermanClient::getInstance()->isPlayerOwner())
      {
         HelpManager::getInstance()->addMessage(
            LOUNGE,
            TEXT_LOUNGE_START_GAME,
            Constants::HelpSeverityNotification,
            Constants::HelpLocationTopRight,
            15000
         );
      }

      HelpManager::getInstance()->addMessage(
         LOUNGE,
         TEXT_LOUNGE_CHAT,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );
   }

   else if (page == OPTIONS_CONTROLS)
   {
      HelpManager::getInstance()->addMessage(
         OPTIONS_CONTROLS,
         TEXT_OPTIONS_JOYSTICK,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );

      HelpManager::getInstance()->addMessage(
         OPTIONS_CONTROLS,
         TEXT_OPTIONS_KEYBOARD,
         Constants::HelpSeverityNotification,
         Constants::HelpLocationTopRight,
         15000
      );
   }

   else if (page == OPTIONS_AUDIO)
   {
   }
}


