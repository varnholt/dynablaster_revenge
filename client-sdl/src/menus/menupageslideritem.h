#pragma once

// base
#include "menupageitem.h"
#include "signal.h"

class MenuPageSliderItem : public MenuPageItem
{
public:
   //! constructor
   MenuPageSliderItem();

   virtual bool isGrabbingMouseEvents();

   virtual void mousePressed(int x, int y);

   virtual void mouseMoved(int x, int y);

   virtual void mouseReleased();

   void setMinimum(int min);
   void setMaximum(int max);

   int getMinimum() const;
   int getMaximum() const;

   float getValue() const;

   void setValue(float);

   Signal<float> valueChangedSignal;

protected:
   int mMinimum;
   int mMaximum;

   float mValue;

   int mRelativeToX;
};
