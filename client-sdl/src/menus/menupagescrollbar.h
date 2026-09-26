#pragma once

#include "menupageitem.h"
#include "signal.h"

class MenuPageScrollbar : public MenuPageItem
{
public:
   MenuPageScrollbar();

   virtual bool isGrabbingMouseEvents();

   virtual void mousePressed(int x, int y);

   virtual void mouseMoved(int x, int y);

   virtual void mouseReleased();

   void setHeight(int);

   void setTop(int);

   void updateFromAnimation(float);

   Signal<float> scrollToPercentageSignal;

protected:
   int mPosition;

   int mHeight;

   int mTop;

   float mOffset;

   int mRelativeToY;

   //! suppresses scrollToPercentageSignal while updateFromAnimation() drives mouseMoved()
   //! programmatically, to avoid feeding the table's own scrollAnimation update straight back to it
   bool mSignalsBlocked;
};
