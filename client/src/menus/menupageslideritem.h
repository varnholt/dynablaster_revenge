#ifndef MENUPAGESLIDERITEM_H
#define MENUPAGESLIDERITEM_H

// base
#include "menupageitem.h"

class MenuPageSliderItem : public MenuPageItem
{
   Q_OBJECT

   public:

      //! constructor
      MenuPageSliderItem();

      virtual bool isGrabbingMouseEvents();

      virtual void mousePressed(int x, int y);

      virtual void mouseMoved(int x, int y);

      virtual void mouseReleased();


      void setMinimum(int min);
      void setMaximum(int max);

      int getMinimum() const;
      int getMaximum() const;

      float getValue() const;

      void setValue(float);


   signals:

      void valueChanged(float val);


   protected:


      int mMinimum;
      int mMaximum;

      float mValue;

      int mRelativeToX;
};

#endif // MENUPAGESLIDERITEM_H
