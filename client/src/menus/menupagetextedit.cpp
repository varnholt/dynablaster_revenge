// header
#include "menupagetextedit.h"

// menus
#include "fontpool.h"

// Qt
#include <QChar>
#include <QGLContext>

#define CURSOR_UPDATE_TIME 500

MenuPageTextEditItem::MenuPageTextEditItem()
   : mFont(0),
     mFontXOffset(0),
     mFontYOffset(0),
     mFieldWidth(255),
     mMaxLength(-1),
     mScale(0.0f),
     mEditingActive(false),
     mCursorVisible(false),
     mAlpha(255),
     mCursorPosition(0)
{
   mPageItemType = PageItemTypeTextedit;
   mInteractive = true;
}


void MenuPageTextEditItem::initialize()
{
   mFont = FontPool::Instance()->get(qPrintable(mFontName));

   // init update timer
   mTimer.setInterval(CURSOR_UPDATE_TIME);

   connect(
      &mTimer,
      SIGNAL(timeout()),
      this,
      SLOT(updateCursorHighlight())
   );
}


void MenuPageTextEditItem::draw()
{
   if (isVisible())
   {
      if (mColor.isValid())
      {
         glColor4ub(mColor.red(), mColor.green(), mColor.blue(), mAlpha);
      }
      else
      {
         glColor4ub(255, 255, 255, mAlpha);
      }

      /*
               i0               i0+maxLength
                [               ]
         [ABCDEFGHIJKLMNOPQRSTUVWXYZ]

      */

      int i0 = mCursorPosition - getFieldWidth();
      i0 = qMax(i0, 0);
      QString visibleText = mText.mid(i0, getFieldWidth());

      mFont->buildVertices(
         mScale,
         qPrintable(visibleText),
         mLayerActive->getLeft() + mFontXOffset,
         mLayerActive->getBottom() + mFontYOffset
      );

      mFont->draw();

      if (mEditingActive)
      {
         drawCursor();
      }

      glColor4f(1,1,1,1);
   }
}


void MenuPageTextEditItem::keyPressed(int key, const QString& text)
{
   if (key == Qt::Key_Backspace)
   {
      if (isCursorAtEnd())
      {
         // chop from end
         mText.chop(1);
         moveCursorLeft();
      }
      else
      {
         if (getCursorPosition() > 0)
         {
            // replace chars
            mText = mText.replace(getCursorPosition() - 1, 1, "");
            moveCursorLeft();
         }
      }
   }
   else if (key == Qt::Key_Delete)
   {
      if (!isCursorAtEnd())
      {
         // replace chars
         mText = mText.replace(getCursorPosition(), 1, "");
      }
   }
   else if (key == Qt::Key_Left)
   {
      moveCursorLeft();
   }
   else if (key == Qt::Key_Right)
   {
      moveCursorRight();
   }
   else if (key == Qt::Key_Home)
   {
      moveCursorToStart();
   }
   else if (key == Qt::Key_End)
   {
      moveCursorToEnd();
   }
   else if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Escape)
   {
      // ignored
   }
   else if (!text.isEmpty())
   {
      if (isCursorAtEnd())
      {
         // append chars
         if (mText.length() < getMaxLength())
            mText.append(text);
      }
      else
      {
         // replace chars
         mText = mText.replace(getCursorPosition(), 1, text);
      }

      moveCursorRight();
   }
}


void MenuPageTextEditItem::setFontName(const QString &fontName)
{
   mFontName = fontName;
}


void MenuPageTextEditItem::setRegExp(const QRegExp& regexp)
{
   mRegexp = regexp;
}


void MenuPageTextEditItem::setFontXOffset(int xOffset)
{
   mFontXOffset = xOffset;
}


void MenuPageTextEditItem::setFontYOffset(int yOffset)
{
   mFontYOffset = yOffset;
}


void MenuPageTextEditItem::setFieldWidth(int fieldWidth)
{
   mFieldWidth = fieldWidth;
}


