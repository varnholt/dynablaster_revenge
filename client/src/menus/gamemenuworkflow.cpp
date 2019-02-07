// Qt
#include <QApplication>
#include <QHostAddress>
#include <QNetworkInterface>

// menu
#include "gamemenudefines.h"
#include "gamemenuinterface.h"
#include "gamemenuinterfacecreate.h"
#include "gamemenuinterfacelounge.h"
#include "gamemenuinterfacemain.h"
#include "gamemenuinterfaceoptions.h"
#include "gamemenuinterfaceselect.h"
#include "gamemenuinterfacecredits.h"
#include "gamemenuworkflow.h"
#include "gameplayback.h"

// game
#include "bombermanclient.h"
#include "gamesettings.h"
#include "gamestatemachine.h"
#include "helpmanager.h"
#include "playerinfo.h"


//-----------------------------------------------------------------------------
/*!
*/
GameMenuWorkflow::GameMenuWorkflow(QObject *parent)
 : MenuWorkflow(parent)
{
   mMenuInterface = new GameMenuInterface(this);
   mGameMenuInterfaceCreate = new GameMenuInterfaceCreate(this);
   mGameMenuInterfaceLounge = new GameMenuInterfaceLounge(this);
   mGameMenuInterfaceMain = new GameMenuInterfaceMain(this);
   mGameMenuInterfaceOptions = new GameMenuInterfaceOptions(this);
   mGameMenuInterfaceSelect = new GameMenuInterfaceSelect(this);
   mGameMenuInterfaceCredits = new GameMenuInterfaceCredits(this);

   mGameListUpdateTimer.setInterval(1000);

   connect(
      &mGameListUpdateTimer,
      SIGNAL(timeout()),
      SIGNAL(gameListRequest())
   );

   connect(
      mGameMenuInterfaceOptions,
      SIGNAL(updateFullscreen()),
      this,
      SIGNAL(updateFullscreen())
   );

   connect(
      mGameMenuInterfaceOptions,
      SIGNAL(updateShowFps()),
      this,
      SIGNAL(updateShowFps())
   );

   connect(
      mGameMenuInterfaceOptions,
      SIGNAL(updateVsync()),
      this,
      SIGNAL(updateVsync())
   );
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if saved host is localhost
*/
bool GameMenuWorkflow::isHostLocal()
{
   QString hostName = GameSettings::getInstance()->getLoginSettings()->getHost();
   QHostAddress hostAddress(hostName);

   bool local = false;

   QList<QHostAddress> list = QNetworkInterface::allAddresses();
   foreach(const QHostAddress& address, list)
   {
      if (hostAddress == address)
      {
         local = true;
         break;
      }
   }

   return local;
}


//-----------------------------------------------------------------------------
/*!
   \param page page that is open
   \param action action that shall be executed
*/
void GameMenuWorkflow::actionRequest(
   const QString& page,
   const QString& action
)
{
   qDebug(
      "GameMenuWorkflow::actionRequest: page: %s, action: %s",
      qPrintable(page),
      qPrintable(action)
   );

   if (page == MAINMENU)
   {
      /*
         concept:
         1) extract all data from menu with menuinterace class
         2) let menuinterface communicate the dataobjects to the client
         3) get informed by menuinterface about updates
            like loginResponse e.g.


         server workflow

         1) LoginRequest, wait for
            LoginResponse

         2) ListGameRequest, wait for
            ListGameResponse

         3) CreateGameRequest if required, wait for
            CreateGameResponse

         4) JoinGameRequest, wait for
            JoinGameResponse (for each player in the game)

         5) StartGameRequest, wait for
            StartGameResponse

      */


      if (action == MAINMENU_ACTION_SINGLE)
      {
         BombermanClient::getInstance()->setGameMode(
            Constants::GameModeSinglePlayer
         );

         emit host();

         // extract login data from menu
         mGameMenuInterfaceMain->updateLoginData();

         // fire login request
         emit loginRequest(
            "127.0.0.1", // GameSettings::getInstance()->getLoginSettings()->getHost(),
            GameSettings::getInstance()->getLoginSettings()->getNick()
         );
      }

      else if (action == MAINMENU_ACTION_MULTI)
      {                 
         BombermanClient::getInstance()->setGameMode(
            Constants::GameModeMultiPlayer
         );

         // extract login data from menu
         mGameMenuInterfaceMain->updateLoginData();

         bool local = isHostLocal();

         if (local)
            emit host();

         // fire login request
         emit loginRequest(
            GameSettings::getInstance()->getLoginSettings()->getHost(),
            GameSettings::getInstance()->getLoginSettings()->getNick()
         );
      }

      else if (action == MAINMENU_ACTION_OPTIONS)
      {
         // extract login data from menu
         mGameMenuInterfaceMain->updateLoginData();

         emit pageChangeRequest(OPTIONS_VIDEO);
      }

      else if (action == MAINMENU_ACTION_ABOUT)
      {
         // extract login data from menu
         mGameMenuInterfaceMain->updateLoginData();

         emit pageChangeRequest(ABOUT);
      }

      else if (action == MAINMENU_ACTION_QUIT)
      {
         qApp->exit(0);
      }

      else if (action == MAINMENU_ACTION_POUET)
      {
         mGameMenuInterfaceMain->openPouet();
      }

      else if (action == MAINMENU_ACTION_FACEBOOK)
      {
         mGameMenuInterfaceMain->openFacebook();
      }

      else if (action == MAINMENU_ACTION_HOME)
      {
         mGameMenuInterfaceMain->openHome();
      }
   }

   if (page == GAME_SELECT)
   {
      if (action == GAME_SELECT_TABLE)
      {
         mGameMenuInterfaceSelect->updateGameProperties();
      }

      else if (action == GAME_SELECT_ACTION_JOIN)
      {
         int gameId = mGameMenuInterfaceSelect->getSelectedGame();
         emit joinGame(gameId);
      }

      else if (action == GAME_SELECT_ACTION_CREATE)
      {
         emit pageChangeRequest(GAME_CREATE);
      }

      else if (action == GAME_SELECT_ACTION_BACK)
      {
         emit pageChangeRequest(MAINMENU);
      }
   }

   if (page == GAME_CREATE)
   {
      if (action == GAME_CREATE_ACTION_OK)
      {
         mGameMenuInterfaceCreate->createGame();
      }

      else if (action == GAME_CREATE_ACTION_CANCEL)
      {
         Constants::GameMode mode = BombermanClient::getInstance()->getGameMode();

         QString nextPage;

         if (mode == Constants::GameModeMultiPlayer)
            nextPage = GAME_SELECT;
         else
            nextPage = MAINMENU;

         emit pageChangeRequest(nextPage);
      }
   }

   if (page == LOUNGE)
   {
      Constants::GameState state = GameStateMachine::getInstance()->getState();

      if (state == Constants::GameStopped)
      {
         if (action == LOUNGE_ACTION_START)
         {
            int gameId = BombermanClient::getInstance()->getGameId();
            emit gameStartRequest(gameId);
         }

         else if (action == LOUNGE_ACTION_ADDPLAYER)
         {
            emit addHeadlessPlayer();
            mGameMenuInterfaceLounge->updateAddHeadlessPlayerButton();
         }

         else if (action == LOUNGE_ACTION_BACK)
         {
            mGameMenuInterfaceLounge->clearLoungeMessage();
            mGameMenuInterfaceLounge->clearLoungeMessages();

            Constants::GameMode mode = BombermanClient::getInstance()->getGameMode();

            QString nextPage;

            if (mode == Constants::GameModeMultiPlayer)
               nextPage = GAME_SELECT;
            else
               nextPage = MAINMENU;

            emit pageChangeRequest(nextPage);
            emit leaveGameRequest();
         }

         else if (action == LOUNGE_LINEEDIT_SAY)
         {
            processLoungeMessage(true);
         }
      }
   }

   if (
         page == OPTIONS_VIDEO
      || page == OPTIONS_AUDIO
      || page == OPTIONS_CONTROLS
      || page == OPTIONS_GAME
   )
   {
      if (action == OPTIONS_ACTION_OK)
      {
         mGameMenuInterfaceOptions->storeOptions();
         emit pageChangeRequest(MAINMENU);
      }

      else if (action == OPTIONS_ACTION_CANCEL)
      {
         mGameMenuInterfaceOptions->restoreAudioOptions();
         mGameMenuInterfaceOptions->restoreVideoOptions();
         emit pageChangeRequest(MAINMENU);
      }

      else if (action == OPTIONS_ACTION_VIDEO)
      {
         emit pageChangeRequest(OPTIONS_VIDEO);
      }

      else if (action == OPTIONS_ACTION_CONTROLS)
      {
         emit pageChangeRequest(OPTIONS_CONTROLS);
      }

      else if (action == OPTIONS_ACTION_AUDIO)
      {
         emit pageChangeRequest(OPTIONS_AUDIO);
      }

      else if (action == OPTIONS_ACTION_GAME)
      {
         emit pageChangeRequest(OPTIONS_GAME);
      }

      else if (
            page == OPTIONS_AUDIO
         && action == OPTIONS_AUDIO_RESTORE_DEFAULTS
      )
      {
         mGameMenuInterfaceOptions->restoreAudioDefaults();
      }

      else if (
            page == OPTIONS_CONTROLS
         && action == OPTIONS_CONTROLS_RESTORE_DEFAULTS
      )
      {
         mGameMenuInterfaceOptions->restoreControlsDefaults();
      }

      else if (
            page == OPTIONS_VIDEO
         && action == OPTIONS_VIDEO_RESTORE_DEFAULTS
      )
      {
         mGameMenuInterfaceOptions->restoreVideoDefaults();
      }

      else if (
            page == OPTIONS_GAME
         && action == OPTIONS_GAME_RESTORE_DEFAULTS
      )
      {
         mGameMenuInterfaceOptions->restoreGameDefaults();
      }
   }

   if (page == ABOUT)
   {
      if (action == ABOUT_ACTION_BACK)
      {
         emit pageChangeRequest(MAINMENU);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::processLoungeMessage(bool finishedTyping)
{
   QString message;
   message = mGameMenuInterfaceLounge->getLoungeMessage();

   if (!message.trimmed().isEmpty())
   {
      // lounge always sends to all players
      emit sendMessage(message, finishedTyping);

      if (finishedTyping)
      {
         mGameMenuInterfaceLounge->clearLoungeMessage();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::actionKeyPressed(
   const QString &page,
   const QString &itemName,
   int key
)
{
   if (page == LOUNGE)
   {
      if (itemName == LOUNGE_LINEEDIT_SAY)
      {
         if (
               key != Qt::Key_Return
            && key != Qt::Key_Enter
         )
         {
            processLoungeMessage(false);
         }
      }
   }
   else if (page == OPTIONS_CONTROLS)
   {
      mGameMenuInterfaceOptions->processOptionsControlKeyPressed(
         key,
         itemName
      );
   }
}


//-----------------------------------------------------------------------------
/*!
   \param granted game successfully created
   \param gameId id of game that has been created
   \param owner \c true if game owner
*/
void GameMenuWorkflow::createGameResponse(bool granted, int gameId, bool owner)
{
   if (granted)
   {
      // the owner wants to join the game he just created
      if (owner)
      {
         emit joinGame(gameId);
      }
      else
      {
         if (!GamePlayback::getInstance()->isReplaying())
         {
            emit pageChangeRequest(GAME_SELECT);
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::gamesListUpdated(const QList<GameInformation>& list)
{
   // qDebug("GameMenuWorkflow::gamesListUpdated");

   mGameMenuInterfaceSelect->gamesListUpdated(list);
   mGameMenuInterfaceSelect->updateServerProperties();
   mGameMenuInterfaceSelect->updateGameProperties();
}


//-----------------------------------------------------------------------------
/*!
   \param page next page
*/
void GameMenuWorkflow::pageChanged(const QString& page)
{
   MenuWorkflow::pageChanged(page);

   // update background color
   GameMenuInterface* interface = (GameMenuInterface*)mMenuInterface;
   interface->updateBackgroundColor(page);

   bool updateGameList = false;

   // initially disable all listeners
   mGameMenuInterfaceOptions->setMonitorAudioSettingsEnabled(false);
   mGameMenuInterfaceOptions->setMonitorVideoSettingsEnabled(false);
   mGameMenuInterfaceOptions->setMonitorJoystickEnabled(false);
   mGameMenuInterfaceLounge->setLoungeConnectionsEnabled(false);
   mGameMenuInterfaceCreate->setMonitorCreateGameOptionsEnabled(false);

   emit mainMenuShown(page == MAINMENU);

   if (page == MAINMENU)
   {
      qDebug("pageChanged: main menu");
      mGameMenuInterfaceMain->updateLedStatus();
      mGameMenuInterfaceMain->deserializeLoginData();
      mGameMenuInterfaceMain->deserializeVersion();
   }

   else if (page == GAME_SELECT)
   {
      // request the list of games immediately
      emit gameListRequest();

      // and get list updates on a regular base
      updateGameList = true;
   }

   else if (page == GAME_CREATE)
   {
      mGameMenuInterfaceCreate->deserializeCreateGameData();
      mGameMenuInterfaceCreate->initializeCreateGameOptions();
      mGameMenuInterfaceCreate->setMonitorCreateGameOptionsEnabled(true);
   }

   else if (page == LOUNGE)
   {
      mGameMenuInterfaceLounge->initializeLoungeStates();
      mGameMenuInterfaceLounge->setLoungeConnectionsEnabled(true);
      mGameMenuInterfaceLounge->gameInfoUpdated();
      mGameMenuInterfaceLounge->playerInfoMapUpdated(
         BombermanClient::getInstance()->getPlayerInfoMap()
      );
      mGameMenuInterfaceLounge->updateAddHeadlessPlayerButton();

      // ready to create the bots
      QTimer::singleShot(
         1000,
         BombermanClient::getInstance(),
         SLOT(initializeBots())
      );
   }

   else if (page == OPTIONS_CONTROLS)
   {
      mGameMenuInterfaceOptions->initializeButtonLabels();
      mGameMenuInterfaceOptions->resetJoystickStates();
      mGameMenuInterfaceOptions->deserializeControllerSettings();
      mGameMenuInterfaceOptions->setMonitorJoystickEnabled(true);
   }

   else if (page == OPTIONS_AUDIO)
   {
      mGameMenuInterfaceOptions->deserializeAudioSettings();
      mGameMenuInterfaceOptions->setMonitorAudioSettingsEnabled(true);
   }

   else if (page == OPTIONS_VIDEO)
   {
      mGameMenuInterfaceOptions->deserializeVideoSettings();
      mGameMenuInterfaceOptions->setMonitorVideoSettingsEnabled(true);
   }

   else if (page == OPTIONS_GAME)
   {
      mGameMenuInterfaceOptions->deserializeGameplaySettings();
   }

   else if (page == ABOUT)
   {
      mGameMenuInterfaceCredits->reset();
   }

   if (updateGameList)
      mGameListUpdateTimer.start();
   else
      mGameListUpdateTimer.stop();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::playerInfoMapUpdated(QMap<int, PlayerInfo*>* playerInfo)
{
   mGameMenuInterfaceLounge->playerInfoMapUpdated(playerInfo);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::joinGameResponse(bool success)
{
   if (success)
   {
      if (!GamePlayback::getInstance()->isReplaying())
      {
         emit pageChangeRequest(LOUNGE);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::initialize()
{
   mGameMenuInterfaceMain->deserializeLoginData();
   mGameMenuInterfaceMain->updateLedStatus();

   // call base
   MenuWorkflow::initialize();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::messageReceived(
   int senderId,
   const QString& message,
   bool typingFinished
)
{
   if (typingFinished)
   {
      mGameMenuInterfaceLounge->addLoungeMessage(
         senderId,
         message
      );

      qDebug(
         "GameMenuWorkflow::messageReceived: display message: %s",
         qPrintable(message)
      );
   }

   mGameMenuInterfaceLounge->updatePlayerTyping(
      senderId,
      typingFinished
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::showMainMenu()
{
   emit pageChangeRequest(MAINMENU);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::menuVisible(bool visible)
{
   MenuWorkflow::menuVisible(visible);

   if (visible)
   {
      if (getCurrentPage() == LOUNGE)
      {
         mGameMenuInterfaceLounge->initializeLastPlayerMessage();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param granted \c true login successful
*/
void GameMenuWorkflow::loginResponse(bool granted)
{
   if (granted)
   {
      // store login data for the next session
      GameSettings::getInstance()->getLoginSettings()->serialize();
      mGameMenuInterfaceMain->updateLedStatus();

      // change page
      if (!GamePlayback::getInstance()->isReplaying())
      {
         Constants::GameMode mode = BombermanClient::getInstance()->getGameMode();

         QString nextPage;

         if (mode == Constants::GameModeMultiPlayer)
            nextPage = GAME_SELECT;
         else
            nextPage = GAME_CREATE;

         emit pageChangeRequest(nextPage);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameMenuWorkflow::serverOnline(bool /*online*/)
{
   mGameMenuInterfaceMain->updateLedStatus();
}

