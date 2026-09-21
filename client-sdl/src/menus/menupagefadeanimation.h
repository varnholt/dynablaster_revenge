#pragma once

#include "framework/frametimer.h"
#include "menupageanimation.h"

class MenuPageFadeAnimation : public MenuPageAnimation
{
   Q_OBJECT

public:
   MenuPageFadeAnimation();

   virtual ~MenuPageFadeAnimation();

   void setAlpha(float);

   void setFadeIn(bool);

   virtual void initialize();

   float getAlpha() const;

   bool isStopped() const;

   void setStopped(bool value);

public slots:

   virtual void start();

   virtual void animate();

private:
   FrameTimer mElapsed;

   bool mStopped;

   bool mFadeIn;

   float mAlpha;
};
