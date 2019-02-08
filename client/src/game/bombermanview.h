#ifndef BOMBERMANVIEW_H
#define BOMBERMANVIEW_H

// base
#include "maindrawable.h"

// Qt
#include <QTime>
#include <QTimer>

// forward declarations
class BrightnessFilter;
class CountdownDrawable;
class Drawable;
class FrameBuffer;
class GameDrawable;
class GameHelpDrawable;
class GameLogoDrawable;
class GameMessagingDrawable;
class GameStatsDrawable;
class GameWinDrawable;
class HotkeyDrawable;
class LoadingDrawable;
class MenuDrawable;
class MenuMouseCursor;
class MusicPlayerDrawable;
class RenderDevice;
class RoundsDrawable;

class GameView : public MainDrawable
{
Q_OBJECT

public:

   //! constructor
   GameView(QWidget *parent, const QGLFormat& format);

   //! destructor
   virtual ~GameView();

   bool error() const;

   void setVideoExport(float fps);

   FrameBuffer* getRenderBuffer(int id) const;

   HotkeyDrawable* getHotkeyDrawable() const;
   RoundsDrawable* getRoundsDrawable() const;
   CountdownDrawable* getCountdownDrawable() const;
   MenuDrawable* getMenuDrawable() const;
   GameDrawable* getGameDrawable() const;
   GameStatsDrawable* getGameStatsDrawable() const;
   GameMessagingDrawable* getGameMessagingDrawable() const;
   GameHelpDrawable* getGameHelpDrawable() const;
   LoadingDrawable* getLoadingDrawable() const;
   MusicPlayerDrawable* getMusicPlayerDrawable() const;
   MenuMouseCursor* getMenuMouseCursor() const;


signals:

   //! fps changed
   void updateFps(float fps);

   //! gl context is initialized
   void glContextInitialized();

   //! gui is idle
   void idle(bool idle);


public slots:

   //! show game
   void showGame();

   //! show menu immediately
   void showMenu();

   //! show menu with fixed delay
   void showMenuWithDelay();

   //! update fullscreen state
   void deserializeFullscreen();

   //! update show fps
   void deserializeShowFps();

   //! setter for idle state
   void setIdle(bool idle);

   //! queue drawbale for init
   void initDrawable(Drawable *drawable);

   //! stop the idle timer
   void stopIdleTimer();

   //! start the idle timer
   void startIdleTimer();

   //! restart idle timer
   void restartIdleTimer(bool clickedOrKeyPressed = true);

   //! export current scene as obj
   void exportObj();

   //! getter for last mouse press modifiers
   Qt::KeyboardModifiers getMousePressKeyboardModifiers() const;

   //! setter for last mouse press keyboard modifiers
   void setMousePressKeyboardModifiers(Qt::KeyboardModifiers modifiers);

   //! setter for swap interval (vsync), default is 0 (off)
   void setSwapInterval(int interval);

   //! user presses/released zoom key
   void zoomIn(bool state);
   void zoomOut(bool state);


protected slots:

   void animate();

   //! idle timer timed out
   void idleTimerTimeout();

   //! show menu part 1: disable game drawables
   void showMenuDisableGame();

   //! show menu part 2: enable menu drawables
   void showMenuShowEnableMenu();

   //! disable game after menu visible part 1
   void hideGameAfterMenuShownStep1();

   //! disable game after menu visible part 2
   void hideGameAfterMenuShownStep2(bool visible);


protected:

   // event handlers
   virtual void keyPressEvent(QKeyEvent*);
   virtual void keyReleaseEvent(QKeyEvent*);
   virtual void mousePressEvent(QMouseEvent* event);
   virtual void mouseMoveEvent(QMouseEvent* event);
   virtual void mouseReleaseEvent(QMouseEvent* event);
   virtual void wheelEvent(QWheelEvent *);


   // gl functionality

   //! inialize gl context
   void initializeGL();

   //! paint gl overwritten
   void paintGL();

   //! resize gl overwritten
   void resizeGL(int width, int height);

   //! update frame buffer related settings
   void updateFrameBufferSettings();


private:

   //! resize framebuffer if necessary
   bool resizeFrameBuffer(
      FrameBuffer*& fb,
      int width,
      int height,
      int samples = 0,
      int formatFlags = 0
   );

   //! initialize signals and slots
   void initDrawableConnections();

   //! getter for idle state
   bool isIdle() const;


   // drawables
   GameWinDrawable*         mGameWinDrawable;             //!< game win drawable
   HotkeyDrawable*          mHotkeyDrawable;              //!< hotkey drawable
   CountdownDrawable*       mCountdownDrawable;           //!< countdown drawable
   RoundsDrawable*          mRoundsDrawable;              //!< game rounds drawable
   GameDrawable*            mGameDrawable;                //!< game drawable
   GameMessagingDrawable*   mGameMessagingDrawable;       //!< game messaging
   GameStatsDrawable*       mGameStatsDrawable;           //!< game stats drawable
   MenuDrawable*            mMenuDrawable;                //!< menu drawable
   GameLogoDrawable*        mLogoDrawable;                //!< logo drawable
   MusicPlayerDrawable*     mMusicPlayerDrawable;         //!< music player drawable
   GameHelpDrawable*        mGameHelpDrawable;            //!< error handler drawable
   LoadingDrawable*         mLoadingDrawable;             //!< loading drawable
   MenuMouseCursor*         mMenuMouseCursor;             //!< mouse cursor

   // render stuff
   RenderDevice*            mDevice;                      //!< render device
   FrameBuffer*             mFrameBuffer;                 //!< frame buffer
   FrameBuffer*             mRenderBuffer;                //!< render buffer (multisampled)
   FrameBuffer*             mTempBuffer1;                 //!< temp buffer
   FrameBuffer*             mTempBuffer2;                 //!< temp buffer
   FrameBuffer*             mTempBuffer3;                 //!< temp buffer
   BrightnessFilter*        mBrightness;                  //!< brightness filter (blits framebuffer to screen)
   int                      mDevicePixelRatio;            //!< used to save pixel ratio to determine retina displays

   // fps
   int                      mFrames;                      //!< number of rendered frames
   float                    mFrameTime;                   //!< frame time

   QTime                    mCurrentTime;
   float                    mDelta;                       //! delta time

   // idle processing
   QTimer                   mIdleTimer;                   //!< idle timer
   bool                     mIdle;                        //!< ui is currently idle
   QList<Drawable*>         mDrawableInitQueue;
   bool                     mVideoExport;
   float                    mVideoTimeDelta;
   int                      mFrameNumber;
   bool                     mShowFps;                     //!< fps shown
   bool                     mError;
   Qt::KeyboardModifiers    mMousePressKeyboardModifiers; //!< last mouse press event's keyboard modifiers
   int                      mSwapInterval;
   bool                     mZoomIn, mZoomOut;
   float                    mZoom;
};

#endif
