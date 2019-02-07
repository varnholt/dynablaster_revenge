#ifndef MENUPAGELISTITEM_H
#define MENUPAGELISTITEM_H

// base
#include "menupageitem.h"
#include "framework/frametimer.h"

// Qt
#include <QColor>
#include <QRegExp>


class Clipper;
class MenuPageListItemElement;

class MenuPageListItem : public MenuPageItem
{
   Q_OBJECT

   public:

      MenuPageListItem();

      // main

      virtual void initialize();

      virtual void draw();

      void setFontName(const QString& fontName);

      void setRegExp(const QRegExp& regexp);

      void setFontXOffset(int xOffset);

      void setFontYOffset(int yOffset);

      void setFieldWidth(int maxChars);

      void setScale(float scale);

      void setRowHeight(int height);

      //! getter for list item elements
      QList<MenuPageListItemElement*>* getElements() const;

      //! getter for element at i
      MenuPageListItemElement* getElementAt(int i) const;

      //! get element text
      const QString& getElementText(int element);

      //! getter for element count
      int getElementCount();

      //! getter for the active element id
      int getActiveElement() const;

      //! setter for active element id
      void setActiveElement(int);

      //! setter for the active element id
      void setElementActive(int, bool);

      //! getter for focussed element id
      int getFocussedElement() const;

      //! setter for focussed element id
      void setFocussedElement(int element);

      //! setter for element focus
      void setElementFocussed(int element, bool focussed);

      //! append an item to the list
      virtual void appendItem(
         const QString& item,
         const QColor& color = QColor("#FFFFFF"),
         bool overrideAlpha = false,
         const QColor& outlineColor = QColor()
      );

      //! clear all items from the list
      void clear();

      //! set list highlighting enabled
      void setHighlightingEnabled(bool enabled);

      //! getter for highlighting flag
      bool isHighlightingEnabled() const;

      //! set row alphas
      void setRowAlphas(int row0, int row1);


      // overwritten

      virtual bool hasNestedElements();

      virtual void mouseMoved(int x, int y);

      virtual void mousePressed(int x, int y);

      virtual void setLayerFirstElement(PSDLayer* layer);
      virtual void setLayerDefaultElement(PSDLayer* layer);
      virtual void setLayerLastElement(PSDLayer* layer);
      virtual void setLayerGradientElement(PSDLayer* layer);
      virtual void setLayerSelectedElement(PSDLayer* layer);
      virtual void setLayerFocussedElement(PSDLayer *layer);

      virtual PSDLayer* getLayerFirstElement() const;
      virtual PSDLayer* getLayerDefaultElement() const;
      virtual PSDLayer* getLayerLastElement() const;
      virtual PSDLayer* getLayerGradientElement() const;
      virtual PSDLayer* getLayerSelectedElement() const;
      virtual PSDLayer *getLayerFocussedElement() const;

      //! getter for blend duration
      float getBlendDuration() const;

      //! setter for blend duration
      void setBlendDuration(float value);

      //! getter for y offset source
      float getYOffsetSource() const;

      //! setter for y offset source
      void setYOffsetSource(float value);

      //! getter for y offset dest
      float getYOffsetDest() const;

      //! setter for y offset dest
      void setYOffsetDest(float value);


   public slots:

      virtual void animate(float time);

      virtual void scrollUp();

      virtual void scrollDown();

      virtual void scrollStop();

      //! scroll to particular percentage of table
      virtual void scrollToPercentage(float percent, bool clicked = true);

      //! scroll to item of given index
      virtual void scrollToIndex(int index, bool clicked = true);

      //! smooth scroll to index
      virtual int scrollSmoothToIndex(int index);


   signals:

      void scrollAnimation(float percent);

      void elementFocussed(int element);


   protected:

      //! draw the text
      void drawText();

      //! draw the rows
      void drawRows();

      //! bind list item shader
      void bindShader();

      //! release list item shader
      void releaseShader();

      //! bind row texture (if present)
      PSDLayer *bindRowTexture(int row, float& u, float& v, float& s, float& t);

      //! generate a new item instance
      MenuPageListItemElement* itemInstance();

      //! initialize item instance
      void initializeItem(
         MenuPageListItemElement* element,
         int index
      );

      //! update table bounds
      virtual void updateTableBounds();

      //! getter for maximum table height
      virtual int getMaxTableHeight() const;

      //! getter for maximum table width
      virtual int getMaxTableWidth() const;

      //! set alpha value for given element
      void selectAlpha(int rowToggle, MenuPageListItemElement* element);

      //! update scrollbars depending on current offset
      void updateScrollbars();

      //! limit y to not allow table movement out of bounds
      void limitY(float& y);

      //! update focussed element from given relative y position
      void updateFocussedElement(int relY);

      //! clipper to clip table to
      Clipper* mClipper;

      //! list of items
      mutable QList<MenuPageListItemElement*> mElements;

      //! time elapsed used for scrolling animation
      FrameTimer mElapsed;


      // offset

      float mX;

      float mY;


      // dimensions

      float mWidthAllElements;

      float mHeightAllElements;


      // properties for single lineedits

      QString mFontName;

      QRegExp mRegexp;

      int mFontXOffset;

      int mFontYOffset;

      int mFieldWidth;

      float mScale;

      float mScrollValue;

      int mVerticalSpacing;

      int mRowHeight;

      int mFocussedElement;

      int mActiveElement;

      bool mScrollingActive;

      bool mHighlightingActive;


      // individual layers for elements (to be used by comboboxes etc)

      //! first element layer
      PSDLayer* mLayerFirstElement;

      //! default element layer
      PSDLayer* mLayerDefaultElement;

      //! last element layer
      PSDLayer* mLayerLastElement;

      //! gradient layer
      PSDLayer* mLayerGradient;

      //! selected element layer
      PSDLayer* mLayerSelectedElement;

      //! focussed element layer
      PSDLayer* mLayerFocussedElement;


      //! sampler for clamp texture
      int mSamplerTextureClamp;

      //! sampler for highlight texture
      int mSamplerTextureHighlight;

      //! alphas
      int mRowAlpha[2];


      // blend between two positions

      //! blend timer
      FrameTimer mBlendTimer;

      //! blend duration
      float mBlendDuration;

      //! source mY
      float mYOffsetSource;

      //! destination mY
      float mYOffsetDest;

      //! copy of dest
      float mYDest;

      //! last mouse y position
      int mMouseY;
};

#endif // MENUPAGELISTITEM_H
