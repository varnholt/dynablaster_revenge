// dynablaster_sdl - the real game entry point. Launches straight into the actual main menu with
// real window/mouse input, no CLI flags and no test/diagnostic scaffolding - that all lives in
// the separate dynablaster_sdl_harness binary (src/main_harness.cpp), which builds against the
// same dynablaster_core library. See CMakeLists.txt.
#include "gles3.h"
#include "glescontext.h"

#include "gldevice.h"

#include "tools/filestream.h"

#include "sdlglobaltime.h"

#include "framework/timerhandler.h"

#include "game/countdowndrawable.h"
#include "game/gamedrawable.h"
#include "game/gamelogodrawable.h"
#include "game/gamewindrawable.h"
#include "game/soundmanager.h"

#include "menus/bitmapfont.h"
#include "menus/fontmap.h"
#include "menus/fontpool.h"
#include "menus/menu.h"
#include "menus/menudrawable.h"
#include "menus/menumousecursor.h"
#include "menus/menupagenavigator.h"

#include "game/bombermanclient.h"
#include "game/positioninterpolation.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>

#include <SDL3/SDL.h>

namespace
{

/// \brief maps the editing/navigation keys MenuPageTextEditItem::keyPressed() special-cases
/// (menus/menupagetextedit.cpp) to Qt::Key. Printable characters don't go through this - they
/// come from SDL_EVENT_TEXT_INPUT instead, which already gives correctly-shifted/composed text.
int mapEditingKey(SDL_Keycode key)
{
   switch (key)
   {
      case SDLK_BACKSPACE:
         return Qt::Key_Backspace;
      case SDLK_DELETE:
         return Qt::Key_Delete;
      case SDLK_LEFT:
         return Qt::Key_Left;
      case SDLK_RIGHT:
         return Qt::Key_Right;
      case SDLK_HOME:
         return Qt::Key_Home;
      case SDLK_END:
         return Qt::Key_End;
      case SDLK_RETURN:
         return Qt::Key_Return;
      case SDLK_KP_ENTER:
         return Qt::Key_Enter;
      default:
         return 0;
   }
}

/// \brief maps the movement/bomb/zoom/start keys BombermanClient::keyPressed() checks against
/// GameSettings::ControllerSettings' default keymap (client/src/game/gamesettings.cpp -
/// initializeDefaultMap(): Up/Down/Left/Right arrows, Space for bomb, [ ] for zoom, F10 for
/// start), plus Return/Enter/Escape - not part of that remappable keymap, but hardcoded special
/// cases BombermanClient::processKeyPressed() checks directly (client/src/game/bombermanclient.cpp:
/// Return/Enter toggles in-game chat, Escape closes chat if open or otherwise leaves the game).
/// Both were missing here entirely, so neither ever reached BombermanClient while in-game.
/// Separate from mapEditingKey() below (menu text-field navigation) - only one of
/// menuDrawable/gameDrawable is visible and receives key events at a time, so reusing the same
/// Qt::Key values for both is fine.
int mapGameKey(SDL_Keycode key)
{
   switch (key)
   {
      case SDLK_UP:
         return Qt::Key_Up;
      case SDLK_DOWN:
         return Qt::Key_Down;
      case SDLK_LEFT:
         return Qt::Key_Left;
      case SDLK_RIGHT:
         return Qt::Key_Right;
      case SDLK_SPACE:
         return Qt::Key_Space;
      case SDLK_LEFTBRACKET:
         return Qt::Key_BracketLeft;
      case SDLK_RIGHTBRACKET:
         return Qt::Key_BracketRight;
      case SDLK_F10:
         return Qt::Key_F10;
      case SDLK_RETURN:
         return Qt::Key_Return;
      case SDLK_KP_ENTER:
         return Qt::Key_Enter;
      case SDLK_ESCAPE:
         return Qt::Key_Escape;
      case SDLK_TAB:
         return Qt::Key_Tab;
      default:
         return 0;
   }
}

/// \brief registers the BitmapFonts the ported menu pages + in-game HUD actually need (see
/// project memory - "time" is for HUD text nothing here uses yet, and stays deferred until
/// something needs it).
void registerMenuFont()
{
   BitmapFont* fontDefault =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.1f, 3.0f, 32.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.05f, -0.025f);

