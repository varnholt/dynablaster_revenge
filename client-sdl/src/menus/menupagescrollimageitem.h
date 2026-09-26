#pragma once

// base
#include "menupageitem.h"

// forward declarations
class Clipper;
class PSDLayer;

class MenuPageScrollImageItem : public MenuPageItem
{
public:
   MenuPageScrollImageItem();

   virtual ~MenuPageScrollImageItem();

   virtual void initialize();

   virtual void draw();

   virtual void reset();

   virtual void animate(float time);

protected:
   Clipper* mClipper;

   PSDLayer* mLayer;

   float mY;
   float mStartTime;
   float mAnimationTime;
   bool mMoveUp;
   float mRelativeTimePrevious;
};
