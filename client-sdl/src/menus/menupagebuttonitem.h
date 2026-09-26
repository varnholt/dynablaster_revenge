#pragma once

#include "menupageitem.h"

#include "framework/frametimer.h"

class MenuPageButtonItem : public MenuPageItem
{
public:
   MenuPageButtonItem();

   virtual void draw();

   virtual void setFocus(bool focus);

   virtual void setEnabled(bool enabled);

private:
   FrameTimer mFocusOutTime;

   float mFadeValue;
   bool mFadeOut;
};