void MenuPageTextEditItem::setMaxLength(int maxLength)
{
   mMaxLength = maxLength;
}


int MenuPageTextEditItem::getMaxLength() const
{
   return mMaxLength;
}


int MenuPageTextEditItem::getFieldWidth() const
{
   return mFieldWidth;
}


void MenuPageTextEditItem::setScale(float scale)
{
   mScale = scale;
}

float MenuPageTextEditItem::getScale() const
{
   return mScale;
}


void MenuPageTextEditItem::setText(const QString& text)
{
   mText = text.left(qMax(text.length(), getFieldWidth()));
   setCursorPosition(mText.length());
}


void MenuPageTextEditItem::setColor(const QColor &color)
{
   mColor = color;
}


void MenuPageTextEditItem::setOutlineColor(const QColor &outlineColor)
{
   mOutlineColor = outlineColor;
}


const QColor &MenuPageTextEditItem::getColor() const
{
   return mColor;
}


void MenuPageTextEditItem::setAlpha(int alpha)
{
   mAlpha = alpha;
}


bool MenuPageTextEditItem::isActionRequestOnClickEnabled() const
{
   return false;
}


void MenuPageTextEditItem::setCursorPosition(int index)
{
   mCursorPosition = index;
}


int MenuPageTextEditItem::getCursorPosition() const
{
   return mCursorPosition;
}


bool MenuPageTextEditItem::isEditingActive() const
{
   return mEditingActive;
}


void MenuPageTextEditItem::moveCursorRight()
{
   setCursorPosition(
      qMin(
         getCursorPosition() + 1,
         getText().length()
      )
   );
}


void MenuPageTextEditItem::moveCursorLeft()
{
   setCursorPosition(
      qMax(
         getCursorPosition() - 1,
         0
      )
   );
}


void MenuPageTextEditItem::moveCursorToStart()
{
   setCursorPosition(0);
}


void MenuPageTextEditItem::moveCursorToEnd()
{
   setCursorPosition(getText().length());
}


const QString &MenuPageTextEditItem::getText() const
{
   return mText;
}


void MenuPageTextEditItem::activated()
{
   mTimer.start();

   // call the timer's slot once initially
   updateCursorHighlight();

   mEditingActive = true;
   MenuPageItem::activated();
}


void MenuPageTextEditItem::deactivated()
{
   mTimer.stop();
   mEditingActive = false;
   MenuPageItem::deactivated();
}


void MenuPageTextEditItem::paste(const QString &text)
{
   for (int i = 0; i < text.length(); i++)
      keyPressed(Qt::Key_unknown, text.at(i));
}


void MenuPageTextEditItem::drawCursor()
{
   float alphaFactor = qMax(1.0f - 0.75f*(mCursorTime.elapsed() / (float)CURSOR_UPDATE_TIME), 0.0f);

   float left = 0.0f;
   float right = 0.0f;
   float top = 0.0f;
   float bottom = 0.0f;

   mFont->getCursor(mScale, getCursorPosition(), left, right, top, bottom);

   glBindTexture(GL_TEXTURE_2D, 0);

   glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

   if (mColor.isValid())
   {
      glColor4ub(mColor.red(), mColor.green(), mColor.blue(), 128 * alphaFactor);
   }
   else
   {
      glColor4ub(255, 255, 255, 128 * alphaFactor);
   }

   glBegin(GL_QUADS);

   glVertex3f(
      left,
      top,
      0
   );

   glVertex3f(
      right,
      top,
      0
   );

   glVertex3f(
      right,
      bottom,
      0
   );

   glVertex3f(
      left,
      bottom,
      0
   );

   glEnd();

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void MenuPageTextEditItem::updateCursorHighlight()
{
   // mCursorVisible = !mCursorVisible;
   mCursorTime.restart();
}


bool MenuPageTextEditItem::isCursorAtEnd() const
{
   return (getCursorPosition() == getText().length());
}

