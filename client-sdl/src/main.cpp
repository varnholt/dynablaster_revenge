#include "gles3.h"
#include "glescontext.h"
#include "inputinjector.h"
#include "screenshot.h"

#include "gldevice.h"

#include "tools/filestream.h"

#include "demofactory.h"
#include "sdlglobaltime.h"

#include "engine/nodes/scenegraph.h"

#include "effects/spherefragments/spherefragmentsdrawable.h"
#include "game/gamelogodrawable.h"

#include "image/image.h"
#include "menus/bitmapfont.h"
#include "menus/fontmap.h"
#include "menus/fontpool.h"
#include "menus/menu.h"
#include "menus/menudrawable.h"
#include "menus/menumousecursor.h"
#include "menus/menupage.h"
#include "menus/menupageeditablecomboboxitem.h"

#include <QObject>

#include <SDL3/SDL.h>

#include <cstdlib>
#include <string>
#include <vector>

namespace
{

/// \brief logs Menu::actionRequest so scripted button-click selftests (see --click/--dumplayer)
/// have real, observable proof that MenuPageItem::activated() -> action(QString) ->
/// MenuPage::actionRequestFromItem -> Menu::actionRequest fired, not just "no crash". Real
/// GameMenuWorkflow (the actual button->network-request handler) is a later phase - this only
/// verifies the menu-item click pipeline itself.
class ActionLogger : public QObject
{
   Q_OBJECT

public slots:

   void onActionRequest(const QString& page, const QString& action)
   {
      SDL_Log("Menu::actionRequest: page=%s action=%s", qPrintable(page), qPrintable(action));
   }
};

/// \brief the scripted key sequence a headless --selftest run feeds into the game.
std::vector<InputInjector::Step> makeSelftestScript()
{
   return {
      {SDLK_RIGHT, 10},
      {SDLK_DOWN, 10},
      {SDLK_SPACE, 5},
   };
}

std::string argValue(const std::vector<std::string>& args, const std::string& prefix)
{
   for (const auto& arg : args)
   {
      if (arg.rfind(prefix, 0) == 0)
      {
         return arg.substr(prefix.size());
      }
   }

   return {};
}

bool hasFlag(const std::vector<std::string>& args, const std::string& flag)
{
   for (const auto& arg : args)
   {
      if (arg == flag)
      {
         return true;
      }
   }

   return false;
}

/// \brief registers the BitmapFonts the ported menu pages actually need (see project memory -
/// "outlined"/"time"/"large"/"large-outlined" are for in-game HUD text, not menus, and are
/// deferred until something that needs them is ported). "lounge" is needed now: page_4's
/// table_lounge_font_name = lounge in menu.ini.
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
   const std::vector<std::string> args(argv + 1, argv + argc);
   const bool selftest = hasFlag(args, "--selftest");
   const bool menuMode = hasFlag(args, "--menu");
   // isolated verification harness for the sphere-fragments "exploding earth/bomb" effect
   // (GameLogoDrawable's real engine) before it gets wired into the actual menu composition -
   // see project memory on GameLogoDrawable scoping.
   const bool logo3dMode = hasFlag(args, "--logo3d");
   const std::string screenshot_path = argValue(args, "--screenshot=");
   const std::string click_arg = argValue(args, "--click=");
   const std::string dump_layer = argValue(args, "--dumplayer=");
   const std::string page_name = argValue(args, "--page=");

   GlesContext context;
   if (!context.init("dynablaster (SDL3 + GLES3)", 800, 600))
   {
      return 1;
   }

   // shaders (loaded by GLDevice::loadShader) and textures (loaded by Image via loadtga) are
   // both plain FileStream reads, resolved against these search paths rather than a hardcoded
   // prefix on every call site.
   FileStream::addPath("data/shaders");
   FileStream::addPath("data/textures");
   FileStream::addPath("data/game");
   FileStream::addPath("data/level-castle");
   FileStream::addPath("data/logo");  // GameLogoPointSprite's "pointsprite" texture

   // constructing a RenderDevice sets the global activeDevice pointer (see renderdevice.cpp) -
   // every Material/VertexBuffer/etc call below goes through this.
   GLDevice device;
   device.init();
   device.resize(context.width(), context.height());
   device.setCulling(false);  // materials normally drive this per-draw via getCulling(); this demo doesn't wire that up

