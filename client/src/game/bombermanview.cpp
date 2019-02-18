// header
#include "bombermanview.h"

// menus
#include "countdowndrawable.h"
#include "fontmap.h"
#include "fontpool.h"
#include "gamehelpdrawable.h"
#include "gamedrawable.h"
#include "gamelogodrawable.h"
#include "gamemessagingdrawable.h"
#include "gamesettings.h"
#include "gamestatsdrawable.h"
#include "gamewindrawable.h"
#include "hotkeydrawable.h"
#include "keyboardmapper.h"
#include "loadingdrawable.h"
#include "menumousecursor.h"
#include "menudrawable.h"
#include "musicplayerdrawable.h"
#include "soundsystem/sounddeviceinterface.h"
#include "render/texturepool.h"
#include "roundsdrawable.h"

// game
#include "videooptions.h"

// engine
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "framework/timerhandler.h"
#include "tools/filestream.h"
#include "tools/profiling.h"
#include "image/bmp.h"
#include "image/imagepool.h"

// post production
#include "postproduction/brightnessfilter.h"

// Qt
#include <QIcon>
#include <QMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopWidget>

// cmath
#include <math.h>

#define CAMERA_ZOOM_SPEED 0.15f

//-----------------------------------------------------------------------------
/*!
   \param parent parent widget
   \format gl format
*/
GameView::GameView(QWidget *parent, const QGLFormat& format)
 : MainDrawable(parent, format),
   mGameWinDrawable(0),
   mHotkeyDrawable(0),
   mCountdownDrawable(0),
   mRoundsDrawable(0),
   mGameDrawable(0),
   mGameMessagingDrawable(0),
   mGameStatsDrawable(0),
   mMenuDrawable(0),
   mLogoDrawable(0),
   mMusicPlayerDrawable(0),
   mGameHelpDrawable(0),
   mLoadingDrawable(0),
   mMenuMouseCursor(0),
   mDevice(0),
   mFrameBuffer(0),
   mRenderBuffer(0),
   mTempBuffer1(0),
   mTempBuffer2(0),
   mTempBuffer3(0),
   mBrightness(0),
   mDevicePixelRatio(1),
   mFrames(0),
   mFrameTime(0.0f),
   mDelta(0.0f),
   mIdle(false),
   mVideoExport(false),
   mVideoTimeDelta(0.0),
   mFrameNumber(0),
   mShowFps(0),
   mError(false),
   mMousePressKeyboardModifiers(Qt::ShiftModifier), // leave this so gamepads get a chance
   mSwapInterval(0),
   mZoomIn(false),
   mZoomOut(false),
   mZoom(1.0f)
{
   // enable mouse tracking
   setMouseTracking(true);
   setFocusPolicy(Qt::StrongFocus);

   FileStream::addPath( "data" );
   FileStream::addPath( "data/game" );
   FileStream::addPath( "data/shaders" );

   setWindowIcon(QIcon("data/icons/dynablaster_revenge-256.ico"));

   deserializeShowFps();

   QDesktopWidget *device = QApplication::desktop();

   // qDebug("%d %d %d %d", device->logicalDpiX(), device->logicalDpiY(), device->physicalDpiX(), device->physicalDpiY());
   // TODO: find and use reliable criteria   
   if (device->physicalDpiX() > 200)
   {
      mDevicePixelRatio = 2;
   }
#ifdef Q_OS_MAC
   // TODO: hardcoded to support Retina displays
   mDevicePixelRatio = 2;
#endif

   GameSettings::VideoSettings* video=
      GameSettings::getInstance()->getVideoSettings();
   mZoom= video->getZoom();
}


//-----------------------------------------------------------------------------
/*!
*/
GameView::~GameView()
{
   foreach (Drawable *drawable, mDrawables)
      delete drawable;

   delete mBrightness;
   delete mDevice;

   delete ImagePool::Instance();

   // delete buffers
   delete mFrameBuffer;
   delete mRenderBuffer;
   delete mTempBuffer1;
   delete mTempBuffer2;
   delete mTempBuffer3;
}


bool GameView::error() const
{
   return mError;
}

//-----------------------------------------------------------------------------
/*!
*/
void GameView::setVideoExport(float fps)
{
   mVideoTimeDelta= 1.0f / fps;
   SoundDeviceInterface::Instance()->startCapture();
   mVideoExport= true;

}

