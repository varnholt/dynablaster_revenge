#pragma once

#include "menupageitem.h"

#include "framework/frametimer.h"

class MenuPageButtonItem : public MenuPageItem
{
   Q_OBJECT

   public:

      MenuPageButtonItem(QObject* parent = 0);

      virtual void draw();


   public slots:

      virtual void setFocus(bool focus);

      virtual void setEnabled(bool enabled);


   private:

      FrameTimer mFocusOutTime;

      float mFadeValue;
      bool mFadeOut;

};
