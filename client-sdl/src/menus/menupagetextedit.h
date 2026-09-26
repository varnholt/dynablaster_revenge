#pragma once

// base
#include "framework/frametimer.h"
#include "menupageitem.h"

// math
#include "math/color.h"

#include <QString>

// forward declarations
class BitmapFont;

/// \brief GLES3 port of client/src/menus/menupagetextedit.cpp.
/// drawCursor()'s highlight quad drops the mColor tint (always drawn white now) - it used to be
/// an untextured glColor4ub'd quad, which has no direct GLES3 equivalent (every draw needs a
/// real bound texture); see the .cpp for the replacement (a lazily-created 1x1 white texture
/// through the shared texalphaignore shader).
/// setRegExp()/mRegexp are dropped - QRegExp doesn't exist in Qt6 (superseded by
/// QRegularExpression), and the field was already dead in the original: stored by the setter
/// but never actually read anywhere in this file.
class MenuPageTextEditItem : public MenuPageItem
{
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
   const Color& getColor() const;

   //! check if action request on click is enabled
   virtual bool isActionRequestOnClickEnabled() const;

   //! setter for cursor position
   void setCursorPosition(int);

   //! getter for cursor position
   int getCursorPosition() const;

   //! check if editing is active
   bool isEditingActive() const;

   void setText(const QString&);

   void setScale(float scale);

   void setColor(const Color& color);

   void setOutlineColor(const Color& outlineColor);

   void setAlpha(int alpha);

   void setFontName(const QString& fontName);

   void setFontXOffset(int xOffset);

   void setFontYOffset(int yOffset);

   void setFieldWidth(int fieldWidth);

   virtual void keyPressed(int key, const std::string& text);

   virtual void activated();

   virtual void deactivated();

   virtual void paste(const std::string& text);

protected:

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

   int mFontXOffset;

   int mFontYOffset;

   int mFieldWidth;

   int mMaxLength;

   float mScale;

   bool mEditingActive;

   bool mCursorVisible;

   Color mColor;

   int mAlpha;

   Color mOutlineColor;

   int mCursorPosition;

   // lazily created 1x1 white texture + dynamic quad buffer for drawCursor() - see class
   // comment.
   unsigned int mCursorTexture;
   unsigned int mCursorVertexBuffer;
};
