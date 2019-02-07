#ifndef MENUWIDGET_H
#define MENUWIDGET_H

// Qt
#include <QGLWidget>
#include <QImage>
#include <QList>

// engine
#include "drawable.h"

// menus
#include "image/psd.h"

class Menu;
class MenuPageFadeAnimation;
class ShaderPool;

class MenuDrawable : public QObject, public Drawable
{
    Q_OBJECT

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
      void mousePressEvent(
         int x,
         int y,
         Qt::MouseButton = Qt::LeftButton
      );

      //!
      void mouseMoveEvent(int x, int y);

      //!
      void mouseReleaseEvent(QMouseEvent* event);

      //!
      void keyPressEvent(QKeyEvent* event);

      //! overwrite animate
      virtual void animate(float globalTime);

      //! initialization finished
      void initializationFinished();


   signals:

      //! page was changed
      void pageChanged(const QString&);

      //! page change active
      void pageChangeActive(bool);

      //! visible or not
      void visible(bool);

      //! page change finished
      void pageChangeAnimationStoppedSignal();

      //! signal key pressed event
      void keyPressed(QKeyEvent*);


   protected slots:

      //! setter for active page by name
      void pageChangeRequest(const QString&);

      //! page change has been finished
      void pageChangeAnimationStopped();

      //! fade in has finished
      void fadeInFinished();

      //! fade out has finished
      void fadeOutFinished();


   protected:

      void initGlParameters();

      void cleanupGlParameters();

      void setInputBlocked(bool);

      bool isInputBlocked() const;

      void drawFrameBuffer(unsigned int texture);

      void drawMenuContents();

      //! animate fade out of frame buffer
      void animateFadeFrameBuffer(float dt);

      //! fade out menu framebuffer
      void startFadeOutFrameBuffer();

      //! fade in menu framebuffer
      void startFadeInFrameBuffer();


      // menu

      Menu* mMenu;

      //
      float mWidthRatio;

      //
      float mHeightRatio;

      //
      MenuPageFadeAnimation* mFadeInAnimation;

      //
      MenuPageFadeAnimation* mFadeOutAnimation;

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
};

#endif // MENUWIDGET_H
