#pragma once

// base
#include "framework/drawable.h"

// psd
#include "image/psd.h"

#include "framework/frametimer.h"

#include <string>

class PSDLayer;

class MenuMouseCursor : public Drawable
{
public:
   enum State
   {
      Default,
      Clicked,
      Busy
   };

   MenuMouseCursor(RenderDevice* dev, bool visible = false);

   virtual ~MenuMouseCursor();

   virtual void initializeGL();
   virtual void paintGL();
   virtual void animate(float globalTime);

   //! mouse events
   virtual void mousePressEvent(int x, int y);
   virtual void mouseMoveEvent(int x, int y);
   virtual void mouseReleaseEvent();

   void setBusy(bool);

protected:
   void initializeLayers();

   void initGlParameters();
   void cleanupGlParameters();

   void paintDefaultCursor();
   void paintClickedCursor();
   void paintCursor(PSDLayer* layer, float opacity = 1.0f);
   void paintBusyIcon();

   PSD mPsd;

   std::string mFilename;

   PSDLayer* mDefaultLayer;
   PSDLayer* mClickedLayer;
   PSDLayer* mBusyLayer;

   int mBusyX;
   int mBusyY;

   int mX;
   int mY;

   float mSizeFactor;

   bool mBusy;
   bool mMousePressed;

   FrameTimer mClickTime;

   float mTime;
};
