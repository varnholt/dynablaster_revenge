#ifndef MENUPAGEBACKGROUNDITEM_H
#define MENUPAGEBACKGROUNDITEM_H

// base
#include "menupageitem.h"

#include "framework/frametimer.h"

class MenuPageBackgroundItem : public MenuPageItem
{
   Q_OBJECT

   public:

      enum BackgroundColor
      {
         BackgroundColorRed = 0,
         BackgroundColorGreen,
         BackgroundColorBlue
      };

      //! constructor
      MenuPageBackgroundItem(QObject* parent = 0);     

      // main

      virtual void draw();

      virtual void initialize();

      void addGradientLayer(
         PSDLayer* gradient,
         BackgroundColor color
      );


   public slots:

      void setBackgroundColor(BackgroundColor);


   private slots:

      void unitTest1Slot();


   private:

      void unitTest1();

      FrameTimer mElapsed;

      float mX;
      float mY;

      FrameTimer mFlipBackgroundElapsed;
      BackgroundColor mBackgroundColor;
      BackgroundColor mBackgroundColorPrevious;
      PSDLayer* mBackgroundLayers[BackgroundColorBlue + 1];
};

#endif // MENUPAGEBACKGROUNDITEM_H
