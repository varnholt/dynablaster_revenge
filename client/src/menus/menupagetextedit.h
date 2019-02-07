#ifndef MENUPAGETEXTEDIT_H
#define MENUPAGETEXTEDIT_H

// base
#include "menupageitem.h"
#include "framework/frametimer.h"

// Qt
#include <QColor>
#include <QRegExp>

// forward declarations
class BitmapFont;


class MenuPageTextEditItem : public MenuPageItem
{
   Q_OBJECT

   public:

      //! constructor
      MenuPageTextEditItem();

       // main

       //! draw textedit
       virtual void draw();

       //! initialize textedit
       virtual void initialize();

       //! getter for field width
       int getFieldWidth() const;

       //! setter for max length
       void setMaxLength(int maxLength);

       //! getter for max length
       int getMaxLength() const;

       //! getter for scale
       float getScale() const;

       //! getter for text
       const QString& getText() const;

       //! getter for color
       const QColor& getColor() const;

       //! check if action request on click is enabled
       virtual bool isActionRequestOnClickEnabled() const;

       //! setter for cursor position
       void setCursorPosition(int);

       //! getter for cursor position
       int getCursorPosition() const;

       //! check if editing is active
       bool isEditingActive() const;


   public slots:

      void setText(const QString&);

      void setScale(float scale);

      void setColor(const QColor& color);

      void setOutlineColor(const QColor& outlineColor);

      void setAlpha(int alpha);

      void setFontName(const QString& fontName);

      void setRegExp(const QRegExp& regexp);

      void setFontXOffset(int xOffset);

      void setFontYOffset(int yOffset);

      void setFieldWidth(int fieldWidth);


      virtual void keyPressed(int key, const QString& text);

      virtual void activated();

      virtual void deactivated();

      virtual void paste(const QString & text);


   protected slots:

       //! update the cursor's highlight
       void updateCursorHighlight();


   protected:

       //! getter for cursor at end state
       bool isCursorAtEnd() const;

       //! cursor right
       void moveCursorRight();

       //! cursor left
       void moveCursorLeft();

       //! cursor to start
       void moveCursorToStart();

       //! cursor to end
       void moveCursorToEnd();

       //! draw the cursor
       void drawCursor();

       QString mFontName;

       FrameTimer mTimer;

       FrameTimer mCursorTime;

       BitmapFont* mFont;

       QString mText;

       QRegExp mRegexp;

       int mFontXOffset;

       int mFontYOffset;

       int mFieldWidth;

       int mMaxLength;

       float mScale;

       bool mEditingActive;

       bool mCursorVisible;

       QColor mColor;

       int mAlpha;

       QColor mOutlineColor;

       /*
         font_name = bomberman
         font_x_offset = 10
         font_y_offset = 10
         max_lenght = 5
      */

       int mCursorPosition;
};

#endif // MENUPAGETEXTEDIT_H
