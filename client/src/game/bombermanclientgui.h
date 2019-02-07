#ifndef BOMBERMANCLIENTGUI_H
#define BOMBERMANCLIENTGUI_H

#include <QObject>

// game
class BombermanClient;
class GameView;
class MenuJoystickHandler;

// menu
class GameMenuHelp;
class GameMenuInterface;
class GameMenuWorkflow;
class MenuDrawable;

// Qt
class QShortcut;
class QSplashScreen;
class QTimer;

class BombermanClientGui: public QObject
{
   Q_OBJECT

   public:

      //! constructor
      BombermanClientGui(QObject* parent = 0, float fps = 0.0f);

      //! destructor
      ~BombermanClientGui();

      //! initialize client gui
      void initialize();

      //! init signals and slots
      void initSignalSlot();

      //! init game view
      void initGameView();

      //! init joystick integration
      void initJoystickIntegration();


   signals:

      //! text has been pasted
      void paste(const QString&);


   public slots:

      //! start game without menu
      void autoStartGamePhase1();

      //! start game without menu
      void autoStartGamePhase2();

      //! read the swap interval
      void deserializeSwapInterval();


   private slots:
      
      //! show fps in the window title
      void updateFps(float fps);

      //! show splash screen
      void showSplashScreen();

      //! hide the splash screen again
      void hideSplashScreen();


      // hotkeys

      //! play next rack
      void playNextTrack();

      //! play previous track
      void playPreviousTrack();

      //! toggle fullscreen
      void toggleFullscreen();

      //! add headless client
      void addHeadlessPlayer();

      //! paste was pressed
      void paste();

   protected slots:

      //! a headless player was removed
      void headlessPlayerRemoved();


   private:

      //! client
      BombermanClient* mClient;

      //! game view
      GameView* mGameView;

      //! game implementation of menu workflow
      GameMenuWorkflow* mWorkflow;

      //! joystick support for the menu
      MenuJoystickHandler* mMenuJoystickHandler;

      //! game menu help
      GameMenuHelp* mMenuHelp;

      //! render thread
      QTimer* mUpdateTimer;

      //! loading splash screen
      QSplashScreen* mSplash;


      // shortcuts

      //! help shortcut
      QShortcut* mShortcutHotkeys;

      //! next track shortcut
      QShortcut* mShortcutTrackNext;

      //! prev track shortcut
      QShortcut* mShortcutTrackPrev;

      //! fullscreen shortcut
      QShortcut* mShortcutFullscreen;

      //! stop game
      QShortcut* mShortcutEndGame;

      //! save playback
      QShortcut* mShortcutSavePlayback;

      //! hotkey for mute music
      QShortcut* mShortcutMuteMusic;

      //! hotkey for mute sfx
      QShortcut* mShortcutMuteSfx;

      //! hotkey for show ips
      QShortcut* mShortcutShowIps;

      //! hotkey for headless test code
      QShortcut* mShortcutAddHeadlessPlayer;
      QShortcut* mShortcutAddHeadlessPlayerUS;

      //! hotkey for paste
      QShortcut* mShortcutPaste;

      //! hotkey for export current scene as obj
      QShortcut* mExportObj;
};

#endif
