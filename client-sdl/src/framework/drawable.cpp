#include "drawable.h"

Drawable::Drawable(RenderDevice* dev, bool visible) : mDevice(dev), mVisible(visible)
{
}

Drawable::~Drawable()
{
}

void Drawable::animate(float /*time*/)
{
}

void Drawable::resizeGL()
{
}

void Drawable::mousePressEvent(int /*x*/, int /*y*/)
{
}

void Drawable::mouseMoveEvent(int /*x*/, int /*y*/)
{
}

void Drawable::mouseReleaseEvent()
{
}

void Drawable::keyPressEvent(const KeyEvent& /*event*/)
{
}

void Drawable::keyReleaseEvent(const KeyEvent& /*event*/)
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
