#include "menupagebuttonitem.h"

// Qt
#include <QGLContext>

// math
#include "math.h"


MenuPageButtonItem::MenuPageButtonItem(QObject* parent)
   : MenuPageItem(parent),
     mFadeValue(0.0f),
     mFadeOut(false)
{
   mPageItemType = PageItemTypeButton;
   mInteractive = true;
}


void MenuPageButtonItem::setFocus(bool focus)
{
   // only allow item focussing when the button is enabled
   if (isEnabled() || !focus)
   {
      if (mFocussed && !focus)
      {
         mFadeOut = true;
         mFocusOutTime.restart();
      }

      // call base
      MenuPageItem::setFocus(focus);
   }
}


void MenuPageButtonItem::setEnabled(bool enabled)
{
   MenuPageItem::setEnabled(enabled);

   // fade out that button if it's not enabled
   if (!enabled)
      setFocus(false);
}


void MenuPageButtonItem::draw()
{
   // call base
   MenuPageItem::draw();

   if (isVisible())
   {
      if (mFadeOut)
      {
         PSDLayer* psdLayer = getActiveLayer();

         mFadeValue = 0.0f;

         if (mFocusOutTime.elapsed() < 500)
            mFadeValue = cos(mFocusOutTime.elapsed() * 0.005);

         psdLayer->render(0, 0, mFadeValue);

         // either cos drops below 0.0 or focus out time exceeds 300ms
         if (mFadeValue < 0.1f)
         {
            mFadeOut = false;
         }
      }
   }
}

