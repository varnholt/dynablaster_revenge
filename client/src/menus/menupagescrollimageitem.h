#ifndef MENUPAGESCROLLIMAGEITEM_H
#define MENUPAGESCROLLIMAGEITEM_H

// base
#include "menupageitem.h"

// tools
#include "tools/array.h"

// menus
#include "vertex.h"

// forward declarations
class Clipper;
class PSDLayer;

class MenuPageScrollImageItem : public MenuPageItem
{
   Q_OBJECT

   public:

      MenuPageScrollImageItem();

      virtual ~MenuPageScrollImageItem();

      virtual void initialize();

      virtual void draw();


   public slots:


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

#endif // MENUPAGESCROLLIMAGEITEM_H
