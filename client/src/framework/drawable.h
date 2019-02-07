#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <qnamespace.h>

class RenderDevice;

// qt forwards
class QMouseEvent;
class QKeyEvent;
class QWheelEvent;

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
      virtual void mousePressEvent(
         int x,
         int y,
         Qt::MouseButton = Qt::LeftButton
      );

      virtual void mouseMoveEvent(int x, int y);
      virtual void mouseReleaseEvent(QMouseEvent* event);
      virtual void wheelEvent(QWheelEvent *);

      //! keyboard events
      virtual void keyPressEvent(QKeyEvent* event);
      virtual void keyReleaseEvent(QKeyEvent* event);

      virtual void setVisible(bool visible);
      virtual bool isVisible() const;

   protected:
      RenderDevice*           mDevice;       //!< render device
      bool                    mVisible;      //!< visibility flag
};

#endif // DRAWABLE_H
