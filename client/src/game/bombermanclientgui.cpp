// header
#include "bombermanclientgui.h"

// engine
#include "tools/singleton.h"
#include "renderthread.h"
#include "soundmanager.h"

// joystick
#include "gamejoystickintegration.h"
#include "gamejoystickmapping.h"
#include "joystickinterface.h"

// game
#include "bombermanclient.h"
#include "bombermanview.h"
#include "controllerui.h"
#include "countdowndrawable.h"
#include "fontpool.h"
#include "gamedrawable.h"
#include "gamehelpdrawable.h"
#include "gamemessagingdrawable.h"
#include "gameplayback.h"
#include "gamesettings.h"
#include "gamestatsdrawable.h"
#include "headlessclient.h"
#include "headlessintegration.h"
#include "hotkeydrawable.h"
#include "loadingdrawable.h"
#include "menujoystickhandler.h"
#include "musicplayerdrawable.h"
#include "positioninterpolation.h"
#include "roundsdrawable.h"
#include "soundmanager.h"

// menus
#include "menu.h"
#include "menudrawable.h"
#include "gamemenuhelp.h"
#include "gamemenuinterface.h"
#include "gamemenuworkflow.h"

// qt
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QMainWindow>
#include <QFrame>
#include <QShortcut>
#include <QSplashScreen>
#include <QVBoxLayout>
#include <QTimer>


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
   \param fps frames per second
*/
BombermanClientGui::BombermanClientGui(QObject* parent, float fps)
 : QObject(parent),
   mClient(0),
   mGameView(0),
   mWorkflow(0),
   mMenuJoystickHandler(0),
   mMenuHelp(0),
   mUpdateTimer(0),
   mSplash(0),
   mShortcutTrackNext(0),
   mShortcutTrackPrev(0),
   mShortcutFullscreen(0),
   mShortcutEndGame(0),
   mShortcutSavePlayback(0),
   mShortcutMuteMusic(0),
   mShortcutMuteSfx(0),
   mShortcutShowIps(0),
   mShortcutAddHeadlessPlayer(0),
   mShortcutAddHeadlessPlayerUS(0)
{
   QGLFormat format= QGLFormat::defaultFormat();
   format.setSwapInterval(0);

   mGameView = new GameView(0, format);
   mGameView->move(0,0);

   deserializeSwapInterval();

   if (fps > 0.0f)
   {
      new SoundManager(true);
      mGameView->setVideoExport(fps);
   }

   mClient = new BombermanClient();

//   GameJoystickIntegration::getInstance();

   mMenuHelp = new GameMenuHelp(this);
   mWorkflow = new GameMenuWorkflow(this);
   mMenuJoystickHandler = new MenuJoystickHandler(this);

   mUpdateTimer= new QTimer(this);
   mUpdateTimer->setInterval(1);

   connect(
      mUpdateTimer,
      SIGNAL( timeout() ),
      mGameView,
      SLOT( update() ),
      Qt::DirectConnection
   );

   connect(
      mGameView,
      SIGNAL( updateFps(float) ),
      this,
      SLOT( updateFps(float) ),
      Qt::QueuedConnection
   );

   connect(
      mClient, SIGNAL( zoomIn(bool) ),
      mGameView, SLOT( zoomIn(bool) ),
      Qt::QueuedConnection
   );

   connect(
      mClient, SIGNAL( zoomOut(bool) ),
      mGameView, SLOT( zoomOut(bool) ),
      Qt::QueuedConnection
   );

   qRegisterMetaType< QList<GameInformation> >("QList<GameInformation>");
   qRegisterMetaType<Constants::Color>("Constants::Color");
   qRegisterMetaType<Constants::ExtraType>("Constants::ExtraType");

   // hotkeys
   mShortcutHotkeys = new QShortcut(mGameView);
   mShortcutTrackNext = new QShortcut(mGameView);
   mShortcutTrackPrev = new QShortcut(mGameView);
   mShortcutFullscreen = new QShortcut(mGameView);
   mShortcutEndGame = new QShortcut(mGameView);
   mShortcutSavePlayback = new QShortcut(mGameView);
   mShortcutMuteMusic = new QShortcut(mGameView);
   mShortcutMuteSfx = new QShortcut(mGameView);
   mShortcutShowIps = new QShortcut(mGameView);
   mShortcutAddHeadlessPlayer = new QShortcut(mGameView);
   mShortcutAddHeadlessPlayerUS = new QShortcut(mGameView);
   mShortcutPaste = new QShortcut(mGameView);
   mExportObj = new QShortcut(mGameView);

   mShortcutHotkeys->setKey(Qt::Key_F1);
   mShortcutMuteMusic->setKey(Qt::Key_F2);
   mShortcutMuteSfx->setKey(Qt::Key_F3);
   mShortcutShowIps->setKey(Qt::Key_F4);
   mShortcutTrackNext->setKey(Qt::Key_PageDown);
   mShortcutTrackPrev->setKey(Qt::Key_PageUp);
   mShortcutFullscreen->setKey(Qt::ALT + Qt::Key_Return);
   mShortcutEndGame->setKey(Qt::Key_F10);
   mShortcutSavePlayback->setKey(Qt::Key_F11);
   mShortcutAddHeadlessPlayer->setKey(Qt::SHIFT + Qt::Key_Plus);
   mShortcutAddHeadlessPlayerUS->setKey(Qt::CTRL + Qt::Key_Equal);
   mShortcutPaste->setKey(Qt::CTRL + Qt::Key_V);
   mExportObj->setKey(Qt::ALT + Qt::Key_O);
}


