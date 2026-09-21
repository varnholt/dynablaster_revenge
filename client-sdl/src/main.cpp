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

#include <QObject>

#include <SDL3/SDL.h>

namespace
{

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

int main(int, char**)
{
   GlesContext context;
   if (!context.init("Dynablaster Revenge", 800, 600))
   {
      return 1;
   }

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

   bool running = true;

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
            default:
               break;
         }
      }

      device.clear();

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
         // GameLogoDrawable's own fade/spark timing (FADE_IN_LENGTH etc.) is calibrated against
         // the original engine's Drawable::animate() convention (real seconds * 62.5, see
         // client/src/game/bombermanview.cpp) - timeMs is real milliseconds, so convert
         // (ms/1000)*62.5 == ms*0.0625 to keep the fade durations meaning what they say. The
         // earth/bomb sphere's own rotation reads GlobalTime directly (see
         // SphereFragmentsDrawable/SphereGeometryVbo) - update it here too so the logo actually
         // animates during play.
         globalTime.update();
         logoDrawable.animate(timeMs * 0.0625f);
         logoDrawable.paintGL();
      }

      context.swap();
   }

   return 0;
}
