#ifndef MENUPAGESCROLLBAR_H
#define MENUPAGESCROLLBAR_H

#include "menupageitem.h"


class MenuPageScrollbar : public MenuPageItem
{
   Q_OBJECT

   public:

       MenuPageScrollbar();

       virtual bool isGrabbingMouseEvents();

       virtual void mousePressed(int x, int y);

       virtual void mouseMoved(int x, int y);

       virtual void mouseReleased();

       void setHeight(int);

       void setTop(int);


   public slots:

       void updateFromAnimation(float);


   signals:

       void scrollToPercentage(float percent);


   protected:

       int mPosition;

       int mHeight;

       int mTop;

       float mOffset;

       int mRelativeToY;

};

#endif // MENUPAGESCROLLBAR_H
