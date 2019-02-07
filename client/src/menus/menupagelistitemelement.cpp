#include "menupagelistitemelement.h"

// framework
#include "framework/gldevice.h"
#include "bitmapfont.h"

// math
#include "math.h"

MenuPageListItemElement::MenuPageListItemElement()
   : MenuPageTextEditItem(),
     mIndex(0),
     mWidth(0),
     mHeight(0),
     mX(0.0f),
     mY(0.0f),
     mFadeOut(false),
     mFadeValue(0.0f),
     mOverrideAlpha(false)
{
   mPageItemType = PageItemTypeListElement;
}


MenuPageListItemElement::~MenuPageListItemElement()
{
   delete mLayerActive;
   mLayerActive = 0;

   delete mLayerInactive;
   mLayerInactive = 0;
}   


void MenuPageListItemElement::initialize()
{
   // create dummy layers
   mLayerActive = new PSDLayer();
   mLayerInactive = new PSDLayer();

   MenuPageTextEditItem::initialize();
}


void MenuPageListItemElement::draw(float x, float y, float opacity)
{
   const QColor& rgb = getColor();
   glColor4f(
      rgb.red() / 255.0,
      rgb.green() / 255.0,
      rgb.blue() / 255.0,
      opacity
   );

   // draw item data
   mFont->buildVertices(
      mScale,
      qPrintable(mText),
      mX + x,
      mY + y + mHeight,
      -1,
      mHeight
   );

   float r = 0.0f;
   float g = 0.0f;
   float b = 0.0f;
   float a = 0.0f;
   if (mOutlineColor.isValid())
   {
      mFont->getOutlineColor(r, g, b, a);
      mFont->setOutlineColor(
         mOutlineColor.redF(),
         mOutlineColor.greenF(),
         mOutlineColor.blueF(),
         mOutlineColor.alphaF()
      );
   }

   mFont->draw();

   if (mOutlineColor.isValid())
      mFont->setOutlineColor(r, g, b, a);
}



void MenuPageListItemElement::setFocus(bool focus)
{
   // focus lost, then fade out
   if (mFocussed && !focus)
   {
      if (!mActive)
      {
         mFadeOut = true;
         mFocusOutTime.restart();
      }
   }

   // call base
   MenuPageTextEditItem::setFocus(focus);
}



void MenuPageListItemElement::setActive(bool active)
{
   // active flag lost, then fade out
   if (mActive && !active)
   {
      mActive = true;
      mFocusOutTime.restart();
   }

   // call base
   MenuPageTextEditItem::setActive(active);
}



void MenuPageListItemElement::setIndex(int index)
{
   mIndex = index;
}


void MenuPageListItemElement::setHeight(int height)
{
   mHeight = height;
}


void MenuPageListItemElement::setWidth(int width)
{
   mWidth = width;
}


int MenuPageListItemElement::getHeight() const
{
   return mHeight;
}


int MenuPageListItemElement::getWidth() const
{
   return mWidth;
}


void MenuPageListItemElement::setX(float x)
{
   mX = x;
}


void MenuPageListItemElement::setY(float y)
{
   mY = y;
}


float MenuPageListItemElement::getX() const
{
   return mX;
}


float MenuPageListItemElement::getY() const
{
   return mY;
}


Array<Vertex> MenuPageListItemElement::getBoundingRectVertices(
   float x,
   float y
)
{
   Array<Vertex> arr(4);

   arr.add(
      Vertex(
         x + mX,
         y + mY + mHeight,
         0.0f,
         1.0f
      )
   );

   arr.add(
      Vertex(
         x + mX + mWidth,
         y + mY + mHeight,
         1.0f,
         1.0f
      )
   );

   arr.add(
      Vertex(
         x + mX + mWidth,
         y + mY,
         1.0f,
         0.0f
      )
   );

   arr.add(
      Vertex(
         x + mX,
         y + mY,
         0.0f,
         0.0f
      )
   );

   return arr;
}


void MenuPageListItemElement::stopFadeOut()
{
   mFadeOut = false;
}


bool MenuPageListItemElement::isFadingOut()
{
   return mFadeOut;
}


float MenuPageListItemElement::getFadeOutValue()
{
   float val = 0.0f;

   if (getFocusOutTime().elapsed() < 500)
      val = cos(mFocusOutTime.elapsed() * 0.01);

   return val;
}


void MenuPageListItemElement::setOverrideAlpha(bool override)
{
   mOverrideAlpha = override;
}


bool MenuPageListItemElement::isOverrideAlphaActive() const
{
   return mOverrideAlpha;
}


const FrameTimer &MenuPageListItemElement::getFocusOutTime() const
{
   return mFocusOutTime;
}
