#pragma once

#include "keyevent.h"

class RenderDevice;

/// \brief GLES3 port of client/src/framework/drawable.cpp.
///
/// The legacy constructor/destructor auto-registered with MainDrawable::getInstance() (a global
/// list MainDrawable's own paintGL()/animate() loop walked each frame). MainDrawable - the
/// QGLWidget hosting the real game - isn't part of this port; main.cpp drives the render loop
/// itself and calls each Drawable it owns (MenuDrawable, MenuMouseCursor, ...) directly, so that
/// registration is dropped rather than resurrecting MainDrawable just to keep a list nothing
/// reads.
class Drawable
{
public:
   Drawable(RenderDevice* dev, bool visible = false);
   virtual ~Drawable();

   virtual void initializeGL() = 0;
   virtual void paintGL() = 0;
   virtual void resizeGL();

   virtual void animate(float globalTime);

   //! mouse events
   virtual void mousePressEvent(int x, int y);

   virtual void mouseMoveEvent(int x, int y);
   virtual void mouseReleaseEvent();

   //! keyboard events
   virtual void keyPressEvent(const KeyEvent& event);
   virtual void keyReleaseEvent(const KeyEvent& event);

   virtual void setVisible(bool visible);
   virtual bool isVisible() const;

protected:
   RenderDevice* mDevice;  //!< render device
   bool mVisible;          //!< visibility flag
};
