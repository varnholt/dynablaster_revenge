#pragma once

// engine
#include "framework/drawable.h"

// shared
#include "signal.h"

// menus
#include "image/psd.h"

#include <memory>

class Menu;
class MenuPageFadeAnimation;
class FrameBuffer;

/// \brief GLES3 port of client/src/menus/menudrawable.cpp.
///
/// ShaderPool is not ported (see project memory) - the shared texalphaignore shader is loaded
/// through GLDevice::loadShader/setShader instead, same as every other menu item.
/// MainDrawable::getInstance()->getRenderBuffer(1) doesn't exist in this port (no MainDrawable -
/// main.cpp drives Drawables directly), so this owns its own FrameBuffer for the page
/// cross-fade, sized to the current page and (re)created lazily.
/// initGlParameters()'s fixed-function glMatrixMode/glLoadIdentity/glColor4f become
/// GLDevice::setProjectionMatrix() (2D ortho, same pattern as every screen-space menu draw) -
/// there is no separate "reset modelview to identity" step because GLDevice::push()/pop() already
/// replace the world transform outright per draw call (see gldevice.h), unlike the legacy
/// glPushMatrix()/glPopMatrix() stack this used to sit on top of.
/// drawFrameBuffer(unsigned int) is dropped - declared and defined in the original, never called
/// anywhere (dead code even upstream).
class MenuDrawable : public Drawable
{
public:
   MenuDrawable(RenderDevice*);

   ~MenuDrawable();

   Menu* getMenu();

   // gl

   void initializeGL();

   void paintGL();

   virtual void setVisible(bool visible);

   // event handler

   //!
   void mousePressEvent(int x, int y);

   //!
   void mouseMoveEvent(int x, int y);

   //!
   void mouseReleaseEvent();

   //!
   void keyPressEvent(const KeyEvent& event);

   //! overwrite animate
   virtual void animate(float globalTime);

   //! initialization finished
   void initializationFinished();

   //! page was changed
   Signal<const std::string&> pageChangedSignal;

   //! page change active
   Signal<bool> pageChangeActiveSignal;

   //! visible or not
   Signal<bool> visibleSignal;

   //! page change finished
   Signal<> pageChangeAnimationStoppedSignal;

   //! signal key pressed event
   Signal<const KeyEvent&> keyPressedSignal;

   //! setter for active page by name
   void pageChangeRequest(const std::string&);

protected:
   //! page change has been finished
   void pageChangeAnimationStopped();

   //! fade in has finished
   void fadeInFinished();

   //! fade out has finished
   void fadeOutFinished();

   void initGlParameters();

   void cleanupGlParameters();

   void setInputBlocked(bool);

   bool isInputBlocked() const;

   void drawMenuContents();

   //! animate fade out of frame buffer
   void animateFadeFrameBuffer(float dt);

   //! fade out menu framebuffer
   void startFadeOutFrameBuffer();

   //! fade in menu framebuffer
   void startFadeInFrameBuffer();

   // menu

   std::unique_ptr<Menu> mMenu;

   //
   std::unique_ptr<MenuPageFadeAnimation> mFadeInAnimation;

   //
   std::unique_ptr<MenuPageFadeAnimation> mFadeOutAnimation;

   //!
   bool mInputBlocked;

   //! mouse x position
   int mMouseX;

   //! mouse y position
   int mMouseY;

   //! time
   float mTime;

   //! fade out framebuffer flag
   bool mFadeOut;

   //! fade in framebuffer flag
   bool mFadeIn;

   //! alpha
   float mAlpha;

   //! reset time on setVisible(true)
   bool mResetTime;

   //! alpha shader
   unsigned int mShader;
   int mAlphaParameter;

   //! page cross-fade render target - see class comment for why this port owns it directly
   //! instead of pulling it from MainDrawable.
   std::unique_ptr<FrameBuffer> mFrameBuffer;
};