   SDL_Log("GL_VERSION: %s", glGetString(GL_VERSION));
   SDL_Log("GL_RENDERER: %s", glGetString(GL_RENDERER));
   SDL_Log("GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

   InputInjector injector;
   if (selftest && !menuMode && !logo3dMode)
   {
      injector.queue(makeSelftestScript());
   }

   // materials that animate off a running clock (DisplacementMaterial's flag-wave shader) read
   // GlobalTime::Instance() - the original game got that from MainDrawable (the QGLWidget
   // itself, see client/src/framework/maindrawable.h); this is that clock's replacement here.
   SdlGlobalTime globalTime;

   // real level data: the actual castle level (client/data/level-castle/level.hjb) with its
   // real textures, loaded through the real (ported, not reimplemented) SceneGraph::load().
   // DemoMaterialFactory mirrors the real material-ID dispatch the original game used for this
   // exact level (LevelCastle::createMaterial) - unlike the block.hjb prop, this level ships its
   // own Camera node, so SceneGraph::setupCamera() has real data to frame instead of falling
   // back to the identity camera.
   //
   // Skipped entirely in --menu mode: the menu-system demo below renders through the same
   // GLDevice/activeDevice but has nothing to do with the level scene.
   SceneGraph scene;
   DemoMaterialFactory factory;
   MenuDrawable* menuDrawable = nullptr;
   MenuMouseCursor* menuCursor = nullptr;
   GameLogoDrawable* logoDrawable = nullptr;
   SphereFragmentsDrawable* logo3d = nullptr;

   if (logo3dMode)
   {
      logo3d = new SphereFragmentsDrawable(&device);
      logo3d->initializeGL();
      logo3d->setVisible(true);
   }
   else if (menuMode)
   {
      registerMenuFont();

      menuDrawable = new MenuDrawable(&device);
      menuDrawable->initializeGL();
      menuDrawable->setVisible(true);

      menuCursor = new MenuMouseCursor(&device);
      menuCursor->initializeGL();
      menuCursor->setVisible(true);

      // the menu draws its own cursor (above, MenuMouseCursor) - hide the OS cursor so the two
      // don't overlap on screen.
      SDL_HideCursor();

      // the animated main-menu logo (sphere-fragments earth/bomb effect + "Dynablaster"/"Revenge"
      // PSD text overlay + spark sparks) - only actually visible while the main menu page is
      // showing (see GameLogoDrawable::pageChanged()), matching the original's own
      // showMenuShowEnableMenu()/showGame() wiring (client/src/game/bombermanview.cpp).
      logoDrawable = new GameLogoDrawable(&device);
      logoDrawable->initializeGL();
      logoDrawable->setVisible(true);
      QObject::connect(menuDrawable, SIGNAL(pageChanged(QString)), logoDrawable, SLOT(pageChanged(QString)));

      // --page=<psd path>: jump straight to a real page for a static verification screenshot,
      // bypassing full click-driven navigation (which needs GameMenuWorkflow - a later phase).
      // Real page switches also drive a fade animation via MenuDrawable's page-change path; this
      // just flips isActive() directly since a static screenshot doesn't need the transition.
      if (!page_name.empty())
      {
         MenuPage* targetPage = menuDrawable->getMenu()->getPageByName(page_name.c_str());
         if (targetPage)
         {
            MenuPage* previous = menuDrawable->getMenu()->getCurrentPage();
            if (previous && previous != targetPage)
            {
               previous->setActive(false);
            }
            targetPage->setActive(true);
            menuDrawable->getMenu()->setCurrentPage(targetPage);
         }
         else
         {
            SDL_Log("--page=%s: no such page", page_name.c_str());
         }
      }

      // --dumplayer=<name>: print a page_1 PSD layer's page-space bounds and exit, used to find
      // click coordinates for scripted --click tests without guessing (MenuPage is-a PSD, so its
      // own layers - not just page items - are queryable by name).
      if (!dump_layer.empty())
      {
         PSD::Layer* layer = menuDrawable->getMenu()->getCurrentPage()->getLayer(dump_layer.c_str());
         if (!layer)
         {
            layer = menuDrawable->getMenu()->getBackground()->getLayer(dump_layer.c_str());
         }
         if (layer)
         {
            SDL_Log(
               "layer %s: left=%d top=%d width=%d height=%d center=(%d,%d) opacity=%d",
               dump_layer.c_str(),
               layer->getLeft(),
               layer->getTop(),
               layer->getWidth(),
               layer->getHeight(),
               layer->getLeft() + layer->getWidth() / 2,
               layer->getTop() + layer->getHeight() / 2,
               layer->getOpacity()
            );

            // temporary diagnostic: sample decoded pixel alpha at several points to check for a
            // real PSD alpha-channel decode bug vs. a shader/blending issue (2026-09-20).
            Image* image = layer->getImage();
            if (image)
            {
               const int w = image->getWidth();
               const int h = image->getHeight();
               const int samples[][2] = {{w / 2, h / 2}, {w / 4, h / 4}, {(3 * w) / 4, (3 * h) / 4}, {5, 5}, {w - 5, h - 5}};
               for (auto& s : samples)
               {
                  unsigned int px = image->getScanline(s[1])[s[0]];
                  SDL_Log(
                     "  pixel(%d,%d): a=%d r=%d g=%d b=%d", s[0], s[1], (px >> 24) & 0xff, (px >> 16) & 0xff, (px >> 8) & 0xff, px & 0xff
                  );
               }

               // scan row 0 for the first non-zero-alpha pixel and print its color.
               unsigned int* row0 = image->getScanline(0);
               for (int x = 0; x < w; x++)
               {
                  unsigned int px = row0[x];
                  unsigned int a = (px >> 24) & 0xff;
                  if (a > 0)
                  {
                     SDL_Log(
                        "  first non-zero-alpha in row0 at x=%d: a=%d r=%d g=%d b=%d", x, a, (px >> 16) & 0xff, (px >> 8) & 0xff, px & 0xff
                     );
                     break;
                  }
               }

               // count how many pixels in row 0 have non-zero alpha, for density context.
               int nonZero = 0;
               for (int x = 0; x < w; x++)
                  if (((row0[x] >> 24) & 0xff) > 0)
                     nonZero++;
               SDL_Log("  row0 non-zero-alpha pixel count: %d / %d", nonZero, w);

               // row 0 might just be a sparse top margin - check density + an example color at
               // several more representative rows across the image.
               for (int checkY : {100, 300, 540, 800, 1000})
               {
                  unsigned int* row = image->getScanline(checkY);
                  int count = 0;
                  unsigned int examplePixel = 0;
                  for (int x = 0; x < w; x++)
                  {
                     if (((row[x] >> 24) & 0xff) > 0)
                     {
                        count++;
                        if (examplePixel == 0)
                           examplePixel = row[x];
                     }
                  }
                  SDL_Log(
                     "  row %d: non-zero count=%d/%d example a=%d r=%d g=%d b=%d",
                     checkY,
                     count,
                     w,
                     (examplePixel >> 24) & 0xff,
                     (examplePixel >> 16) & 0xff,
                     (examplePixel >> 8) & 0xff,
                     examplePixel & 0xff
                  );
               }
            }

            // check the actual GL texture id the render path uses for this item, vs. a known-good
            // one, to rule out a texture-id mixup between the CPU decode (checked above) and GPU.
            MenuPageItem* bgItem = menuDrawable->getMenu()->getBackground()->getPageItem("background_active");
            if (bgItem)
            {
               SDL_Log("  background page item's active-layer texture id=%u", bgItem->getActiveLayer()->getTexture());
            }
            MenuPageItem* loginItem = menuDrawable->getMenu()->getCurrentPage()->getPageItem("login_window");
            if (loginItem)
            {
               SDL_Log("  login_window page item's active-layer texture id=%u", loginItem->getActiveLayer()->getTexture());
            }
         }
         else
         {
            SDL_Log("layer %s: not found", dump_layer.c_str());
         }
         delete menuCursor;
         delete menuDrawable;
         return 0;
      }

      static ActionLogger actionLogger;
      QObject::connect(
         menuDrawable->getMenu(), SIGNAL(actionRequest(QString, QString)), &actionLogger, SLOT(onActionRequest(QString, QString))
      );

      // the host-address dropdown normally lists previously-used server IPs, loaded from saved
      // settings (a networking concern out of scope for the menu-rendering phase) - a couple of
      // placeholder entries here are just so the dropdown has real rows to render when opened,
      // rather than testing against a permanently-empty list.
      MenuPageItem* hostTableItem = menuDrawable->getMenu()->getCurrentPage()->getPageItem("editablecombobox_host_table");
      if (auto* hostTable = dynamic_cast<MenuPageEditableComboBoxItem*>(hostTableItem))
      {
         hostTable->appendItem("127.0.0.1");
         hostTable->appendItem("192.168.1.1");
         hostTable->appendItem("10.0.0.5");
      }
   }
   else
   {
      const int loaded = scene.load("level.hjb", &factory, nullptr);
      SDL_Log("scene.load(\"level.hjb\") -> %d, materials=%d", loaded, scene.getMaterialCount());
   }

   int clickX = -1;
   int clickY = -1;
   if (!click_arg.empty())
   {
      const size_t comma = click_arg.find(',');
      if (comma != std::string::npos)
      {
         clickX = std::atoi(click_arg.substr(0, comma).c_str());
         clickY = std::atoi(click_arg.substr(comma + 1).c_str());
      }
   }

   // --realclick=x,y (WINDOW-space pixels, unlike --click which is page-space and calls
   // menuDrawable's handlers directly) - pushes genuine SDL_Event structs via SDL_PushEvent so
   // they flow through the exact same SDL_PollEvent loop and convertFromViewPort() conversion a
   // real OS mouse click would, instead of bypassing that path like --click does. Diagnostic tool
   // for the "buttons don't react to real mouse clicks" bug - see project memory (seventeenth
   // session) - isolates whether the real-event code path itself is broken, independent of
   // whether this environment can generate genuine OS input events at all.
   const std::string realclick_arg = argValue(args, "--realclick=");
   int realClickX = -1;
   int realClickY = -1;
   if (!realclick_arg.empty())
   {
      const size_t comma = realclick_arg.find(',');
      if (comma != std::string::npos)
      {
         realClickX = std::atoi(realclick_arg.substr(0, comma).c_str());
         realClickY = std::atoi(realclick_arg.substr(comma + 1).c_str());
      }
   }

   bool running = true;
   int frame = 0;

   // --selftest runs frames back-to-back as fast as the host can render, not paced by a real
   // display's vsync - SDL_GL_SetSwapInterval(1) doesn't throttle a headless/offscreen context
   // the way it does a real window. MenuDrawable's fade-in (mAlpha, driven by wall-clock dt via
   // animate()) would then take an enormous number of real frames to reach 1.0 and unblock
   // input, since dt stays near zero. Feed it a fixed synthetic 60fps timestep instead so the
   // selftest's timing-dependent behavior (fade-in completing, cursor click able to reach the
   // menu) is deterministic regardless of how fast this host actually renders.
   float menuTimeMs = 0.0f;

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

         if (menuMode && menuDrawable)
         {
            // menu items live in 1920x1080 page space (see mainmenu.psd/background.psd), not
            // window space - the original (client/src/game/bombermanview.cpp) converted every
            // mouse event through activeDevice->convertFromViewPort() before handing it to a
            // Drawable, and this port needs the exact same conversion now that main.cpp is doing
            // bombermanview's old job of dispatching events to each Drawable directly.
            switch (event.type)
            {
               case SDL_EVENT_MOUSE_MOTION:
               {
                  int x = static_cast<int>(event.motion.x);
                  int y = static_cast<int>(event.motion.y);
                  device.convertFromViewPort(&x, &y, 1920, 1080);
                  menuDrawable->mouseMoveEvent(x, y);
                  if (menuCursor)
                     menuCursor->mouseMoveEvent(x, y);
                  break;
               }
               case SDL_EVENT_MOUSE_BUTTON_DOWN:
               {
                  int x = static_cast<int>(event.button.x);
                  int y = static_cast<int>(event.button.y);
                  device.convertFromViewPort(&x, &y, 1920, 1080);
                  menuDrawable->mousePressEvent(x, y);
                  if (menuCursor)
                     menuCursor->mousePressEvent(x, y);
                  break;
               }
               case SDL_EVENT_MOUSE_BUTTON_UP:
                  menuDrawable->mouseReleaseEvent(nullptr);
                  if (menuCursor)
                     menuCursor->mouseReleaseEvent(nullptr);
                  break;
               default:
                  break;
            }
         }
      }