void GameView::setSwapInterval(int interval)
{
   mSwapInterval= interval;
   if (mDevice)
      mDevice->setSwapInterval(mSwapInterval);
}

//-----------------------------------------------------------------------------
/*!
*/
void GameView::initializeGL()
{
   mDevice = new GLDevice();
   if (!mDevice->init())
   {
      QMessageBox::critical(
               0,
               "Error",
               "Required OpenGL extensions are not available:\n"+
               GLDevice::getErrorText()
      );
      mError= true;
      return;
   }


   mDevice->setSwapInterval( mSwapInterval );

   BitmapFont* fontDefault =
      new BitmapFont(
         "fonts/font",
         MenuFont::sMenuChars,
         2.1f, 3.0f, 32.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.05f, -0.025f
      );

   BitmapFont* fontOutlined =
      new BitmapFont(
         "fonts/font",
         MenuFont::sMenuChars,
         2.0f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.35f, 0.03f, -0.025f
      );

   BitmapFont* fontLounge =
      new BitmapFont(
         "fonts/font",
         MenuFont::sMenuChars,
         2.2f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.1f, 0.05f, -0.025f
      );

   BitmapFont* fontTime =
      new BitmapFont(
         "fonts/font",
         MenuFont::sMenuChars,
         2.1f, 3.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.15f, 0.03f, -0.025f
      );

   BitmapFont* fontLarge =
      new BitmapFont(
         "fonts/font",
         MenuFont::sMenuChars,
         2.0f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.04f, 0.0025f, -0.01f
      );

   BitmapFont* fontLargeOutlined =
      new BitmapFont(
         "fonts/font",
         MenuFont::sMenuChars,
         2.0f, 4.0f, 32.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.15f, 0.03f, -0.025f
      );

   // font was generated with the following parameters:
   // 1024x1024, fontsize: 60, radius: 32, supersampling: 4, spacing: 4, padding: 2
   // chars: ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,:;'"!@#$%^&*()-_+=[]{}<>?/\|~
   FontPool::Instance()->add( "default",        fontDefault);
   FontPool::Instance()->add( "outlined",       fontOutlined);
   FontPool::Instance()->add( "lounge",         fontLounge);
   FontPool::Instance()->add( "time",           fontTime);
   FontPool::Instance()->add( "large",          fontLarge);
   FontPool::Instance()->add( "large-outlined", fontLargeOutlined);

    //   FontPool::Instance()->add(
   //       "default",
   //       new BitmapFont(
   //          "fonts/font_small",
   //          MenuFont::sSmallMenuChars, 12.0f, 4.0f, 10.0f)
   //       );

   mBrightness= new BrightnessFilter();
   mBrightness->init();

   // init drawables
   mGameDrawable = new GameDrawable(mDevice);
   mGameMessagingDrawable = new GameMessagingDrawable(mDevice);
   mGameStatsDrawable = new GameStatsDrawable(mDevice);


   installEventFilter(mGameDrawable);

   mMenuDrawable = new MenuDrawable(mDevice);
   mLogoDrawable = new GameLogoDrawable(mDevice);
   mMusicPlayerDrawable = new MusicPlayerDrawable(mDevice);
   mGameHelpDrawable = new GameHelpDrawable(mDevice);
   mLoadingDrawable = new LoadingDrawable(mDevice);
   mRoundsDrawable = new RoundsDrawable(mDevice);
   mCountdownDrawable = new CountdownDrawable(mDevice);
   mGameWinDrawable = new GameWinDrawable(mDevice);
   mHotkeyDrawable = new HotkeyDrawable(mDevice);
   mMenuMouseCursor = new MenuMouseCursor(mDevice);

   connect(
      mGameDrawable,
      SIGNAL(requestInit(Drawable*)),
      this,
      SLOT(initDrawable(Drawable*)),
      Qt::QueuedConnection
   );

   foreach(Drawable* drawable, mDrawables)
      drawable->initializeGL();

   mCurrentTime.start();

   // init video options
   VideoOptions* options = new VideoOptions();
   options->initialize();

   // connect drawables
   initDrawableConnections();

   emit glContextInitialized();

   // start idle action after 1 minute
   mIdleTimer.setInterval(60000);
   connect(
      &mIdleTimer,
      SIGNAL(timeout()),
      this,
      SLOT(idleTimerTimeout())
   );
   mIdleTimer.start();
}