   BitmapFont* fontLounge =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.2f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.1f, 0.05f, -0.025f);

   // Tab overlay (GamePlayerNameDisplay) - matches the original's own outline params exactly
   // (client/src/game/bombermanview.cpp).
   BitmapFont* fontOutlined =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.0f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.35f, 0.03f, -0.025f);

   // win/trophy screen (GameWinDrawable) - "wins!"/"draw game" headline and the scoreboard rows.
   BitmapFont* fontLarge =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.0f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.04f, 0.0025f, -0.01f);

   BitmapFont* fontLargeOutlined =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.0f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.15f, 0.03f, -0.025f);

   FontPool::Instance()->add("default", fontDefault);
   FontPool::Instance()->add("lounge", fontLounge);
   FontPool::Instance()->add("outlined", fontOutlined);
   FontPool::Instance()->add("large", fontLarge);
   FontPool::Instance()->add("large-outlined", fontLargeOutlined);
}

}  // namespace

int main(int argc, char** argv)
{
   // Server (server/src/server.h, copied in as-is from the old client's embedded server - see
   // client.pro's own ../server/src SOURCES) needs a running Qt event loop to ever fire
   // QTcpServer::newConnection/readyRead - there was no QCoreApplication anywhere in this port
   // before now. Constructed once, pumped once per frame below (processEvents()) rather than
   // handing control to qApp->exec(), since SDL already owns the main loop here.
   QCoreApplication qtApp(argc, argv);

   // BombermanClient (client/src/game/bombermanclient.cpp, copied in as-is - see project memory,
   // Phase 4) owns Server construction itself now, on demand via host() (matches the real
   // client/src/game/bombermanclientgui.cpp construction order - "new BombermanClient()" then
   // initialize()). Superseded the earlier always-on "Server server;" proof-of-concept from before
   // BombermanClient existed in this port.
   BombermanClient bombermanClient;
   bombermanClient.initialize();

   GlesContext context;
   // matches the real original client's own DEFAULT_VIDEO_WIDTH/HEIGHT (client/src/game/gamesettings.cpp)
   // - 16:9, same aspect as the menu system's own 1920x1080 page space (mainmenu.psd etc.).
   if (!context.init("Dynablaster Revenge", 1024, 576))
   {
      return 1;
   }

   // needed for SDL_EVENT_TEXT_INPUT - off by default in SDL3.
   SDL_StartTextInput(context.window());

   // shaders (loaded by GLDevice::loadShader) and textures (loaded by Image via loadtga) are
   // both plain FileStream reads, resolved against these search paths rather than a hardcoded
   // prefix on every call site.
   FileStream::addPath("data/shaders");
   FileStream::addPath("data/textures");
   FileStream::addPath("data/game");
   FileStream::addPath("data/logo");  // GameLogoPointSprite's "pointsprite" texture

   // constructing a RenderDevice sets the global activeDevice pointer (see renderdevice.cpp) -
   // every Material/VertexBuffer/etc call below goes through this.
   GLDevice device;
   device.init();
   device.resize(context.width(), context.height());
   device.setCulling(false);  // materials normally drive this per-draw via getCulling(); not wired up yet

   SDL_Log("GL_VERSION: %s", glGetString(GL_VERSION));
   SDL_Log("GL_RENDERER: %s", glGetString(GL_RENDERER));
   SDL_Log("GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

   // materials that animate off a running clock (DisplacementMaterial's flag-wave shader,
   // SphereFragmentsDrawable's earth/bomb rotation) read GlobalTime::Instance() - the original
   // game got that from MainDrawable (the QGLWidget itself, see client/src/framework/maindrawable.h);
   // this is that clock's replacement here.
   SdlGlobalTime globalTime;

   registerMenuFont();

   MenuDrawable menuDrawable(&device);
   menuDrawable.initializeGL();
   menuDrawable.setVisible(true);

   MenuMouseCursor menuCursor(&device);
   menuCursor.initializeGL();
   menuCursor.setVisible(true);

   // the menu draws its own cursor (above, MenuMouseCursor) - hide the OS cursor so the two
   // don't overlap on screen.
   SDL_HideCursor();

   // the animated main-menu logo (sphere-fragments earth/bomb effect + "Dynablaster"/"Revenge"
   // PSD text overlay + spark sparks) - only actually visible while the main menu page is
   // showing (see GameLogoDrawable::pageChanged()), matching the original's own
   // showMenuShowEnableMenu()/showGame() wiring (client/src/game/bombermanview.cpp).
   GameLogoDrawable logoDrawable(&device);
   logoDrawable.initializeGL();
   logoDrawable.setVisible(true);
   QObject::connect(&menuDrawable, SIGNAL(pageChanged(QString)), &logoDrawable, SLOT(pageChanged(QString)));

   // turns button clicks (Menu::actionRequest) into actual page navigation - see
   // menupagenavigator.h for exactly what this does and doesn't handle yet (no networking).
   // pageChangeRequest connects to MenuDrawable's *protected* slot of the same name - legal via
   // Qt's string-based SIGNAL/SLOT connect, which bypasses C++ access control; this is the same
   // connection the real client/src/game/bombermanclientgui.cpp makes for GameMenuWorkflow.
   MenuPageNavigator navigator;
   QObject::connect(menuDrawable.getMenu(), SIGNAL(actionRequest(QString, QString)), &navigator, SLOT(onActionRequest(QString, QString)));
   QObject::connect(&navigator, SIGNAL(pageChangeRequest(QString)), &menuDrawable, SLOT(pageChangeRequest(QString)));

   // matches GameMenuWorkflow::pageChanged() - populates GAME_CREATE's dropdowns/checkboxes once
   // the page actually becomes current (see MenuPageNavigator::onPageChanged()).
   QObject::connect(&menuDrawable, SIGNAL(pageChanged(QString)), &navigator, SLOT(onPageChanged(QString)));

   // menu hover/click sound feedback - matches bombermanclientgui.cpp's own wiring.
   QObject::connect(
      menuDrawable.getMenu(),
      SIGNAL(layerFocussed(QString, QString)),
      SoundManager::getInstance(),
      SLOT(playSoundMouseOver(QString, QString)),
      Qt::QueuedConnection
   );
   QObject::connect(
      &menuDrawable,
      SIGNAL(pageChanged(QString)),
      SoundManager::getInstance(),
      SLOT(playSoundMouseClick(QString)),
      Qt::QueuedConnection
   );

   // GameDrawable (Phase 5, see project memory) - the real in-game rendering (map/players/bombs/
   // extras). Starts hidden; BombermanClient::showGame()/showMenu() (see below) toggle it on/off
   // against the menu, matching client/src/game/bombermanview.cpp's GameView::showGame()/
   // showMenu() (that class itself isn't ported - main.cpp already does its job of owning/
   // dispatching to each Drawable directly, same as it already does for the menu system).
   GameDrawable gameDrawable(&device);
   gameDrawable.initializeGL();
   gameDrawable.setVisible(false);

   // pre-round countdown HUD overlay (client/src/game/countdowndrawable.cpp) - drawn on top of
   // GameDrawable each frame while a countdown is in progress, matching GameView's own draw
   // order (CountdownDrawable is constructed/registered last in the original's GameView, so it
   // renders after GameDrawable's own main scene).
   CountdownDrawable countdownDrawable(&device);
   countdownDrawable.initializeGL();

   // win/trophy screen (client/src/game/gamewindrawable.cpp) - self-governs its own visibility
   // via GameStateMachine's stateChanged signal (connected in its own constructor), so it needs
   // no explicit wiring here beyond construction + the animate/paintGL calls below.
   GameWinDrawable gameWinDrawable(&device);
   gameWinDrawable.initializeGL();

   // client<->game wiring - mirrors client/src/game/bombermanclientgui.cpp's
   // BombermanClientGui::initConnections() (only the connections relevant to what's actually
   // ported here; chat/stats/rounds/music-player/joystick wiring is still out of scope).
   QObject::connect(&bombermanClient, SIGNAL(loadLevel(QString)), &gameDrawable, SLOT(loadLevel(QString)));
   QObject::connect(&gameDrawable, SIGNAL(levelLoaded(QString)), &bombermanClient, SLOT(levelLoaded(QString)));
   QObject::connect(&bombermanClient, SIGNAL(shakeBlock(MapItem*)), &gameDrawable, SLOT(shakeBlock(MapItem*)));
   QObject::connect(&bombermanClient, SIGNAL(setPlayerPosition(int,float,float,float)), &gameDrawable, SLOT(setPlayerPosition(int,float,float,float)));
   QObject::connect(&bombermanClient, SIGNAL(setPlayerSpeed(int,float,float,float)), &gameDrawable, SLOT(setPlayerSpeed(int,float,float,float)));
   QObject::connect(bombermanClient.getPositionInterpolation(), SIGNAL(setPlayerPosition(int,float,float,float)), &gameDrawable, SLOT(setPlayerPosition(int,float,float,float)));
   QObject::connect(bombermanClient.getPositionInterpolation(), SIGNAL(setPlayerSpeed(int,float,float,float)), &gameDrawable, SLOT(setPlayerSpeed(int,float,float,float)));
   QObject::connect(bombermanClient.getPositionInterpolation(), SIGNAL(setMapItemPosition(MapItem*,float,float,float)), &gameDrawable, SLOT(setMapItemPosition(MapItem*,float,float,float)));
   QObject::connect(&bombermanClient, SIGNAL(removeMapItem(MapItem*)), bombermanClient.getPositionInterpolation(), SLOT(removeMapItem(MapItem*)));
   QObject::connect(&bombermanClient, SIGNAL(playfieldScale(float,float)), &gameDrawable, SLOT(setPlayfieldScale(float,float)));
   QObject::connect(&bombermanClient, SIGNAL(playfieldSize(int,int)), &gameDrawable, SLOT(setPlayfieldSize(int,int)));
   QObject::connect(&gameDrawable, SIGNAL(keyPressed(QKeyEvent*)), &bombermanClient, SLOT(keyPressed(QKeyEvent*)));
   QObject::connect(&gameDrawable, SIGNAL(keyReleased(QKeyEvent*)), &bombermanClient, SLOT(keyReleased(QKeyEvent*)));
   QObject::connect(&bombermanClient, SIGNAL(createMapItem(MapItem*)), &gameDrawable, SLOT(createMapItem(MapItem*)));
   QObject::connect(&bombermanClient, SIGNAL(removeMapItem(MapItem*)), &gameDrawable, SLOT(removeMapItem(MapItem*)));
   QObject::connect(&bombermanClient, SIGNAL(destroyMapItem(MapItem*,float)), &gameDrawable, SLOT(destroyMapItem(MapItem*,float)));
   QObject::connect(&bombermanClient, SIGNAL(addPlayer(int,QString,Constants::Color)), &gameDrawable, SLOT(addPlayer(int,QString,Constants::Color)));
   QObject::connect(&bombermanClient, SIGNAL(removePlayer(int)), &gameDrawable, SLOT(removePlayer(int)));
   QObject::connect(&bombermanClient, SIGNAL(extraRemoved(int,int,bool,Constants::ExtraType,int)), &gameDrawable, SLOT(extraRemoved(int,int,bool,Constants::ExtraType,int)));
   QObject::connect(&bombermanClient, SIGNAL(detonation(int,int,int,int,int,int,float)), &gameDrawable, SLOT(addDetonation(int,int,int,int,int,int,float)));
   QObject::connect(&bombermanClient, SIGNAL(playerInfected(int,Constants::SkullType,int,int,int)), &gameDrawable, SLOT(playerInfected(int,Constants::SkullType,int,int,int)));
   QObject::connect(&bombermanClient, SIGNAL(playerId(int)), &gameDrawable, SLOT(setPlayerId(int)));
   QObject::connect(&bombermanClient, SIGNAL(countdown(int)), &countdownDrawable, SLOT(countdown(int)));

   // menu<->game visibility switch - matches GameView::showGame()/showMenu() exactly (minus the
   // still-deferred GameStatsDrawable/GameMessagingDrawable/MusicPlayerDrawable/GameHelpDrawable).
   QObject::connect(&bombermanClient, &BombermanClient::showGame, [&]() {
      menuDrawable.setVisible(false);
      logoDrawable.setVisible(false);
      menuCursor.setVisible(false);
      gameDrawable.setVisible(true);
   });
   auto showMenuAgain = [&]() {
      gameDrawable.setVisible(false);
      countdownDrawable.setVisible(false);
      menuDrawable.setVisible(true);
      logoDrawable.setVisible(true);
      menuCursor.setVisible(true);
   };
   QObject::connect(&bombermanClient, &BombermanClient::showMenu, showMenuAgain);
   // matches GameView::showMenuWithDelay(): a round ending naturally shows the win/trophy screen
   // first (GameWinDrawable, still rendered on top of the - now blurred - game scene), only
   // switching to the menu once its own fade-out sequence finishes. showMenu() above (early
   // leave/ESC, no valid game id) stays an immediate switch - matches GameWinDrawable's own
   // isGameIdValid() gate, which skips showing itself in exactly that case.
   QObject::connect(&bombermanClient, &BombermanClient::gameStopped, [&]() {
      QTimer::singleShot(SHOW_WINNER_TIME_SUM, showMenuAgain);
   });
   // pageChangeRequest is a protected slot (see the navigator wiring above) - invokeMethod goes
   // through Qt's meta-object system, bypassing C++ access control the same way the string-based
   // SIGNAL/SLOT connects elsewhere in this file already do.
   QObject::connect(&bombermanClient, &BombermanClient::showMainMenu, [&]() {
      QMetaObject::invokeMethod(&menuDrawable, "pageChangeRequest", Q_ARG(QString, QString("data/menus/mainmenu.psd")));
   });

   // matches BombermanClientGui's own startup sequence (SoundManager::getInstance()->
   // startPlaylist(), called once real init is done) - background music.
   SoundManager::getInstance()->startPlaylist();

   bool running = true;
   QObject::connect(&navigator, &MenuPageNavigator::quitRequest, [&running]() { running = false; });

   while (running)
   {
      SDL_Event event{};
      while (SDL_PollEvent(&event))
      {
         if (event.type == SDL_EVENT_QUIT)
         {
            running = false;
         }

         // Alt+Enter toggles fullscreen - matches the original's own global QShortcut
         // (client/src/game/bombermanclientgui.cpp: mShortcutFullscreen->setKey(Qt::ALT +
         // Qt::Key_Return)), works regardless of menu/game state, same as there.
         if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN && (event.key.mod & SDL_KMOD_ALT))
         {
            const bool isFullscreen = (SDL_GetWindowFlags(context.window()) & SDL_WINDOW_FULLSCREEN) != 0;
            SDL_SetWindowFullscreen(context.window(), !isFullscreen);
            continue;  // don't also forward the plain Return key to the menu/game below
         }

         // nothing previously re-queried the window's actual pixel size or re-ran
         // device.resize() after startup, so a fullscreen toggle (or any window resize) kept
         // rendering into the old, smaller viewport in a corner of the now-larger window.
         if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
         {
            context.updateSize();
            device.resize(context.width(), context.height());
         }

         // menu items live in 1920x1080 page space (see mainmenu.psd/background.psd), not window
         // space - the original (client/src/game/bombermanview.cpp) converted every mouse event
         // through activeDevice->convertFromViewPort() before handing it to a Drawable, and this
         // port needs the exact same conversion now that main.cpp is doing bombermanview's old
         // job of dispatching events to each Drawable directly.
         switch (event.type)
         {
            case SDL_EVENT_MOUSE_MOTION:
            {
               int x = static_cast<int>(event.motion.x);
               int y = static_cast<int>(event.motion.y);
               device.convertFromViewPort(&x, &y, 1920, 1080);
               menuDrawable.mouseMoveEvent(x, y);
               menuCursor.mouseMoveEvent(x, y);
               break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
               int x = static_cast<int>(event.button.x);
               int y = static_cast<int>(event.button.y);
               device.convertFromViewPort(&x, &y, 1920, 1080);
               menuDrawable.mousePressEvent(x, y);
               menuCursor.mousePressEvent(x, y);
               break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
               menuDrawable.mouseReleaseEvent(nullptr);
               menuCursor.mouseReleaseEvent(nullptr);
               break;
            case SDL_EVENT_KEY_DOWN:
            {
               if (gameDrawable.isVisible())
               {
                  const int qtKey = mapGameKey(event.key.key);
                  if (qtKey != 0)
                  {
                     QKeyEvent keyEvent(QEvent::KeyPress, qtKey, Qt::NoModifier, QString(), event.key.repeat);
                     gameDrawable.keyPressEvent(&keyEvent);
                  }
               }
               else
               {
                  const int qtKey = mapEditingKey(event.key.key);
                  if (qtKey != 0)
                  {
                     QKeyEvent keyEvent(QEvent::KeyPress, qtKey, Qt::NoModifier);
                     menuDrawable.keyPressEvent(&keyEvent);
                  }
               }
               break;
            }
            case SDL_EVENT_KEY_UP:
            {
               if (gameDrawable.isVisible())
               {
                  const int qtKey = mapGameKey(event.key.key);
                  if (qtKey != 0)
                  {
                     QKeyEvent keyEvent(QEvent::KeyRelease, qtKey, Qt::NoModifier, QString(), event.key.repeat);
                     gameDrawable.keyReleaseEvent(&keyEvent);
                  }
               }
               break;
            }
            case SDL_EVENT_TEXT_INPUT:
            {
               if (!gameDrawable.isVisible())
               {
                  QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString::fromUtf8(event.text.text));
                  menuDrawable.keyPressEvent(&keyEvent);
               }
               break;
            }
            default:
               break;
         }
      }

      // pumps Server's QTcpServer/QTimer signals (newConnection, readyRead, ...) - see the
      // QCoreApplication comment above main().
      QCoreApplication::processEvents();

      device.clear();

      // must run before menuDrawable.paintGL() - the page cross-fade animation reads GlobalTime
      // (via FrameTimer), so updating it after paintGL() makes every frame's fade calc use last
      // frame's stale time instead of this frame's.
      globalTime.update();

      // drives every FrameTimer's timeout() signal (client/src/game/bombermanview.cpp:385's
      // TimerHandler::Instance()->update() - never carried over to this port). Without this,
      // FrameTimer::start()'s timer never fires at all - broke PositionInterpolation's own
      // FrameTimer-based update loop, which is why kicked bombs never visually moved.
      TimerHandler::Instance()->update();

      const float timeMs = static_cast<float>(SDL_GetTicks());

      if (menuDrawable.isVisible())
      {
         menuDrawable.animate(timeMs);
         menuDrawable.paintGL();
      }

      if (menuCursor.isVisible())
      {
         menuCursor.animate(timeMs);
         menuCursor.paintGL();
      }

      if (logoDrawable.isVisible())
      {
         // real seconds * 62.5, matching client/src/game/bombermanview.cpp's Drawable::animate() convention.
         logoDrawable.animate(timeMs * 0.0625f);
         logoDrawable.paintGL();
      }

      if (gameDrawable.isVisible())
      {
         // real seconds * 62.5, matching client/src/game/bombermanview.cpp's Drawable::animate() convention.
         gameDrawable.animate(timeMs * 0.0625f);
         gameDrawable.paintGL();
      }

      if (countdownDrawable.isVisible())
      {
         // real seconds * 62.5, matching client/src/game/bombermanview.cpp's Drawable::animate() convention.
         countdownDrawable.animate(timeMs * 0.0625f);
         countdownDrawable.paintGL();
      }

      if (gameWinDrawable.isVisible())
      {
         // real seconds * 62.5, matching client/src/game/bombermanview.cpp's Drawable::animate() convention.
         gameWinDrawable.animate(timeMs * 0.0625f);
         gameWinDrawable.paintGL();
      }

      context.swap();
   }

   return 0;
}
