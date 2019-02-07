#ifndef MENUPAGELISTITEMELEMENT_H
#define MENUPAGELISTITEMELEMENT_H

// base
#include "menupagetextedit.h"

// framework
#include "tools/array.h"
#include "vertex.h"
#include "framework/frametimer.h"

class MenuPageListItemElement : public MenuPageTextEditItem
{
   public:

      MenuPageListItemElement();

      virtual ~MenuPageListItemElement();

      // main

      virtual void initialize();

      void draw(float x, float y, float opacity = 1.0f);

      void setIndex(int index);

      void setHeight(int height);

      void setWidth(int width);

      int getHeight() const;

      int getWidth() const;

      void setX(float x);

      void setY(float y);

      float getX() const;

      float getY() const;

      Array<Vertex> getBoundingRectVertices(float x, float y);


      bool isFadingOut();

      void stopFadeOut();

      float getFadeOutValue();

      void setOverrideAlpha(bool);

      bool isOverrideAlphaActive() const;

      const FrameTimer& getFocusOutTime() const;


   public slots:

      virtual void setFocus(bool);

      virtual void setActive(bool);


   private:

      int mIndex;

      int mWidth;

      int mHeight;

      float mX;

      float mY;


      // fading

      FrameTimer mFocusOutTime;

      bool mFadeOut;

      float mFadeValue;


      // color specific

      bool mOverrideAlpha;
};

#endif // MENUPAGELISTITEMELEMENT_H