//-----------------------------------------------------------------------------
/*!
   \param id buffer id
   \return ptr to frame buffer
*/
FrameBuffer* GameView::getRenderBuffer(int id) const
{
   FrameBuffer* buffer= 0;
   switch (id)
   {
      case 0: buffer= mRenderBuffer; break;
      case 1: buffer= mTempBuffer1; break;
      case 2: buffer= mTempBuffer2; break;
      case 3: buffer= mTempBuffer3; break;
      default: buffer= 0; break;
   }
   return buffer;
}


//-----------------------------------------------------------------------------
/*!
   \return hotkey drawable
*/
HotkeyDrawable *GameView::getHotkeyDrawable() const
{
   return mHotkeyDrawable;
}


//-----------------------------------------------------------------------------
/*!
   \return rounds drawable
*/
RoundsDrawable *GameView::getRoundsDrawable() const
{
   return mRoundsDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::animate()
{
   float time;
   float prevTime= getTime();
   if ( mVideoTimeDelta > 0.0f )
   {
      time= getTime() + mVideoTimeDelta;
   }
   else
   {
      time= mCurrentTime.elapsed() / 1000.0;
   }
   mDelta= time - prevTime;
   setTime( time );

   float t= time * 62.5;

   TimerHandler::Instance()->update();

   foreach (Drawable *drawable, mDrawables)
      if (drawable->isVisible())
         drawable->animate(t);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::zoomIn(bool state)
{
   mZoomIn= state;
   if (mZoomIn && mZoomOut)
      mZoomOut= false;
}

void GameView::zoomOut(bool state)
{
   mZoomOut= state;
   if (mZoomOut && mZoomIn)
      mZoomIn= false;
}


//-----------------------------------------------------------------------------
/*!
   \param e key event
*/
void GameView::keyPressEvent(QKeyEvent* e)
{
   // "isShown==false" indicates the hotkey drawable is already
   // fading out; in case it is already fading out pass the keyboard events
   // to the other drawables
   if (
         mHotkeyDrawable->isVisible()
      && mHotkeyDrawable->isShown()
   )
   {
      if (e->key() != Qt::Key_F1)
      {
         mHotkeyDrawable->setVisible(false);
      }

      restartIdleTimer(true);
   }
   else
   {
      KeyboardMapper* km = KeyboardMapper::getInstance();

      bool allowed =
            (km->isKeyAllowed((Qt::Key)e->key()) || km->isModifiedKeyAllowed(e->text()))
         && !km->isModifiedKeyForbidden(e->text());

      if (allowed)
      {
         foreach (Drawable *drawable, mDrawables)
            if (drawable->isVisible())
               drawable->keyPressEvent(e);
      }

      restartIdleTimer(true);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param e key event
*/
void GameView::keyReleaseEvent(QKeyEvent* e)
{
   KeyboardMapper* km = KeyboardMapper::getInstance();

   bool allowed =
         (km->isKeyAllowed((Qt::Key)e->key()) || km->isModifiedKeyAllowed(e->text()))
      && !km->isModifiedKeyForbidden(e->text());

   if (allowed)
   {
      foreach (Drawable *drawable, mDrawables)
         if (drawable->isVisible())
            drawable->keyReleaseEvent(e);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::initDrawableConnections()
{
   connect(
      mMenuDrawable,
      SIGNAL(pageChangeActive(bool)),
      mMenuMouseCursor,
      SLOT(setBusy(bool))
   );

   connect(
      mMenuDrawable,
      SIGNAL(pageChanged(QString)),
      mLogoDrawable,
      SLOT(pageChanged(QString))
   );
}


//-----------------------------------------------------------------------------
/*!
   \param fb ptr to framebuffer
   \param width frame buffer width
   \param height frame buffer height
   \return \c true if successful
*/
bool GameView::resizeFrameBuffer(
   FrameBuffer*& fb,
   int width,
   int height,
   int samples,
   int formatFlags
)
{
   if (samples>1 && (formatFlags & 1))
   {
      qWarning("Depth texture is only available for non multisampled framebuffers!");
   }

   if (fb && fb->resolutionChanged(width, height, samples))
   {
      delete fb;
      fb= 0;
   }

   if (!fb)
   {
      fb= new FrameBuffer(width, height, samples, formatFlags);
      return true;
   }

   return false;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::updateFrameBufferSettings()
{
   GameSettings::VideoSettings* videoSettings =
      GameSettings::getInstance()->getVideoSettings();

   int samples= videoSettings->getAntialias();
   int res= videoSettings->getResolution();
   int width= activeDevice->getWidth();
   int height= activeDevice->getHeight();
   int w= (width + res-1 ) / res;
   int h= (height + res - 1) / res;

   // fix to match 16:9
   if ( (w*9>>4) >= h )
      w= (h<<4)/9;
   else
      h= (w*9)>>4;

   bool changed= resizeFrameBuffer(mRenderBuffer, w, h, samples);
   resizeFrameBuffer(mFrameBuffer, w, h);
   resizeFrameBuffer(mTempBuffer1, w, h, 0, FrameBuffer::DepthTexture);
   resizeFrameBuffer(mTempBuffer2, w, h);
   resizeFrameBuffer(mTempBuffer3, w/2, h/2);

   if (changed)
      videoSettings->setAntialias( mRenderBuffer->samples() );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::deserializeFullscreen()
{
   bool fullscreenRequested =
      GameSettings::getInstance()->getVideoSettings()->isFullscreen();

   if (fullscreenRequested &&! isFullScreen())
      showFullScreen();
   else if (!fullscreenRequested && isFullScreen())
      showNormal();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::paintGL()
{
   if (mError)
   {
      mDevice->clear();
      qApp->quit();
      return;
   }

   GameSettings::VideoSettings* video=
      GameSettings::getInstance()->getVideoSettings();

   // handle camera zoom
   if (mZoomIn)
   {
      qDebug("zoom in");
      mZoom+= mDelta*CAMERA_ZOOM_SPEED;
      if (mZoom > 1.3f) mZoom= 1.3f;
      video->setZoom(mZoom);
   }

   if (mZoomOut)
   {
      qDebug("zoom out");
      mZoom-= mDelta*CAMERA_ZOOM_SPEED;
      if (mZoom < 0.7f) mZoom= 0.7f;
      video->setZoom(mZoom);
   }

   // set border
   mDevice->setBorder(
      video->getBorderLeft(),
      video->getBorderTop(),
      video->getBorderRight(),
      video->getBorderBottom() );

   if (mGameDrawable)
      mGameDrawable->setCameraZoom( mZoom );

   animate();

   updateFrameBufferSettings();
   
   mFrameBuffer->bind();

   while (!mDrawableInitQueue.isEmpty())
   {
      Drawable* drawable= mDrawableInitQueue.takeLast();
      drawable->paintGL();
   }

   // clear screen
   mDevice->clear();

   // double t1= getCpuTick();
   // int count= 0;
   foreach (Drawable *drawable, mDrawables)
   {
      if (drawable->isVisible())
      {
         drawable->paintGL();
/*
         glFinish();
         double t2= getCpuTick();
         printf("%d: %.10f \n", count++, (t2-t1)/1000000.0);
         t1= t2;
*/
      }
   }

   mFrameBuffer->unbind();

   // clear screen
   mDevice->clear();

   float level= video->getBrightness();
   int left= activeDevice->getBorderLeft();
   int bottom= activeDevice->getBorderBottom();
   int width= activeDevice->getWidth();
   int height= activeDevice->getHeight();

   int w= (height<<4) / 9;
   if (width > w)
   {
      // pillar box
      left += (width-w) >> 1;
      activeDevice->setViewPort(left,bottom,w,height);
      width= w;
   }
   else
   {
      // letter box
      int h= (width * 9) >> 4;
      bottom += (height - h) >> 1;
      activeDevice->setViewPort(left,bottom,width,h);
      height= h;
   }

   int scaleX= (int)floor((float)width / mFrameBuffer->width() + 0.5);
   int scaleY= (int)floor((float)height / mFrameBuffer->height() + 0.5);

   float u,v;
   if (width >= mFrameBuffer->width() && height >= mFrameBuffer->height())
   {
      u= (float) width / (mFrameBuffer->width() * scaleX);
      v= (float) height / (mFrameBuffer->height() * scaleY);
   }
   else
   {
      u= 1.0f;
      v= 1.0f;
   }

   mBrightness->setGamma( 2.2f - (level - 0.5f));
   mBrightness->process( mFrameBuffer->texture(), u, v );
   
   // figure fps
   if (mShowFps)
   {
      mFrames++;
      float curTime= getTime();
      if (curTime - mFrameTime >= 1.0f)
      {
         float fps= mFrames / (curTime - mFrameTime);
         mFrames= 0;
         mFrameTime= curTime;

         emit updateFps( fps );
      }
   }

   if (mVideoExport && (mFrameNumber & 1))
   {
      unsigned int* temp= new unsigned int[width * height];
      glReadPixels(0,0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, temp);
      char filename[256];
      sprintf(filename, "d:\\encode\\cap%5d.bmp", (mFrameNumber>>1)+10000);
      if (mFrameNumber < 20000)
      saveBmp(filename, temp, width, height);
      delete[] temp;
   }

   mFrameNumber++;
}


//-----------------------------------------------------------------------------
/*!
   \param width frame buffer width
   \param height frame buffer height
*/
void GameView::resizeGL(int width, int height)
{
   mDevice->resize(width, height);

   foreach(Drawable* drawable, mDrawables)
      drawable->resizeGL();

   GameSettings::VideoSettings* settings =
      GameSettings::getInstance()->getVideoSettings();

   settings->setWidth( width );
   settings->setHeight( height );
   settings->serialize();
}


//-----------------------------------------------------------------------------
/*!
   \param event mouse event
*/
void GameView::mousePressEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      if (!mHotkeyDrawable->isVisible())
      {
         // store mouse press keyboard modifiers
         setMousePressKeyboardModifiers(event->modifiers());

         foreach(Drawable* drawable, mDrawables)
         {
            if (drawable->isVisible())
            {
               int x = event->x() * mDevicePixelRatio;
               int y = event->y() * mDevicePixelRatio;

               activeDevice->convertFromViewPort(&x, &y, 1920, 1080);

               drawable->mousePressEvent(
                  x,
                  y,
                  event->button()
               );
            }
         }

         restartIdleTimer(true);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::idleTimerTimeout()
{
   setIdle(true);
   emit idle(true);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::stopIdleTimer()
{
   if (mIdleTimer.isActive())
      mIdleTimer.stop();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::startIdleTimer()
{
   if (!mHotkeyDrawable->isVisible())
      mIdleTimer.start();
}


//-----------------------------------------------------------------------------
/*!
   \param clickedOrKeyPressed clicked or key pressed flag
*/
void GameView::restartIdleTimer(bool clickedOrKeyPressed)
{
   stopIdleTimer();
   startIdleTimer();

   if (isIdle())
   {
      // only leave idle state when the user pressed a key or clicked
      // a mouse button
      if (clickedOrKeyPressed)
      {
         if (GameStateMachine::getInstance()->getState() != Constants::GamePreparing)
         {
            setIdle(false);
            emit idle(false);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \param idle idle flag
*/
void GameView::setIdle(bool idle)
{
   mIdle = idle;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if idle
*/
bool GameView::isIdle() const
{
   return mIdle;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::initDrawable(Drawable *drawable)
{
   mDrawableInitQueue.append( drawable );
}


//-----------------------------------------------------------------------------
/*!
   \param event mouse event
*/
void GameView::mouseMoveEvent(QMouseEvent* event)
{
   restartIdleTimer(false);

   foreach(Drawable* drawable, mDrawables)
   {
      if (drawable->isVisible())
      {
         int x = event->x() * mDevicePixelRatio;
         int y = event->y() * mDevicePixelRatio;

         activeDevice->convertFromViewPort(&x, &y, 1920, 1080);

         drawable->mouseMoveEvent(x, y);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param event mouse event
*/
void GameView::mouseReleaseEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton)
   {
      if (mHotkeyDrawable->isVisible())
      {
         mHotkeyDrawable->setVisible(false);
         restartIdleTimer(true);
      }
      else
      {
         foreach(Drawable* drawable, mDrawables)
            if (drawable->isVisible())
               drawable->mouseReleaseEvent(event);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param event wheel event
*/
void GameView::wheelEvent(QWheelEvent * event)
{
   foreach(Drawable* drawable, mDrawables)
      if (drawable->isVisible())
         drawable->wheelEvent(event);
}


//-----------------------------------------------------------------------------
/*!
*/
CountdownDrawable* GameView::getCountdownDrawable() const
{
   return mCountdownDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
MenuDrawable* GameView::getMenuDrawable() const
{
   return mMenuDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
GameDrawable* GameView::getGameDrawable() const
{
   return mGameDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
GameStatsDrawable* GameView::getGameStatsDrawable() const
{
   return mGameStatsDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
GameMessagingDrawable* GameView::getGameMessagingDrawable() const
{
    return mGameMessagingDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
GameHelpDrawable *GameView::getGameHelpDrawable() const
{
   return mGameHelpDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
LoadingDrawable *GameView::getLoadingDrawable() const
{
   return mLoadingDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
MusicPlayerDrawable* GameView::getMusicPlayerDrawable() const
{
   return mMusicPlayerDrawable;
}


//-----------------------------------------------------------------------------
/*!
*/
MenuMouseCursor* GameView::getMenuMouseCursor() const
{
   return mMenuMouseCursor;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::showMenuDisableGame()
{
   // disable game drawables
   mGameDrawable->setVisible(false);
   mGameMessagingDrawable->setVisible(false);
   mGameStatsDrawable->setVisible(false);
   mGameWinDrawable->setVisible(false);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::showMenuShowEnableMenu()
{
   // enable menu and music player drawables
   mMusicPlayerDrawable->setVisible( !mVideoExport );
   mMusicPlayerDrawable->setInGame(false);
   mGameHelpDrawable->setVisible(true);
   mMenuDrawable->setVisible(true);
   mLogoDrawable->setVisible(true);
   mMenuMouseCursor->setVisible(true);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::hideGameAfterMenuShownStep1()
{
   // show menu first (fade in)
   showMenuShowEnableMenu();

   // hide the game drawable when the menu has full visibility
   connect(
      mMenuDrawable,
      SIGNAL(visible(bool)),
      this,
      SLOT(hideGameAfterMenuShownStep2(bool))
   );
}


//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void GameView::hideGameAfterMenuShownStep2(bool visible)
{
   if (visible)
   {
      // disable game after menu has full visibility
      showMenuDisableGame();
   }

   disconnect(
      mMenuDrawable,
      SIGNAL(visible(bool)),
      this,
      SLOT(hideGameAfterMenuShownStep2(bool))
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::showMenu()
{
   showMenuDisableGame();
   showMenuShowEnableMenu();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::showGame()
{
   // disable menu drawables
   mMenuDrawable->setVisible(false);
   mLogoDrawable->setVisible(false);
   mMenuMouseCursor->setVisible(false);
   mGameHelpDrawable->setVisible(false);
   mGameWinDrawable->setVisible(false);

   // enable game and music player drawables
   mMusicPlayerDrawable->setVisible( !mVideoExport );
   mMusicPlayerDrawable->setInGame(true);
   mGameStatsDrawable->setVisible(true);
   mGameMessagingDrawable->setVisible(true);
   mGameDrawable->setVisible(true);

   if (mVideoTimeDelta > 0.0f)
   {
      SoundDeviceInterface::Instance()->startCapture();
      mVideoExport= true;
   }

}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::showMenuWithDelay()
{
   int delay =
         SHOW_WINNER_DISPLAY_TIME
       + SHOW_WINNER_FADE_IN_TIME
       + SHOW_WINNER_FADE_OUT_TIME
       + SHOW_WINNER_ADDITIONAL_TIME;

   TimerHandler::singleShot(
      delay,
      this,
      SLOT(hideGameAfterMenuShownStep1())
   );

   TimerHandler::singleShot(
      delay,
      mGameDrawable,
      SLOT(clear())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::exportObj()
{
   mGameDrawable->setExportScene(true);
}


//-----------------------------------------------------------------------------
/*!
   \return last mouse press keyboard modifiers
*/
Qt::KeyboardModifiers GameView::getMousePressKeyboardModifiers() const
{
   return mMousePressKeyboardModifiers;
}


//-----------------------------------------------------------------------------
/*!
   \param modifiers keyboard modifiers
*/
void GameView::setMousePressKeyboardModifiers(Qt::KeyboardModifiers modifiers)
{
   mMousePressKeyboardModifiers = modifiers;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameView::deserializeShowFps()
{
   mShowFps = GameSettings::getInstance()->getVideoSettings()->isFpsShown();
}

