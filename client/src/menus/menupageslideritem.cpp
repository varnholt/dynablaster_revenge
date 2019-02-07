#include "menupageslideritem.h"

MenuPageSliderItem::MenuPageSliderItem()
   : mMinimum(0),
     mMaximum(0),
     mValue(0.0f),
     mRelativeToX(0)
{
   mPageItemType = PageItemTypeSlider;
   mInteractive = true;
}


void MenuPageSliderItem::mousePressed(int x, int y)
{
   mRelativeToX = x - mLayerActive->getLeft();

   setActive(true);
   MenuPageItem::mousePressed(x, y);
}



void MenuPageSliderItem::mouseReleased()
{
   setActive(false);
   MenuPageItem::mouseReleased();
}


void MenuPageSliderItem::mouseMoved(int x, int y)
{
   // qDebug("MenuPageListItem::mouseMoved: x: %d, y: %d", x, y);

   x -= mRelativeToX;

   float width = mMaximum - mMinimum;

   if (x > mMaximum - mLayerActive->getWidth())
      x = mMaximum - mLayerActive->getWidth();
   else if (x < mMinimum)
      x = mMinimum;

   mLayerActive->getLayer()->setX(x);

   // .....................|:::::::::::::::::(*)::::::::::::::::::::::::::::::|
   //                     min                 x                              max
   //                      <----------------------- width -------------------->

   mValue = (x - mMinimum) / width;
   emit valueChanged(mValue);

   MenuPageItem::mouseMoved(x, y);
}


bool MenuPageSliderItem::isGrabbingMouseEvents()
{
   return true;
}


void MenuPageSliderItem::setMinimum(int min)
{
   mMinimum = min;
}


void MenuPageSliderItem::setMaximum(int max)
{
   mMaximum = max;
}


int MenuPageSliderItem::getMinimum() const
{
   return mMinimum;
}


int MenuPageSliderItem::getMaximum() const
{
   return mMaximum;
}


float MenuPageSliderItem::getValue() const
{
   return mValue;
}


void MenuPageSliderItem::setValue(float value)
{
   // reset mouse press relative x
   mRelativeToX = 0;

   // store value
   mValue = value;

   // determine new x position
   float width = mMaximum - mMinimum;

   float x = mMinimum + width * mValue;

   // the slider knob takes a few pixels, too
   // => get rid of half of its width
   x -= mLayerActive->getLayer()->getWidth() * 0.5f;
   if (x < mMinimum)
      x = mMinimum;

   mLayerActive->getLayer()->setX(x);
}


