#include "menupagescrollbar.h"


MenuPageScrollbar::MenuPageScrollbar()
   : MenuPageItem(),
     mPosition(0),
     mHeight(0),
     mTop(0),
     mOffset(0.0f),
     mRelativeToY(0)
{
   mPageItemType = PageItemTypeScrollbar;
   mInteractive = true;
}


void MenuPageScrollbar::mousePressed(int x, int y)
{
   mRelativeToY = y - mLayerActive->getTop();

   setActive(true);
   MenuPageItem::mousePressed(x, y);
}


void MenuPageScrollbar::mouseReleased()
{
   setActive(false);
   MenuPageItem::mouseReleased();
}


void MenuPageScrollbar::mouseMoved(int x, int y)
{
   y -= mRelativeToY;

   // max
   if (y + mLayerActive->getHeight() > mTop + mHeight)
      mPosition = mTop + mHeight - mLayerActive->getHeight();

   // min
   else if (y < mTop)
      mPosition = mTop;

   // in between
   else
      mPosition = y;

   mLayerActive->getLayer()->setY(mPosition);

   mOffset = (mPosition - mTop) / (float)(mHeight - mLayerActive->getHeight());

   emit scrollToPercentage(mOffset);

   MenuPageItem::mouseMoved(x, y);
}


bool MenuPageScrollbar::isGrabbingMouseEvents()
{
   return true;
}


void MenuPageScrollbar::setHeight(int height)
{
   mHeight = height;
}


void MenuPageScrollbar::setTop(int top)
{
   mTop = top;
}


void MenuPageScrollbar::updateFromAnimation(float percent)
{
   // reset mouse press relative y
   mRelativeToY = 0;

   blockSignals(true);
   mouseMoved(0, mTop + ( (mHeight-mLayerActive->getHeight()) * percent) );
   blockSignals(false);
}