      if (selftest && !menuMode && !logo3dMode)
      {
         injector.advance();
      }

      // scripted click for --menu --selftest --click=x,y verification runs: move there first
      // (so hover/focus state updates the same way a real mouse would), then press+release a
      // couple frames later so the item's activated()/mousePressed() path is exercised for real
      // rather than skipped.
      if (menuMode && menuDrawable && clickX >= 0)
      {
         if (frame == 15)
         {
            menuDrawable->mouseMoveEvent(clickX, clickY);
            if (menuCursor)
               menuCursor->mouseMoveEvent(clickX, clickY);
         }
         else if (frame == 20)
         {
            menuDrawable->mousePressEvent(clickX, clickY);
            if (menuCursor)
               menuCursor->mousePressEvent(clickX, clickY);
         }
         else if (frame == 22)
         {
            menuDrawable->mouseReleaseEvent(nullptr);
            if (menuCursor)
               menuCursor->mouseReleaseEvent(nullptr);
         }
      }

      // --realclick=x,y: same frame schedule as --click above, but injects genuine SDL_Event
      // structs (window-space coordinates) via SDL_PushEvent instead of calling menuDrawable's
      // handlers directly - exercises the real SDL_PollEvent switch-case above, including
      // convertFromViewPort(), exactly like a real OS mouse click would.
      if (menuMode && realClickX >= 0)
      {
         const SDL_WindowID windowId = SDL_GetWindowID(context.window());

         if (frame == 15)
         {
            SDL_Event motionEvent{};
            motionEvent.type = SDL_EVENT_MOUSE_MOTION;
            motionEvent.motion.windowID = windowId;
            motionEvent.motion.x = static_cast<float>(realClickX);
            motionEvent.motion.y = static_cast<float>(realClickY);
            SDL_PushEvent(&motionEvent);
         }
         else if (frame == 20)
         {
            SDL_Event downEvent{};
            downEvent.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
            downEvent.button.windowID = windowId;
            downEvent.button.button = SDL_BUTTON_LEFT;
            downEvent.button.down = true;
            downEvent.button.clicks = 1;
            downEvent.button.x = static_cast<float>(realClickX);
            downEvent.button.y = static_cast<float>(realClickY);
            SDL_PushEvent(&downEvent);
         }
         else if (frame == 22)
         {
            SDL_Event upEvent{};
            upEvent.type = SDL_EVENT_MOUSE_BUTTON_UP;
            upEvent.button.windowID = windowId;
            upEvent.button.button = SDL_BUTTON_LEFT;
            upEvent.button.down = false;
            upEvent.button.clicks = 1;
            upEvent.button.x = static_cast<float>(realClickX);
            upEvent.button.y = static_cast<float>(realClickY);
            SDL_PushEvent(&upEvent);
         }
      }

