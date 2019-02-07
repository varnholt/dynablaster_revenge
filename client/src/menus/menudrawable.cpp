#include "menudrawable.h"

// Qt
#include <QGLContext>
#include <QGLShaderProgram>
#include <QMouseEvent>

// menu
#include "menu.h"
#include "menupageitem.h"
#include "shaderpool.h"

// engine
#include "framebuffer.h"
#include "gldevice.h"
#include "maindrawable.h"

// animations
#include "menupagefadeanimation.h"

// math
#include "math.h"


//-----------------------------------------------------------------------------
/*!
   \param device render device
*/
MenuDrawable::MenuDrawable(RenderDevice* device)
   : QObject(),
     Drawable(device),
     mMenu(0),
     mWidthRatio(0.0f),
     mHeightRatio(0.0f),
     mFadeInAnimation(0),
     mFadeOutAnimation(0),
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
   mMenu = new Menu();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuDrawable::initializeGL()
{
   mMenu->initialize();

   mFadeInAnimation = new MenuPageFadeAnimation();
   mFadeInAnimation->setFadeIn(true);
   mFadeInAnimation->initialize();

   // fade out
   mFadeOutAnimation = new MenuPageFadeAnimation();
   mFadeOutAnimation->setFadeIn(false);
   mFadeOutAnimation->initialize();

   mShader= activeDevice->loadShader(
      "texalphaignore-vert.glsl",
      "texalphaignore-frag.glsl"
   );

   mAlphaParameter = activeDevice->getParameterIndex("alpha");
}


//-----------------------------------------------------------------------------
/*!
*/
MenuDrawable::~MenuDrawable()
{
   delete mMenu;
   mMenu = 0;

   delete mFadeInAnimation;
   mFadeInAnimation = 0;

   delete mFadeOutAnimation;
   mFadeOutAnimation = 0;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuDrawable::initGlParameters()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   if (mMenu->getCurrentPage())
   {
      Matrix ortho= Matrix::ortho(
         0.0f,
         mMenu->getCurrentPage()->getWidth(),
         mMenu->getCurrentPage()->getHeight(),
         0.0f,
         -1.0f,
         1.0f
      );
      glLoadMatrixf(ortho);
  }

   // reset color
   glColor4f(1,1,1,1);

   // open modelview matrix and reset it
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   // enable blending
   glEnable(GL_BLEND);
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

         FrameBuffer::push();

         FrameBuffer* fb= MainDrawable::getInstance()->getRenderBuffer(1);
         fb->bind();
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         background->render();
         page->render();

         fb->unbind();
         FrameBuffer::pop();

         pageAlpha =
            animation
               ? ((MenuPageFadeAnimation*)animation)->getAlpha()
               : 1.0f;

         activeDevice->setShader(mShader);
         activeDevice->setParameter(mAlphaParameter, mAlpha);

         fb->draw(pageAlpha);

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
   float val = dt * 0.01;

   if (mFadeOut)
   {
      mAlpha -= val;

      if (mAlpha <= 0.0f)
      {
         mAlpha = 0.0f;
         mFadeOut = false;

         Drawable::setVisible(false);

         setInputBlocked(false);
         emit visible(false);
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
         emit visible(true);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param time time
*/
void MenuDrawable::animate(float time)
{
   if (time != mTime )
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
            foreach(MenuPageItem* item, *page->getPageItems())
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
      emit pageChanged(mMenu->getCurrentPage()->getFilename());
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
   emit visible(true);
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuDrawable::fadeOutFinished()
{
   Drawable::setVisible(false);
   emit visible(false);
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
void MenuDrawable::mouseReleaseEvent(QMouseEvent * /* event */)
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
   mMenu->keyPressed(event->key(), event->text());

   emit keyPressed(event);
}


//-----------------------------------------------------------------------------
/*!
   \param name requested page name
*/
void MenuDrawable::pageChangeRequest(const QString& name)
{
   setInputBlocked(true);

   // get page pointers
   MenuPage* previous = mMenu->getCurrentPage();
   MenuPage* current = mMenu->getPageByName(name);
   mMenu->setCurrentPage(current);

   // set and connect animations
   previous->setAnimation(mFadeOutAnimation);
   current->setAnimation(mFadeInAnimation);

   // cleanup previous connections
   mFadeInAnimation->disconnect();
   mFadeOutAnimation->disconnect();

   // disable previous page when animation has finished
   connect(
      mFadeOutAnimation,
      SIGNAL(stopped()),
      previous,
      SLOT(deactivate())
   );

   connect(
      mFadeOutAnimation,
      SIGNAL(stopped()),
      previous,
      SLOT(resetAnimation())
   );

   connect(
      mFadeInAnimation,
      SIGNAL(stopped()),
      current,
      SLOT(resetAnimation())
   );

   connect(
      mFadeInAnimation,
      SIGNAL(stopped()),
      this,
      SLOT(pageChangeAnimationStopped())
   );

   // activate the current page
   current->setActive(true);

   // start animations
   mFadeInAnimation->start();
   mFadeOutAnimation->start();

   previous->unFocusAllItems();

   // signal page change
   emit pageChanged(name);
   emit pageChangeActive(true);
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to menu
*/
Menu* MenuDrawable::getMenu()
{
   return mMenu;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuDrawable::pageChangeAnimationStopped()
{
   // allow access to the page
   setInputBlocked(false);
   emit pageChangeActive(false);

   // re-trigger mouse move event
   mouseMoveEvent(mMouseX, mMouseY);

   emit pageChangeAnimationStoppedSignal();
}


//-----------------------------------------------------------------------------
/*!
   \param texture texture to draw
*/
void MenuDrawable::drawFrameBuffer(unsigned int texture)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   glColor4f(1,1,1,1);

   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f( 1,-1,-1);
   glTexCoord2f(1,1); glVertex3f( 1, 1,-1);
   glTexCoord2f(0,1); glVertex3f(-1, 1,-1);
   glEnd();

   glColor4f(1,1,1,1);

   // restore projection
   glPopMatrix();

   // restore modelview
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
}

