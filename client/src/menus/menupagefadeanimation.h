#ifndef MENUPAGEFADEANIMATION_H
#define MENUPAGEFADEANIMATION_H

#include "menupageanimation.h"
#include "framework/frametimer.h"

class MenuPageFadeAnimation : public MenuPageAnimation
{
   Q_OBJECT

   public:

       MenuPageFadeAnimation();

       virtual ~MenuPageFadeAnimation();

       int getAlphaLocation();

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

       int mAlphaLocation;

       bool mStopped;

       bool mFadeIn;

       float mAlpha;

};

#endif // MENUPAGEFADEANIMATION_H