      device.clear();

      if (logo3dMode && logo3d)
      {
         globalTime.update();
         logo3d->paintGL();
      }
      else if (menuMode && menuDrawable)
      {
         if (selftest)
            menuTimeMs += 16.6667f;
         else
            menuTimeMs = static_cast<float>(SDL_GetTicks());

         menuDrawable->animate(menuTimeMs);
         menuDrawable->paintGL();

         if (menuCursor && menuCursor->isVisible())
         {
            menuCursor->animate(menuTimeMs);
            menuCursor->paintGL();
         }

         if (logoDrawable && logoDrawable->isVisible())
         {
            // GameLogoDrawable's own fade/spark timing (FADE_IN_LENGTH etc.) is calibrated
            // against the original engine's Drawable::animate() convention (real seconds * 62.5,
            // see client/src/game/bombermanview.cpp) - menuTimeMs is real/synthetic
            // milliseconds, so convert (ms/1000)*62.5 == ms*0.0625 to keep the fade durations
            // meaning what they say. The earth/bomb sphere's own rotation reads GlobalTime
            // directly (see SphereFragmentsDrawable/SphereGeometryVbo) - update it here too so
            // the logo actually animates during real (non-selftest) play, not just once at t=0.
            globalTime.update();
            logoDrawable->animate(menuTimeMs * 0.0625f);
            logoDrawable->paintGL();
         }
      }
      else
      {
         globalTime.update();
         scene.render();
      }

      context.swap();

      ++frame;

      const bool selftestDone = (menuMode || logo3dMode) ? (selftest && frame > 40) : (selftest && injector.finished() && frame > 40);

      if (selftestDone)
      {
         if (!screenshot_path.empty())
         {
            saveScreenshot(screenshot_path, context.width(), context.height());
         }

         running = false;
      }
   }

   delete menuCursor;
   delete logoDrawable;
   delete menuDrawable;
   delete logo3d;

   return 0;
}

#include "main.moc"
