#include "menudrawable.h"

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// menu
#include "defaultshader.h"
#include "menu.h"
#include "menupageitem.h"

// engine
#include "framework/framebuffer.h"
#include "framework/gldevice.h"

// animations
#include "menupagefadeanimation.h"

// math
#include "math.h"
#include "math/matrix.h"

//-----------------------------------------------------------------------------
/*!
   \param device render device
*/
MenuDrawable::MenuDrawable(RenderDevice* device)
    : Drawable(device),
      mInputBlocked(false),
      mMouseX(0),
      mMouseY(0),
      mTime(0.0f),
      mFadeOut(false),
      mFadeIn(false),
      mAlpha(0.0f),
      mResetTime(false),
      mShader(0),
      mAlphaParameter(-1)
{
   mMenu = std::make_unique<Menu>();
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::initializeGL()
{
   mMenu->initialize();

   mFadeInAnimation = std::make_unique<MenuPageFadeAnimation>();
   mFadeInAnimation->setFadeIn(true);
   mFadeInAnimation->initialize();

   // fade out
   mFadeOutAnimation = std::make_unique<MenuPageFadeAnimation>();
   mFadeOutAnimation->setFadeIn(false);
   mFadeOutAnimation->initialize();

   mShader = getDefaultMenuShader();
   mAlphaParameter = getDefaultMenuShaderAlphaParam();
}

//-----------------------------------------------------------------------------
/*!
 */
MenuDrawable::~MenuDrawable() = default;

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::initGlParameters()
{
   if (mMenu->getCurrentPage())
   {
      Matrix ortho = Matrix::ortho(0.0f, mMenu->getCurrentPage()->getWidth(), mMenu->getCurrentPage()->getHeight(), 0.0f, -1.0f, 1.0f);

      static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(ortho);
   }

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   // enable blending - the func must be set explicitly too, not just left enabled: other
   // drawables (SphereFragmentsDrawable's multi-pass compositing, GameLogoDrawable's additive
   // spark pass) legitimately change glBlendFunc mid-frame for their own needs, and this is the
   // first real draw call of a new frame - relying on whatever the previous frame's last
   // drawable happened to leave it at is exactly the kind of global-state coupling that caused
   // the main-menu background to render with a cyan tint instead of its real dark, low-contrast
   // pattern (2026-09-21: confirmed by explicitly resetting the func right before the
   // background's own draw call, which fixed it; moved the fix up to here since every page item
   // needs this, not just the background).
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::drawMenuContents()
{
   initGlParameters();

   float pageAlpha = 0.0f;
   MenuPage* page = 0;
   MenuPage* background = mMenu->getBackground();
   MenuPageAnimation* animation = 0;
   for (int i = 0; i < mMenu->size(); i++)
   {
      page = mMenu->at(i);

      if (page->isActive())
      {
         animation = page->getAnimation();

         if (animation)
            animation->animate();

         // initGlParameters() only sets this once per frame - not enough once 2 pages can be
         // active at once (a real cross-fade). Re-establish per page too.
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         // same reasoning as the blend state above, but for the projection matrix: the blit pass
         // at the end of THIS SAME loop below sets an identity projection for the full-screen FBO
         // quad and never restores it. On a real cross-fade (2 pages active), the second page's
         // pass would then render its background/items with that stale identity projection still
         // active - collapsing page-space coordinates (0..pageWidth/Height) directly into clip
         // space, so only the sliver within [-1,1] survives clipping instead of the whole page.
         // This is the root cause of the "fade only covers a small rect" bug - re-establish the
         // real page-space ortho projection every iteration, not just once before the loop.
         Matrix pageOrtho = Matrix::ortho(0.0f, page->getWidth(), page->getHeight(), 0.0f, -1.0f, 1.0f);
         static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(pageOrtho);

         // page cross-fade render target - see class comment for why this port owns it
         // directly (sized to the current page) instead of pulling one from MainDrawable.
         if (!mFrameBuffer)
         {
            mFrameBuffer = std::make_unique<FrameBuffer>(page->getWidth(), page->getHeight());
         }
         else
         {
            mFrameBuffer->setResolution(page->getWidth(), page->getHeight());
         }

         FrameBuffer::push();

         mFrameBuffer->bind();
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         activeDevice->setShader(mShader);

         background->render();
         page->render();

         mFrameBuffer->unbind();
         FrameBuffer::pop();

         pageAlpha = animation ? ((MenuPageFadeAnimation*)animation)->getAlpha() : 1.0f;

         // same as above - the draws just above may have changed blend state again.
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         // draw the composited page to the screen through the texalphaignore shader (not the
         // default per-item mShader) - the FBO's own alpha channel here is accumulated blend
         // residue from everything just drawn into it, not meaningful per-pixel coverage, so it
         // must be replaced rather than multiplied - see getFramebufferBlitShader()'s doc comment
         // (found 2026-09-20 comparing against a genuine reference build: using the per-item
         // multiplying shader here made the entire composited page look uniformly translucent).
         // With identity world + identity projection (see FrameBuffer::draw()'s doc comment).
         // Order matters here: GLDevice::push() uploads the combined MVP uniform immediately,
         // into whatever shader is bound and using whatever projection is set *at that moment*
         // - so the shader and projection must be set up first, or push() bakes in stale state.
         activeDevice->setShader(getFramebufferBlitShader());
         static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(Matrix());
         activeDevice->push(Matrix());
         activeDevice->setParameter(getFramebufferBlitShaderAlphaParam(), mAlpha);

         mFrameBuffer->draw(pageAlpha);

         activeDevice->pop();
         activeDevice->setShader(0);
      }
   }

   cleanupGlParameters();
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::paintGL()
{
   drawMenuContents();
}

//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void MenuDrawable::setVisible(bool visible)
{
   if (isVisible() != visible)
   {
      if (visible)
      {
         mResetTime = true;
         Drawable::setVisible(visible);

         setInputBlocked(true);
         startFadeInFrameBuffer();
      }
      else
      {
         setInputBlocked(true);
         startFadeOutFrameBuffer();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void MenuDrawable::animateFadeFrameBuffer(float dt)
{
   float val = dt * 0.01f;

   if (mFadeOut)
   {
      mAlpha -= val;

      if (mAlpha <= 0.0f)
      {
         mAlpha = 0.0f;
         mFadeOut = false;

         Drawable::setVisible(false);

         setInputBlocked(false);
         visibleSignal(false);
      }
   }

   else if (mFadeIn)
   {
      mAlpha += val;

      if (mAlpha >= 1.0f)
      {
         mAlpha = 1.0f;
         mFadeIn = false;

         setInputBlocked(false);
         visibleSignal(true);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \param time time
*/
void MenuDrawable::animate(float time)
{
   if (time != mTime)
   {
      float dt = 0.0f;

      // avoid large dt values on setVisble(true)
      if (mResetTime)
      {
         mResetTime = false;
      }
      else
      {
         dt = time - mTime;
      }

      mTime = time;

      MenuPage* page = 0;

      for (int i = 0; i < mMenu->size(); i++)
      {
         page = mMenu->at(i);

         if (page->isActive())
         {
            for (MenuPageItem* item : *page->getPageItems())
            {
               item->animate(time);
            }
         }
      }

      animateFadeFrameBuffer(dt);
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::initializationFinished()
{
   // called externally after everything is set up
   if (mMenu->getCurrentPage())
      pageChangedSignal(mMenu->getCurrentPage()->getFilename().toStdString());
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::startFadeOutFrameBuffer()
{
   mAlpha = 1.0f;
   mFadeOut = true;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::startFadeInFrameBuffer()
{
   mAlpha = 0.0f;
   mFadeIn = true;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::fadeInFinished()
{
   Drawable::setVisible(true);
   visibleSignal(true);
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::fadeOutFinished()
{
   Drawable::setVisible(false);
   visibleSignal(false);
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

//-----------------------------------------------------------------------------
/*!
   \param blocked input blocked flag
*/
void MenuDrawable::setInputBlocked(bool blocked)
{
   mInputBlocked = blocked;
}

//-----------------------------------------------------------------------------
/*!
  \return \c true if blocked
*/
bool MenuDrawable::isInputBlocked() const
{
   return mInputBlocked;
}

//-----------------------------------------------------------------------------
/*!
   \param x x pos
   \param y y pos
   \param button mouse button
*/
void MenuDrawable::mousePressEvent(
   int x,
   int y,
   Qt::MouseButton /*button*/
)
{
   if (!isInputBlocked())
      mMenu->mousePressed(x, y);
}

//-----------------------------------------------------------------------------
/*!
   \param x x pos
   \param y y pos
*/
void MenuDrawable::mouseMoveEvent(int x, int y)
{
   mMouseX = x;
   mMouseY = y;

   mMenu->mouseMoved(x, y);
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::mouseReleaseEvent(QMouseEvent* /*event*/)
{
   if (!isInputBlocked())
      mMenu->mouseReleased();
}

//-----------------------------------------------------------------------------
/*!
   \param event key event that was received
*/
void MenuDrawable::keyPressEvent(QKeyEvent* event)
{
   mMenu->keyPressed(event->key(), event->text().toStdString());

   keyPressedSignal(event);
}

//-----------------------------------------------------------------------------
/*!
   \param name requested page name
*/
void MenuDrawable::pageChangeRequest(const std::string& name)
{
   setInputBlocked(true);

   // get page pointers
   MenuPage* previous = mMenu->getCurrentPage();
   MenuPage* current = mMenu->getPageByName(QString::fromStdString(name));
   mMenu->setCurrentPage(current);

   // set and connect animations
   previous->setAnimation(mFadeOutAnimation.get());
   current->setAnimation(mFadeInAnimation.get());

   // cleanup previous connections
   mFadeInAnimation->stoppedSignal.disconnectAll();
   mFadeOutAnimation->stoppedSignal.disconnectAll();

   // disable previous page when animation has finished
   mFadeOutAnimation->stoppedSignal.connect([previous]() { previous->deactivate(); });

   mFadeOutAnimation->stoppedSignal.connect([previous]() { previous->resetAnimation(); });

   mFadeInAnimation->stoppedSignal.connect([current]() { current->resetAnimation(); });

   mFadeInAnimation->stoppedSignal.connect([this]() { pageChangeAnimationStopped(); });

   // activate the current page
   current->setActive(true);

   // start animations
   mFadeInAnimation->start();
   mFadeOutAnimation->start();

   previous->unFocusAllItems();

   // signal page change
   pageChangedSignal(name);
   pageChangeActiveSignal(true);
}

//-----------------------------------------------------------------------------
/*!
   \return ptr to menu
*/
Menu* MenuDrawable::getMenu()
{
   return mMenu.get();
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuDrawable::pageChangeAnimationStopped()
{
   // allow access to the page
   setInputBlocked(false);
   pageChangeActiveSignal(false);

   // re-trigger mouse move event
   mouseMoveEvent(mMouseX, mMouseY);

   pageChangeAnimationStoppedSignal();
}