//----------------------------------------------------------------------------
/*!
*/
BombermanClientGui::~BombermanClientGui()
{
   delete mUpdateTimer;
   delete mClient;
   delete mGameView;
}


//----------------------------------------------------------------------------
/*!
   \param fps fps
*/
void BombermanClientGui::updateFps(float fps)
{
   char temp[256];
   sprintf(temp, "Dynablaster Revenge - fps: %.02f", fps);
   mGameView->setWindowTitle( QString(temp) );
}

//----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::autoStartGamePhase1()
{
   mClient->host();
   mClient->setLoginAfterConnect(true);
   mClient->loginRequest("127.0.0.1", "coder");

   connect(
      mClient,
      SIGNAL(loginResponse(bool)),
      mClient,
      SLOT(createGameAutomatic())
   );

   connect(
      mClient,
      SIGNAL(createGameResponse(bool, int, bool)),
      mClient,
      SLOT(listGames())
   );

   connect(
      mClient,
      SIGNAL(gamesListUpdated(QList<GameInformation>)),
      this,
      SLOT(autoStartGamePhase2())
   );
}


//----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::autoStartGamePhase2()
{
   disconnect(
      mClient,
      SIGNAL(gamesListUpdated(QList<GameInformation>)),
      this,
      SLOT(autoStartGamePhase2())
   );

//   mClient->joinGame();

   connect(
      mClient,
      SIGNAL(joinGameResponse(bool)),
      mClient,
      SLOT(startGame())
   );

   connect(
      mClient,
      SIGNAL(showGame()),
      mGameView,
      SLOT(showGame())
   );
}


