// header
#include "menupagetextedit.h"

// menus
#include "fontpool.h"
#include "framework/gldevice.h"
#include "math/matrix.h"

// Qt
#include <QChar>

#include <cstring>

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
      mCursorPosition(0),
      mCursorTexture(0),
      mCursorVertexBuffer(0)
{
   mPageItemType = PageItemTypeTextedit;
   mInteractive = true;
}

void MenuPageTextEditItem::initialize()
{
   mFont = FontPool::Instance()->get(qPrintable(mFontName));

   // init update timer
   mTimer.setInterval(CURSOR_UPDATE_TIME);

   connect(&mTimer, SIGNAL(timeout()), this, SLOT(updateCursorHighlight()));
}

void MenuPageTextEditItem::draw()
{
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

      /*
               i0               i0+maxLength
                [               ]
         [ABCDEFGHIJKLMNOPQRSTUVWXYZ]

      */

      int i0 = mCursorPosition - getFieldWidth();
      i0 = qMax(i0, 0);
      QString visibleText = mText.mid(i0, getFieldWidth());

      mFont->buildVertices(
         mScale, qPrintable(visibleText), mLayerActive->getLeft() + mFontXOffset, mLayerActive->getBottom() + mFontYOffset
      );

      mFont->draw();

      if (mEditingActive)
      {
         drawCursor();
      }
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

void MenuPageTextEditItem::setFontName(const QString& fontName)
{
   mFontName = fontName;
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

void MenuPageTextEditItem::setColor(const Color& color)
{
   mColor = color;
}

void MenuPageTextEditItem::setOutlineColor(const Color& outlineColor)
{
   mOutlineColor = outlineColor;
}

const Color& MenuPageTextEditItem::getColor() const
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
   setCursorPosition(qMin(getCursorPosition() + 1, getText().length()));
}

void MenuPageTextEditItem::moveCursorLeft()
{
   setCursorPosition(qMax(getCursorPosition() - 1, 0));
}

void MenuPageTextEditItem::moveCursorToStart()
{
   setCursorPosition(0);
}

void MenuPageTextEditItem::moveCursorToEnd()
{
   setCursorPosition(getText().length());
}

const QString& MenuPageTextEditItem::getText() const
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

void MenuPageTextEditItem::paste(const QString& text)
{
   for (int i = 0; i < text.length(); i++)
      keyPressed(Qt::Key_unknown, text.at(i));
}

void MenuPageTextEditItem::drawCursor()
{
   // the legacy untextured glColor4ub'd quad (glBindTexture(GL_TEXTURE_2D, 0), a special
   // GL_SRC_ALPHA/GL_SRC_COLOR blend for an invert-highlight look) has no direct GLES3
   // equivalent - every draw needs a real bound texture and a real shader. Replaced with a
   // lazily-created 1x1 white texture drawn through the shared texalphaignore shader; the
   // mColor tint is dropped (see class comment), so this is always a white highlight now.
   float alphaFactor = qMax(1.0f - 0.75f * (mCursorTime.elapsed() / (float)CURSOR_UPDATE_TIME), 0.0f);

   float left = 0.0f;
   float right = 0.0f;
   float top = 0.0f;
   float bottom = 0.0f;

   mFont->getCursor(mScale, getCursorPosition(), left, right, top, bottom);

   if (mCursorTexture == 0)
   {
      unsigned int white = 0xFFFFFFFF;
      mCursorTexture = activeDevice->createTexture(&white, 1, 1, 0);
   }

   glBindTexture(GL_TEXTURE_2D, mCursorTexture);

   glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

   const float quad[] = {
      left, top, -1.0f, 0.0f, 0.0f, right, top,    -1.0f, 1.0f, 0.0f, right, bottom, -1.0f, 1.0f, 1.0f,
      left, top, -1.0f, 0.0f, 0.0f, right, bottom, -1.0f, 1.0f, 1.0f, left,  bottom, -1.0f, 0.0f, 1.0f,
   };

   if (mCursorVertexBuffer == 0)
      mCursorVertexBuffer = activeDevice->createVertexBuffer(sizeof(quad), true);
   else
      activeDevice->allocateVertexBuffer(mCursorVertexBuffer, sizeof(quad), true);

   void* dst = activeDevice->lockVertexBuffer(mCursorVertexBuffer, sizeof(quad));
   memcpy(dst, quad, sizeof(quad));
   activeDevice->unlockVertexBuffer(mCursorVertexBuffer);

   activeDevice->push(Matrix());
   activeDevice->setParameter(activeDevice->getParameterIndex("alpha"), (128.0f / 255.0f) * alphaFactor);

   glBindBuffer(GL_ARRAY_BUFFER, mCursorVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void MenuPageTextEditItem::updateCursorHighlight()
{
   mCursorTime.restart();
}

bool MenuPageTextEditItem::isCursorAtEnd() const
{
   return (getCursorPosition() == getText().length());
}
