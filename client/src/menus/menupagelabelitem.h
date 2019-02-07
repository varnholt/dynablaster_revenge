#ifndef MENUPAGELABEL_H
#define MENUPAGELABEL_H

// base
#include "menupageitem.h"

// Qt
#include <QColor>

// forward declarations
class BitmapFont;


class MenuPageLabelItem : public MenuPageItem
{
   Q_OBJECT

   public:

      MenuPageLabelItem();

       // main

       virtual void draw();

       virtual void initialize();

       QString getText() const;


   public slots:

       void setFontName(const QString& fontName);

       void setText(const QString&);

       void setFontXOffset(int xOffset);

       void setFontYOffset(int yOffset);

       void setScale(float scale);

       void setMaxChars(int maxChars);

       void setColor(const QColor& color);

       void setAlpha(int alpha);

       void setCenterWidth(float width);

       void setCenterHeight(float height);


   protected:

       QString mFontName;

       BitmapFont* mFont;

       QString mText;

       int mFontXOffset;

       int mFontYOffset;

       int mMaxChars;

       float mScale;

       QColor mColor;

       int mAlpha;

       float mCenterWidth;

       float mCenterHeight;
};

#endif // MENUPAGELABEL_H
