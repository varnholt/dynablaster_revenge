#include "drawable.h"
#include "maindrawable.h"
#include <QMouseEvent>
#include <QKeyEvent>


Drawable::Drawable(RenderDevice* dev, bool visible)
   : mDevice(dev),
     mVisible(visible)
{
   MainDrawable::getInstance()->addDrawable(this);
}


Drawable::~Drawable()
{
   MainDrawable::getInstance()->removeDrawable(this);
}


void Drawable::animate(float /*time*/)
{
}


void Drawable::resizeGL()
{
}


void Drawable::mousePressEvent(
   int /*x*/,
   int /*y*/,
   Qt::MouseButton /*button*/
)
{
}


void Drawable::mouseMoveEvent(int /*x*/, int /*y*/)
{
}


void Drawable::mouseReleaseEvent(QMouseEvent* /*event*/)
{
}


void Drawable::wheelEvent(QWheelEvent* /*e*/)
{

}


void Drawable::keyPressEvent(QKeyEvent* /*event*/)
{

}


void Drawable::keyReleaseEvent(QKeyEvent* /*event*/)
{

}


void Drawable::setVisible(bool visible)
{
   mVisible = visible;
}


bool Drawable::isVisible() const
{
   return mVisible;
}


