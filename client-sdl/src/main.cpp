// dynablaster_sdl - the real game entry point. Launches straight into the actual main menu with
// real window/mouse input, no CLI flags and no test/diagnostic scaffolding - that all lives in
// the separate dynablaster_sdl_harness binary (src/main_harness.cpp), which builds against the
// same dynablaster_core library. See CMakeLists.txt.
#include "gles3.h"
#include "glescontext.h"

#include "gldevice.h"

#include "tools/filestream.h"

#include "sdlglobaltime.h"

#include "game/gamelogodrawable.h"

#include "menus/bitmapfont.h"
#include "menus/fontmap.h"
#include "menus/fontpool.h"
#include "menus/menu.h"
#include "menus/menudrawable.h"
#include "menus/menumousecursor.h"
#include "menus/menupagenavigator.h"

#include "server.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QObject>

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

/// \brief registers the BitmapFonts the ported menu pages actually need (see project memory -
/// "outlined"/"time"/"large"/"large-outlined" are for in-game HUD text, not menus, and are
/// deferred until something that needs them is ported).
void registerMenuFont()
{
   BitmapFont* fontDefault =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.1f, 3.0f, 32.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.05f, -0.025f);

   BitmapFont* fontLounge =
      new BitmapFont("data/fonts/font", MenuFont::sMenuChars, 2.2f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.1f, 0.05f, -0.025f);

   FontPool::Instance()->add("default", fontDefault);
   FontPool::Instance()->add("lounge", fontLounge);
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

   // matches BombermanClient::host() (client/src/game/bombermanclient.cpp) - construct the real
   // server and confirm it actually bound its port. Unconditional for now (this port has no
   // menu-driven host()/join() wiring yet - that's the rest of Phase 4); this is just proving the
   // embedded server, copied in unmodified, comes up correctly inside client-sdl.
   Server server;
   SDL_Log("Server: isListening=%d", server.isListening() ? 1 : 0);

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

         if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
         {
            running = false;
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
               const int qtKey = mapEditingKey(event.key.key);
               if (qtKey != 0)
               {
                  QKeyEvent keyEvent(QEvent::KeyPress, qtKey, Qt::NoModifier);
                  menuDrawable.keyPressEvent(&keyEvent);
               }
               break;
            }
            case SDL_EVENT_TEXT_INPUT:
            {
               QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString::fromUtf8(event.text.text));
               menuDrawable.keyPressEvent(&keyEvent);
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

      const float timeMs = static_cast<float>(SDL_GetTicks());

      menuDrawable.animate(timeMs);
      menuDrawable.paintGL();

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

      context.swap();
   }

   return 0;
}
