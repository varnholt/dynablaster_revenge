#pragma once

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


   private:

      FrameTimer mElapsed;

      float mX;
      float mY;

      FrameTimer mFlipBackgroundElapsed;
      BackgroundColor mBackgroundColor;
      BackgroundColor mBackgroundColorPrevious;
      PSDLayer* mBackgroundLayers[BackgroundColorBlue + 1];

      // the scrolling quad this draws directly (not via PSDLayer, since its texcoords animate
      // every frame) needs its own dynamically re-uploaded buffer - see draw().
      unsigned int mVertexBuffer;
};
