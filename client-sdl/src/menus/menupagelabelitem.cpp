#include "menupagelabelitem.h"
#include "fontpool.h"

MenuPageLabelItem::MenuPageLabelItem()
    : mFont(0), mFontXOffset(0), mFontYOffset(0), mMaxChars(255), mScale(0.0f), mAlpha(255), mCenterWidth(-1), mCenterHeight(-1)
{
   mPageItemType = PageItemTypeLabel;
}

void MenuPageLabelItem::initialize()
{
   mFont = FontPool::Instance()->get(qPrintable(mFontName));
}

void MenuPageLabelItem::draw()
{
   MenuPageItem::draw();

   if (isVisible())
   {
      if (mColor.isValid())
      {
         mFont->setColor(mColor.redF(), mColor.greenF(), mColor.blueF(), mAlpha / 255.0f);
      }
      else
      {
         mFont->setColor(1.0f, 1.0f, 1.0f, mAlpha / 255.0f);
      }

      mFont->buildVertices(
         mScale,
         qPrintable(mText),
         mLayerActive->getLeft() + mFontXOffset,
         mLayerActive->getBottom() + mFontYOffset,
         mCenterWidth,
         mCenterHeight
      );

      mFont->draw();
   }
}

void MenuPageLabelItem::setFontXOffset(int xOffset)
{
   mFontXOffset = xOffset;
}

void MenuPageLabelItem::setFontYOffset(int yOffset)
{
   mFontYOffset = yOffset;
}

void MenuPageLabelItem::setMaxChars(int maxChars)
{
   mMaxChars = maxChars;
}

void MenuPageLabelItem::setColor(const QColor& color)
{
   mColor = color;
}

void MenuPageLabelItem::setAlpha(int alpha)
{
   mAlpha = alpha;
}

void MenuPageLabelItem::setCenterWidth(float width)
{
   mCenterWidth = width;
}

void MenuPageLabelItem::setCenterHeight(float height)
{
   mCenterHeight = height;
}

void MenuPageLabelItem::setScale(float scale)
{
   mScale = scale;
}

void MenuPageLabelItem::setText(const QString& text)
{
   mText = text;
}

QString MenuPageLabelItem::getText() const
{
   return mText;
}

void MenuPageLabelItem::setFontName(const QString& fontName)
{
   mFontName = fontName;
}