//----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::initSignalSlot()
{
   connect(
      mShortcutPaste,
      SIGNAL(activated()),
      this,
      SLOT(paste())
   );

   connect(
      mExportObj,
      SIGNAL(activated()),
      mGameView,
      SLOT(exportObj()),
      Qt::QueuedConnection
   );

   connect(
      this,
      SIGNAL(paste(QString)),
      mGameView->getMenuDrawable()->getMenu(),
      SLOT(paste(QString))
   );

   connect(
      mShortcutHotkeys,
      SIGNAL(activated()),
      mGameView->getHotkeyDrawable(),
      SLOT(showHotkeys())
   );

   connect(
      mShortcutHotkeys,
      SIGNAL(activated()),
      mGameView,
      SLOT(stopIdleTimer())
   );

   connect(
      mShortcutTrackNext,
      SIGNAL(activated()),
      this,
      SLOT(playNextTrack())
   );

   connect(
      mShortcutTrackPrev,
      SIGNAL(activated()),
      this,
      SLOT(playPreviousTrack())
   );

   connect(
      mShortcutShowIps,
      SIGNAL(activated()),
      BombermanClient::getInstance(),
      SLOT(showIps())
   );

   connect(
      mShortcutMuteMusic,
      SIGNAL(activated()),
      SoundManager::getInstance(),
      SLOT(toggleMuteMusic())
   );

   connect(
      mShortcutMuteSfx,
      SIGNAL(activated()),
      SoundManager::getInstance(),
      SLOT(toggleMuteSfx())
   );

   connect(
      mShortcutFullscreen,
      SIGNAL(activated()),
      this,
      SLOT(toggleFullscreen())
   );

   connect(
      mShortcutEndGame,
      SIGNAL(activated()),
      mClient,
      SLOT(stopGame())
   );

   connect(
      mShortcutSavePlayback,
      SIGNAL(activated()),
      GamePlayback::getInstance(),
      SLOT(save())
   );

   connect(
      mShortcutAddHeadlessPlayer,
      SIGNAL(activated()),
      this,
      SLOT(addHeadlessPlayer())
   );

   connect(
      mShortcutAddHeadlessPlayerUS,
      SIGNAL(activated()),
      this,
      SLOT(addHeadlessPlayer())
   );

   connect(
      mClient,
      SIGNAL(playerId(int)),
      mGameView->getGameDrawable(),
      SLOT(setPlayerId(int)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable()->getMenu(),
      SIGNAL(actionRequest(const QString&, const QString&)),
      mWorkflow,
      SLOT(actionRequest(const QString&, const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable()->getMenu(),
      SIGNAL(actionKeyPressed(const QString&, const QString&, int)),
      mWorkflow,
      SLOT(actionKeyPressed(const QString&, const QString&, int)),
      Qt::QueuedConnection
   );

   // connect menu interface to client
   connect(
      mWorkflow,
      SIGNAL(loginRequest(const QString&, const QString&)),
      mClient,
      SLOT(loginRequest(const QString&, const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(loginResponse(bool)),
      mWorkflow,
      SLOT(loginResponse(bool)),
      Qt::QueuedConnection
   );

   connect(
      mWorkflow,
      SIGNAL(mainMenuShown(bool)),
      mClient,
      SLOT(setMainMenuActive(bool))
   );

   connect(
      mClient,
      SIGNAL(hosting(bool)),
      mWorkflow,
      SLOT(serverOnline(bool)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(gamesListUpdated(const QList<GameInformation>&)),
      mWorkflow,
      SLOT(gamesListUpdated(const QList<GameInformation>&)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(createGameResponse(bool, int, bool)),
      mWorkflow,
      SLOT(createGameResponse(bool, int, bool)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable(),
      SIGNAL(visible(bool)),
      mWorkflow,
      SLOT(menuVisible(bool))
   );

   connect(
      mWorkflow,
      SIGNAL(pageChangeRequest(const QString&)),
      mGameView->getMenuDrawable(),
      SLOT(pageChangeRequest(const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable()->getMenu(),
      SIGNAL(layerFocussed(const QString&, const QString&)),
      SoundManager::getInstance(),
      SLOT(playSoundMouseOver(const QString&, const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable(),
      SIGNAL(pageChanged(const QString&)),
      SoundManager::getInstance(),
      SLOT(playSoundMouseClick(const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable(),
      SIGNAL(pageChanged(const QString&)),
      mGameView->getGameHelpDrawable(),
      SLOT(pageChanged(const QString&))
   );

   connect(
      mGameView->getMenuDrawable(),
      SIGNAL(pageChanged(const QString&)),
      mMenuHelp,
      SLOT(pageChanged(const QString&))
   );

   connect(
      mGameView->getMenuDrawable(),
      SIGNAL(pageChanged(const QString&)),
      mWorkflow,
      SLOT(pageChanged(const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getMenuDrawable(),
      //SIGNAL(pageChangeAnimationStoppedSignal()),
      SIGNAL(pageChanged(const QString&)),
      mMenuJoystickHandler,
      SLOT(focusDefaultElement())
   );

   connect(
      mWorkflow,
      SIGNAL(gameListRequest()),
      mClient,
      SLOT(gameListRequest()),
      Qt::QueuedConnection
   );

   connect(
      mWorkflow,
      SIGNAL(leaveGameRequest()),
      mClient,
      SLOT(leaveGameRequest()),
      Qt::QueuedConnection
   );

   connect(
      mWorkflow,
      SIGNAL(joinGame(int)),
      mClient,
      SLOT(joinGame(int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(joinGameResponse(bool)),
      mWorkflow,
      SLOT(joinGameResponse(bool)),
      Qt::QueuedConnection
   );

   connect(
      mWorkflow,
      SIGNAL(gameStartRequest(int)),
      mClient,
      SLOT(startGame(int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(showGame()),
      mGameView,
      SLOT(showGame()),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(showMenu()),
      mGameView,
      SLOT(showMenu())
   );

   connect(
      mClient,
      SIGNAL(showMainMenu()),
      mWorkflow,
      SLOT(showMainMenu())
   );

   connect(
      mClient,
      SIGNAL(gameStopped()),
      mGameView,
      SLOT(showMenuWithDelay()),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(disconnected()),
      mGameView,
      SLOT(showMenu()),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(disconnected()),
      mWorkflow,
      SLOT(showMainMenu()),
      Qt::QueuedConnection
   );

   connect(
      mWorkflow,
      SIGNAL(host()),
      mClient,
      SLOT(host()),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(playerInfoMapUpdated(QMap<int,PlayerInfo*>*)),
      mWorkflow,
      SLOT(playerInfoMapUpdated(QMap<int,PlayerInfo*>*)),
      Qt::QueuedConnection
   );

   // init game messaging
   connect(
      mClient,
      SIGNAL(messageReceived(int, const QString&, bool)),
      mGameView->getGameMessagingDrawable(),
      SLOT(messageReceived(int, const QString&, bool)),
      Qt::QueuedConnection
   );

   connect(
      mWorkflow,
      SIGNAL(sendMessage(const QString&,bool,int)),
      mClient,
      SLOT(sendMessage(const QString&,bool,int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(messageReceived(int, const QString&, bool)),
      mWorkflow,
      SLOT(messageReceived(int, const QString&, bool)),
      Qt::QueuedConnection
   );

   // init client/view communication
   connect(
      mClient,
      SIGNAL(loadLevel(const QString&)),
      mGameView->getGameDrawable(),
      SLOT(loadLevel(const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(levelLoaded(const QString&)),
      mClient,
      SLOT(levelLoaded(const QString&)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(shakeBlock(MapItem*)),
      mGameView->getGameDrawable(),
      SLOT(shakeBlock(MapItem*))
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(levelLoaded(const QString&)),
      GamePlayback::getInstance(),
      SLOT(levelLoadingFinished())
   );

   connect(
      GamePlayback::getInstance(),
      SIGNAL(finished()),
      mGameView,
      SLOT(restartIdleTimer())
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(levelLoadingStarted(QString)),
      mGameView->getLoadingDrawable(),
      SLOT(loadingStarted())
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(levelLoaded(const QString&)),
      mGameView->getLoadingDrawable(),
      SLOT(loadingStopped())
   );

   connect(
      mClient,
      SIGNAL(setPlayerPosition(int,float,float,float)),
      mGameView->getGameDrawable(),
      SLOT(setPlayerPosition(int,float,float,float))
   );

   connect(
      mClient,
      SIGNAL(setPlayerSpeed(int,float,float,float)),
      mGameView->getGameDrawable(),
      SLOT(setPlayerSpeed(int,float,float,float))
   );

   connect(
      mClient->getPositionInterpolation(),
      SIGNAL(setPlayerPosition(int,float,float,float)),
      mGameView->getGameDrawable(),
      SLOT(setPlayerPosition(int,float,float,float))
   );

   connect(
      mClient,
      SIGNAL(removeMapItem(MapItem*)),
      mClient->getPositionInterpolation(),
      SLOT(removeMapItem(MapItem*))
   );

   connect(
      mClient->getPositionInterpolation(),
      SIGNAL(setPlayerSpeed(int,float,float,float)),
      mGameView->getGameDrawable(),
      SLOT(setPlayerSpeed(int,float,float,float))
   );

   connect(
      mClient->getPositionInterpolation(),
      SIGNAL(setMapItemPosition(MapItem*,float,float,float)),
      mGameView->getGameDrawable(),
      SLOT(setMapItemPosition(MapItem*,float,float,float))
   );

   connect(
      mClient,
      SIGNAL(playfieldScale(float, float)),
      mGameView->getGameDrawable(),
      SLOT(setPlayfieldScale(float, float)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(playfieldSize(int,int)),
      mGameView->getGameDrawable(),
      SLOT(setPlayfieldSize(int,int)),
      Qt::QueuedConnection
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(keyPressed(QKeyEvent*)),
      mClient,
      SLOT(keyPressed(QKeyEvent*))
   );

   connect(
      mGameView->getMenuDrawable(),
      SIGNAL(keyPressed(QKeyEvent*)),
      mMenuJoystickHandler,
      SLOT(keyPressed(QKeyEvent*))
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(keyReleased(QKeyEvent*)),
      mClient,
      SLOT(keyReleased(QKeyEvent*))
   );

   connect(
      mGameView->getGameDrawable(),
      SIGNAL(releaseAllKeys()),
      mClient,
      SLOT(releaseAllKeys())
   );

   connect(
      mClient,
      SIGNAL(createMapItem(MapItem*)),
      mGameView->getGameDrawable(),
      SLOT(createMapItem(MapItem*)),
      Qt::DirectConnection
   );

   connect(
      mClient,
      SIGNAL(removeMapItem(MapItem*)),
      mGameView->getGameDrawable(),
      SLOT(removeMapItem(MapItem*)),
      Qt::DirectConnection
   );

   connect(
      mClient,
      SIGNAL(destroyMapItem(MapItem*, float)),
      mGameView->getGameDrawable(),
      SLOT(destroyMapItem(MapItem*, float)),
      Qt::DirectConnection
      );

   connect(
      mClient,
      SIGNAL(addPlayer(int, const QString&, Constants::Color)),
      mGameView->getGameDrawable(),
      SLOT(addPlayer(int, const QString&, Constants::Color)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(removePlayer(int)),
      mGameView->getGameDrawable(),
      SLOT(removePlayer(int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(extraRemoved(int,int,bool,Constants::ExtraType,int)),
      mGameView->getGameDrawable(),
      SLOT(extraRemoved(int,int,bool,Constants::ExtraType,int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(detonation(int,int,int,int,int,int, float)),
      mGameView->getGameDrawable(),
      SLOT(addDetonation(int,int,int,int,int,int, float)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(playerInfected(int,Constants::SkullType,int,int,int)),
      mGameView->getGameDrawable(),
      SLOT(playerInfected(int,Constants::SkullType,int,int,int))
   );

   connect(
      mClient,
      SIGNAL(playerInfected(int,Constants::SkullType,int,int,int)),
      mGameView->getGameStatsDrawable(),
      SLOT(playerInfected(int,Constants::SkullType,int,int,int))
   );

   // playlist
   connect(
      SoundManager::getInstance(),
      SIGNAL(musicPlaying(QString,QString,QString)),
      mGameView->getMusicPlayerDrawable(),
      SLOT(showCurrentlyPlaying(QString,QString,QString)),
      Qt::QueuedConnection
   );

   // game stats
   connect(
      mClient,
      SIGNAL(timeChanged(int, int)),
      mGameView->getGameStatsDrawable(),
      SLOT(setGameTimeLeft(int, int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(scoreChanged(int)),
      mGameView->getGameStatsDrawable(),
      SLOT(setPlayerScore(int)),
      Qt::QueuedConnection
   );

   // countdown
   connect(
      mClient,
      SIGNAL(countdown(int)),
      mGameView->getCountdownDrawable(),
      SLOT(countdown(int)),
      Qt::QueuedConnection
   );

   connect(
      mClient,
      SIGNAL(showGame()),
      mGameView->getRoundsDrawable(),
      SLOT(showGame()),
      Qt::QueuedConnection
   );

   // game menu interface
   connect(
      mWorkflow,
      SIGNAL(updateFullscreen()),
      mGameView,
      SLOT(deserializeFullscreen())
   );

   connect(
      mWorkflow,
      SIGNAL(updateShowFps()),
      mGameView,
      SLOT(deserializeShowFps())
   );

   connect(
      mWorkflow,
      SIGNAL(updateVsync()),
      this,
      SLOT(deserializeSwapInterval())
   );

   connect(
      mWorkflow,
      SIGNAL(addHeadlessPlayer()),
      this,
      SLOT(addHeadlessPlayer())
   );

   // idle
   connect(
      mGameView,
      SIGNAL(idle(bool)),
      mClient,
      SLOT(idle(bool))
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::initGameView()
{
   mGameView->setSizePolicy(
      QSizePolicy::MinimumExpanding,
      QSizePolicy::MinimumExpanding
   );

   GameSettings::VideoSettings* settings =
      GameSettings::getInstance()->getVideoSettings();

   mGameView->resize( settings->getWidth(), settings->getHeight() );

   mGameView->show();

   mGameView->deserializeFullscreen();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::initialize()
{
   // init config
   bool skipMenu =
      GameSettings::getInstance()->getDevelopmentSettings()->isSkipMenuEnabled();

   bool showSplash =
      GameSettings::getInstance()->getDevelopmentSettings()->isSplashScreenEnabled();

   if (showSplash)
      showSplashScreen();

   mClient->initialize();
   SoundManager::getInstance()->initialize();

   // init game view
   initGameView();

   if (mGameView->error())
      return;

   Menu* menu = mGameView->getMenuDrawable()->getMenu();
   menu->setMenuWorkflow(mWorkflow);

   mWorkflow->initialize();

   // init joystick integration
   initJoystickIntegration();

   // init joystick handler for menus
   mMenuJoystickHandler->initialize();

   // let the workflow take care of the change request
   initSignalSlot();

   // hide splash screen again
   hideSplashScreen();

   // hide mouse cursor
//   qApp->setOverrideCursor(QCursor(Qt::BlankCursor));

   if (skipMenu)
   {
      autoStartGamePhase1();
   }
   else
   {
      // show menu
      mGameView->showMenu();
      mGameView->getMenuDrawable()->initializationFinished();
   }

   // when everything is done, play the music
   bool shuffleMusic =
      GameSettings::getInstance()->getAudioSettings()->isShuffleMusicEnabled();

   bool shuffle1stTrackOnly =
      GameSettings::getInstance()->getAudioSettings()->isShuffle1stTrackOnlyEnabled();

   SoundManager::getInstance()->setShuffleEnabled(shuffleMusic || shuffle1stTrackOnly);
   SoundManager::getInstance()->startPlaylist();

   if (shuffle1stTrackOnly)
      SoundManager::getInstance()->setShuffleEnabled(false);

   mUpdateTimer->start();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::initJoystickIntegration()
{
   GameJoystickIntegration::initializeAll();

   // connect joystick to to game by default
   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);

   if (gji)
   {
      GameSettings::DevelopmentSettings * devSettings =
         GameSettings::getInstance()->getDevelopmentSettings();

      if (devSettings->isJoysticksEnabled())
      {
         connect(
            gji->getJoystickMapping(),
            SIGNAL(keyPressed(int)),
            mClient,
            SLOT(processKeyPressed(int))
         );

         connect(
            gji->getJoystickMapping(),
            SIGNAL(keyReleased(int)),
            mClient,
            SLOT(processKeyReleased(int))
         );

         connect(
            mClient,
            SIGNAL(rumble(float,int)),
            gji,
            SLOT(rumble(float,int))
         );
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::showSplashScreen()
{
   QPixmap pixmap("data/game/loading");
   mSplash = new QSplashScreen(mGameView, pixmap);
   mSplash->show();
   mSplash->showMessage(tr(""));
   qApp->processEvents();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::hideSplashScreen()
{
   if (mSplash)
   {
      mSplash->close();
      delete mSplash;
      mSplash = 0;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::playNextTrack()
{
   SoundManager::getInstance()->playNextTrack();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::playPreviousTrack()
{
   SoundManager::getInstance()->playPreviousTrack();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::toggleFullscreen()
{
   GameSettings::VideoSettings* videoSettings =
      GameSettings::getInstance()->getVideoSettings();

   videoSettings->toggleFullscreen();
   videoSettings->serialize();

   mGameView->deserializeFullscreen();
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::addHeadlessPlayer()
{
   GameJoystickIntegration* ji= GameJoystickIntegration::getInstance(0);

   int joystickCount = 0;
   
   if (ji)
   {
      joystickCount= ji->getJoystickInterface()->getJoystickCount();
   }

   int instanceCount = HeadlessIntegration::getInstanceCount();

   if (
         instanceCount < 8
      && instanceCount < joystickCount
      && mClient->getGameId() != -1
   )
   {
      // remove joystick support for default player
      disconnect(
         GameJoystickIntegration::getInstance(0)->getJoystickMapping(),
         SIGNAL(keyPressed(int)),
         mClient,
         SLOT(processKeyPressed(int))
      );

      disconnect(
         GameJoystickIntegration::getInstance(0)->getJoystickMapping(),
         SIGNAL(keyReleased(int)),
         mClient,
         SLOT(processKeyReleased(int))
      );

      disconnect(
         mClient,
         SIGNAL(rumble(float,int)),
         GameJoystickIntegration::getInstance(0),
         SLOT(rumble(float,int))
      );

      HeadlessIntegration* integration = new HeadlessIntegration();
      integration->setJoystickId(instanceCount);

      HeadlessClient* client = new HeadlessClient();

      integration->setClient(client);
      integration->initialize();
      integration->setGameId(mClient->getGameId());

      QString nick;
      if (instanceCount == 0)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer2Nick();
      if (instanceCount == 1)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer3Nick();
      if (instanceCount == 2)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer4Nick();
      if (instanceCount == 3)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer5Nick();
      if (instanceCount == 4)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer6Nick();
      if (instanceCount == 5)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer7Nick();
      if (instanceCount == 6)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer8Nick();
      if (instanceCount == 7)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer9Nick();
      if (instanceCount == 8)
         nick = GameSettings::getInstance()->getLoginSettings()->getPlayer10Nick();

      client->initialize();
      client->setLoginAfterConnect(true);
      client->loginRequest(mClient->getHost(), nick);

      // main player leaves game the hard way
      connect(
         mClient,
         SIGNAL(showMainMenu()),
         client,
         SLOT(deleteLater()),
         Qt::QueuedConnection
      );

      connect(
         mClient,
         SIGNAL(showMainMenu()),
         integration,
         SLOT(deleteLater()),
         Qt::QueuedConnection
      );

      // main player leaves game the soft way
      connect(
         mWorkflow,
         SIGNAL(leaveGameRequest()),
         client,
         SLOT(deleteLater()),
         Qt::QueuedConnection
      );

      connect(
         mWorkflow,
         SIGNAL(leaveGameRequest()),
         integration,
         SLOT(deleteLater()),
         Qt::QueuedConnection
      );

      connect(
         client,
         SIGNAL(socketError()),
         client,
         SLOT(deleteLater())
      );

      connect(
         client,
         SIGNAL(socketError()),
         integration,
         SLOT(deleteLater())
      );

      // restore old state when all headless instances are gone
      connect(
         integration,
         SIGNAL(destroyed()),
         this,
         SLOT(headlessPlayerRemoved())
      );

      // mGameView->getGameDrawable()->setCameraFollowingPlayer(false);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::headlessPlayerRemoved()
{
   int instanceCount = HeadlessIntegration::getInstanceCount();

   // restore original configuration
   if (instanceCount == 0)
   {
      // mGameView->getGameDrawable()->setCameraFollowingPlayer(true);

      // reconnect default player
      connect(
         GameJoystickIntegration::getInstance(0)->getJoystickMapping(),
         SIGNAL(keyPressed(int)),
         mClient,
         SLOT(processKeyPressed(int))
      );

      connect(
         GameJoystickIntegration::getInstance(0)->getJoystickMapping(),
         SIGNAL(keyReleased(int)),
         mClient,
         SLOT(processKeyReleased(int))
      );

      connect(
         mClient,
         SIGNAL(rumble(float,int)),
         GameJoystickIntegration::getInstance(0),
         SLOT(rumble(float,int))
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::paste()
{
   QClipboard *clipboard = QApplication::clipboard();
   QString text = clipboard->text();

   // qDebug("BombermanClientGui::paste(): %s", qPrintable(text));

   BitmapFont* font = FontPool::Instance()->get("default");
   QString pasteText;

   bool valid = false;

   // there shouldn't be that much stuff to paste
   if (text.length() < 100)
   {
      for (int i = 0; i < text.size(); i++)
      {
         if (font->getCharParameter(text.at(i).toLatin1()))
         {
            pasteText.append(text.at(i));
         }
      }

      if (!pasteText.isEmpty())
      {
         valid = true;
      }
   }

   if (valid)
   {
      emit paste(pasteText);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void BombermanClientGui::deserializeSwapInterval()
{
   GameSettings::VideoSettings* videoSettings =
      GameSettings::getInstance()->getVideoSettings();

   mGameView->setSwapInterval(videoSettings->getVSync());
}


