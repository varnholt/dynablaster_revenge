// header
#include "menupagelistitem.h"

// math
#include "math.h"

// menus
#include "clipper.h"
#include "menupagelistitemelement.h"

#include "gldevice.h"
#include "shaderpool.h"

// Qt
#include <QGLContext>
#include <QGLShaderProgram>

#define SCROLL_SPEED 5.0


//-----------------------------------------------------------------------------
/*!
*/
MenuPageListItem::MenuPageListItem()
 : mClipper(0),
   mX(0.0f),
   mY(0.0f),
   mWidthAllElements(0.0f),
   mHeightAllElements(0.0f),
   mFontXOffset(0),
   mFontYOffset(0),
   mFieldWidth(255),
   mScale(0.0f),
   mScrollValue(0.0f),
   mVerticalSpacing(0),
   mRowHeight(0),
   mFocussedElement(0),
   mActiveElement(0),
   mScrollingActive(false),
   mHighlightingActive(true),
   mLayerFirstElement(0),
   mLayerDefaultElement(0),
   mLayerLastElement(0),
   mLayerGradient(0),
   mLayerSelectedElement(0),
   mLayerFocussedElement(0),
   mSamplerTextureClamp(0),
   mSamplerTextureHighlight(0),
   mBlendDuration(0.0f),
   mYOffsetSource(0.0f),
   mYOffsetDest(0.0f),
   mYDest(0.0f),
   mMouseY(0)
{
   mPageItemType = PageItemTypeList;
   mInteractive = true;

   mRowAlpha[0]=15;
   mRowAlpha[1]=20;
}


//-----------------------------------------------------------------------------
/*!
*/
MenuPageListItemElement* MenuPageListItem::itemInstance()
{
   return new MenuPageListItemElement();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::initializeItem(
   MenuPageListItemElement* element,
   int index
)
{
   if (mLayerActive)
   {
      // set element properties
      element->setIndex(index);
      element->setHeight(mRowHeight);
      element->setWidth(mLayerActive->getWidth());
      element->setX(0);
      element->setY(index * mRowHeight + index * mVerticalSpacing);

      // lineedit properties
      element->setRegExp(mRegexp);
      element->setFontXOffset(mFontXOffset);
      element->setFontYOffset(mFontYOffset);
      element->setFieldWidth(mFieldWidth);
      element->setScale(mScale);
   }
   else
   {
      qWarning(
         "MenuPageListItem::initializeItem: you can't initialize list items elements "
         "without initializing the pagelist item"
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::clear()
{
   mElements.clear();

   // reinit table bounds
   updateTableBounds();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setHighlightingEnabled(bool enabled)
{
   mHighlightingActive = enabled;
}


//-----------------------------------------------------------------------------
/*!
*/
bool MenuPageListItem::isHighlightingEnabled() const
{
   return mHighlightingActive;
}


//-----------------------------------------------------------------------------
/*!
   \param item text to add
   \param color item's color
   \param overrideAlpha \c true if alpha is overriden
   \param outlineColor item's outline color
*/
void MenuPageListItem::appendItem(
   const QString& text,
   const QColor& color,
   bool overrideAlpha,
   const QColor& outlineColor
)
{
   // get a item instance
   MenuPageListItemElement* element = itemInstance();
   element->setParent(this);

   // set text and color
   element->setFontName(mFontName);
   element->setText(text);
   element->setColor(color);
   element->setOverrideAlpha(overrideAlpha);

   if (outlineColor.isValid())
      element->setOutlineColor(outlineColor);

   // set element properties
   initializeItem(element, mElements.size());

   // generate element's vertices
   element->initialize();

   // add item to list
   mElements << element;

   // reinit table bounds
   updateTableBounds();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::updateTableBounds()
{
   // init list item's bounds
   mHeightAllElements =
         (mElements.size() * mRowHeight)            // pixels per element
       + (mElements.size() - 1) * mVerticalSpacing; // pixels between elements

}


//-----------------------------------------------------------------------------
/*!
*/
int MenuPageListItem::getMaxTableHeight() const
{
   return mHeightAllElements;
}


//-----------------------------------------------------------------------------
/*!
*/
int MenuPageListItem::getMaxTableWidth() const
{
   return mWidthAllElements;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::initialize()
{
   mVerticalSpacing = 3;

   // init clipper
   mClipper = new Clipper(
      mLayerActive->getLeft(),
      mLayerActive->getTop(),
      mLayerActive->getLeft() + mLayerActive->getWidth(),
      mLayerActive->getTop() + mLayerActive->getHeight()
   );

   mElapsed.start();

   // init shader
   ShaderPool::registerShaderFromFile(
      "data/shaders/listhighlight-vert.glsl",
      "data/shaders/listhighlight-frag.glsl"
   );

   QGLShaderProgram* shader = ShaderPool::getProgram("data/shaders/listhighlight-vert.glsl");
   mSamplerTextureClamp = shader->uniformLocation("textureClamp");
   mSamplerTextureHighlight = shader->uniformLocation("textureHighlight");
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::updateScrollbars()
{
   float percent = (float)(mY) / (-mHeightAllElements + mLayerActive->getHeight());
   emit scrollAnimation(percent);
}


//-----------------------------------------------------------------------------
/*!
   \param y without limit
   \return limited y
*/
void MenuPageListItem::limitY(float &y)
{
   if (y > 0.0f)
   {
      y = 0.0f;
   }

   else if (
         mHeightAllElements + y  < mLayerActive->getHeight()
      && mHeightAllElements      >= mLayerActive->getHeight()
   )
   {
      y = mLayerActive->getHeight() - mHeightAllElements;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::animate(float /*time*/)
{
   // simple mouse-triggered scrolling animation
   float deceleration = 1.0f + sin(mElapsed.elapsed() * 0.01f);
   float scrollValueMoving = mScrollValue * SCROLL_SPEED;
   float scrollValueStopping = (deceleration) * 0.5f * scrollValueMoving;

   float val =
      mScrollingActive
         ? scrollValueMoving
         : scrollValueStopping;

   mY += val;

   // smooth scrolling animation to target position
   float duration = getBlendDuration() * 1000.0f;
   if (mBlendTimer.elapsed() < duration)
   {
      float elapsed = mBlendTimer.elapsed();

      float a = 0.5f * (1.0f + cos( M_PI * (elapsed / duration) ));
      float b = 1.0f - a;

      mY = a * getYOffsetSource() + b * getYOffsetDest();

      updateScrollbars();

      // update mouse cursor position if somewhere between the very
      // upper and the very lower part of the table so the correct
      // element is highlighted
      int relY = 0;
      relY = mMouseY - mLayerActive->getTop();
      relY -= mYDest;
      updateFocussedElement(relY);
   }

   /*

     /=========================\
     |[.......................]|  mY
     |[.......................]|
     |[.......................]|
     |[.......................]|
     |[.......................]|
     |[.......................]|
     |[.......................]|
     \=========================/  mLayerActive->getHeight()
      [.......................]
      [.......................]
      [.......................]   mMaxY
   */

   /*

     remember:

     1) mY goes into negative direction
        => if > 0, cut at 0

     2) mActiveLayer->getHeight(): 548

        mHeightAllElements gives for mY = -244 => 792
                                 for mY = -297 => 845

                                 845 - 297 = 548
   */

   limitY(mY);

   // scroll animation
   if (
         (val <  0.1f && val > 0)
      || (val > -0.1f && val < 0)
   )
   {
      mScrollValue = 0.0f;
   }
   else if (mScrollValue != 0.0f)
   {
      updateScrollbars();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::selectAlpha(int rowToggle, MenuPageListItemElement* element)
{
   if (mHighlightingActive)
   {
      // draw item highlight
      if (element->isFadingOut())
      {
         glColor4ub(
            255,
            255,
            255,
            mRowAlpha[rowToggle] + 30 * element->getFadeOutValue()
         );
      }
      else if (element->isFocussed() || element->isActive())
      {
         glColor4ub(
            255,
            255,
            255,
            mRowAlpha[rowToggle] + 30
         );
      }
      else
      {
         glColor4ub(
            255,
            255,
            255,
            mRowAlpha[rowToggle]
         );
      }
   }
   else
   {
      glColor4ub(
         255,
         255,
         255,
         mRowAlpha[0]
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
float MenuPageListItem::getYOffsetDest() const
{
   return mYOffsetDest;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setYOffsetDest(float value)
{
   mYOffsetDest = value;
}


//-----------------------------------------------------------------------------
/*!
*/
float MenuPageListItem::getYOffsetSource() const
{
   return mYOffsetSource;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setYOffsetSource(float value)
{
   mYOffsetSource = value;
}


//-----------------------------------------------------------------------------
/*!
*/
float MenuPageListItem::getBlendDuration() const
{
   return mBlendDuration;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setBlendDuration(float value)
{
   mBlendDuration = value;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setRowAlphas(int row0, int row1)
{
   mRowAlpha[0] = row0;
   mRowAlpha[1] = row1;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::bindShader()
{
   if (
       getLayerFirstElement()
      || getLayerLastElement()
   )
   {
      QGLShaderProgram* shader = ShaderPool::getProgram("data/shaders/listhighlight-vert.glsl");
      shader->bind();

      glUniform1i(mSamplerTextureClamp,     0); // GL_TEXTURE0
      glUniform1i(mSamplerTextureHighlight, 1); // GL_TEXTURE1
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::releaseShader()
{
   if (
         getLayerFirstElement()
      || getLayerLastElement()
   )
   {
      ShaderPool::getProgram("data/shaders/listhighlight-vert.glsl")->release();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::bindRowTexture(int row, float& u, float& v, float& s, float& t)
{
   PSDLayer *layer= 0;

   if (
         row == 0
      && getLayerFirstElement()
   )
   {
      layer= getLayerFirstElement();
      glBindTexture(GL_TEXTURE_2D, getLayerFirstElement()->getTexture());
   }
   else if (
         row == mElements.size() - 1
      && getLayerLastElement()
   )
   {
      layer= getLayerLastElement();
      glBindTexture(GL_TEXTURE_2D, getLayerLastElement()->getTexture());
   }
   else if (
         row > 0
      && row < (mElements.size() -1)
      && getLayerDefaultElement()
   )
   {
      layer= getLayerDefaultElement();
      glBindTexture(GL_TEXTURE_2D, getLayerDefaultElement()->getTexture());
   }

   if (layer)
   {
      glBindTexture(GL_TEXTURE_2D, layer->getTexture());
      u= layer->getU();
      v= layer->getV();
   }
   else
   {
      glBindTexture(GL_TEXTURE_2D, 0);
      u= 0.0f;
      v= 0.0f;
   }

   // set selected element textures
   if (getLayerSelectedElement())
   {
      glActiveTexture(GL_TEXTURE1);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, getLayerSelectedElement()->getTexture());
      glActiveTexture(GL_TEXTURE0);
      s= getLayerSelectedElement()->getU();
      t= getLayerSelectedElement()->getV();
   }
   else
   {
      glActiveTexture(GL_TEXTURE1);
      glDisable(GL_TEXTURE_2D);
      glActiveTexture(GL_TEXTURE0);
      s= 0.0f;
      t= 0.0f;
   }

   return layer;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::drawText()
{
   foreach(MenuPageListItemElement* element, mElements)
   {
      float opacity=
         (element->isFocussed() || element->isActive() || element->isOverrideAlphaActive())
            ? 1.0f : 0.5882f;

      Array<Vertex> bound = element->getBoundingRectVertices(
            mLayerActive->getLeft(),
            mLayerActive->getTop() + mY );

      if (mClipper->enable(bound))
      {
         element->draw(
            mLayerActive->getLeft(),
            mLayerActive->getTop() + mY,
            opacity
         );

         mClipper->disable();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::drawRows()
{
   /*
          :
          :
        QUAD (out of scope)
      +----------------------------+--------------------+ ----- clip rect top
      | QUAD                       | QUAD (scroll up)   |
      | QUAD                       +--------------------+
      | QUAD                       |                    |
      | QUAD                       | QUAD (transparent) |
      | QUAD                       |                    |
      | QUAD                       +--------------------+
      | QUAD                       | QUAD (scroll down )|
      +----------------------------+--------------------+ ----- clip rect bottom
        QUAD (out of scope)
         :
         :

   */

   bindShader();

   // draw rows
   int rowToggle= 0;
   int row = 0;
   foreach(MenuPageListItemElement* element, mElements)
   {
      Array<Vertex> boundingRect = element->getBoundingRectVertices(
            mLayerActive->getLeft(),
            mLayerActive->getTop() + mY
         );

      if (mClipper->enable(boundingRect))
      {
         float u,v, s,t;
         bindRowTexture(row, u,v, s,t);

         selectAlpha(rowToggle, element);

         glBegin(GL_QUADS);
         for (int br = 0; br < boundingRect.size(); br++)
         {
            // default tablelistitem version
            const Vertex& vtx= boundingRect[br];
            glMultiTexCoord2f(GL_TEXTURE0, vtx.u*u, vtx.v*v);
            glMultiTexCoord2f(GL_TEXTURE1, vtx.u*s, vtx.v*t);

            glVertex3f(
               vtx.x,
               vtx.y,
               0
            );
         }

         if (
               element->isFadingOut()
            && element->getFadeOutValue() < 0.1f
         )
         {
            element->stopFadeOut();
         }

         glEnd();

         mClipper->disable();         
      }
//      else
//      {
//         qDebug("item is clipped: %s", qPrintable(element->getText()));
//      }

      rowToggle^=1;
      row++;
   }

   glColor4f(1,1,1,1);

   releaseShader();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::draw()
{
   drawText();
   drawRows();

   // release texture 1
   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0);
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setFontName(const QString &fontName)
{
   mFontName = fontName;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setRegExp(const QRegExp& regexp)
{
   mRegexp = regexp;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setFontXOffset(int xOffset)
{
   mFontXOffset = xOffset;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setFontYOffset(int yOffset)
{
   mFontYOffset = yOffset;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setFieldWidth(int fieldWidth)
{
   mFieldWidth = fieldWidth;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setScale(float scale)
{
   mScale = scale;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setRowHeight(int height)
{
   mRowHeight = height;
}


//-----------------------------------------------------------------------------
/*!
   \param index item index
*/
void MenuPageListItem::scrollToIndex(int index, bool /*clicked*/)
{
   int count = qMax(getElementCount(), 1);
   float percent = index / (float)count;

   if (percent < 0.0f)
      percent = 0.0f;
   if (percent > 1.0f)
      percent = 1.0f;

   // qDebug("MenuPageListItem::scrollToIndex: %f", percent);
}


//-----------------------------------------------------------------------------
/*!
   \param index item index
*/
int MenuPageListItem::scrollSmoothToIndex(int index)
{
   // autocorrect input
   int elementCount = getElementCount();
   if (index < 0)
      index = 0;
   if (index > elementCount - 1)
      index = elementCount - 1;

   // init distance and target position
   float oneRowHeight = mRowHeight + mVerticalSpacing;
   float tableVerticalCenter = (mLayerActive->getHeight() * 0.5f);

   float dest =
        tableVerticalCenter
      - mHeightAllElements
      + ((elementCount - index) * oneRowHeight) ;

   // init animation
   setYOffsetSource(mY);
   setYOffsetDest(dest);

   float distance = qAbs(getYOffsetSource() - getYOffsetDest()) / (float)mHeightAllElements;
   float duration = 2.0f * distance;

   setBlendDuration(duration);
   mBlendTimer.restart();

   // compute mouse cursor position
   mYDest = dest;
   limitY(mYDest);

   int tableTop    = mLayerActive->getTop();
   int tableHeight = mLayerActive->getHeight();

   int rowHeight = oneRowHeight * index;
   int mouseOffset = 0;

   // at top
   // initialize mouse offset with the (index * row height)
   if ((int)mYDest == 0)
   {
      mouseOffset = rowHeight;
   }
   // bottom reached
   else if ((int)(mYDest) == -(int)(mHeightAllElements - tableHeight))
   {
      mouseOffset = tableHeight - (mHeightAllElements - rowHeight);
   }
   // in between
   else
   {
      mouseOffset = tableVerticalCenter;
   }

   return tableTop + mouseOffset;
}


//-----------------------------------------------------------------------------
/*!
   \param percent percent to scroll to
   \param clicked \c true if clicked
*/
void MenuPageListItem::scrollToPercentage(float percent, bool clicked)
{
   mY = -mHeightAllElements + mLayerActive->getHeight();
   mY *= percent;

   if (!clicked)
   {
      emit scrollAnimation(percent);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::scrollUp()
{
   mScrollValue = 1.0f;
   mScrollingActive = true;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::scrollDown()
{
   mScrollValue = -1.0f;
   mScrollingActive = true;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::scrollStop()
{
   mElapsed.restart();
   mScrollingActive = false;
}


//-----------------------------------------------------------------------------
/*!
*/
bool MenuPageListItem::hasNestedElements()
{
   return true;
}


//-----------------------------------------------------------------------------
/*!
   \param relY relative y position
*/
void MenuPageListItem::updateFocussedElement(int relY)
{
   int focussedElement = (float)relY / (mVerticalSpacing + mRowHeight);

   if (
          focussedElement > -1
       && mElements.size() > focussedElement
   )
   {
      mElements.at(focussedElement)->setFocus(true);

      // only one element can have focus
      if (focussedElement != mFocussedElement)
      {
         if (mFocussedElement < mElements.size())
         {
            mElements.at(mFocussedElement)->setFocus(false);
         }
      }

      setFocussedElement(focussedElement);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param x x pos
   \param y y pos
*/
void MenuPageListItem::mouseMoved(int /*x*/, int y)
{
   if (isVisible())
   {
      // store last mouse position
      mMouseY = y;

      int relY = 0;
      relY = y - mLayerActive->getTop();
      relY -= mY;

      updateFocussedElement(relY);
   }
}


void MenuPageListItem::mousePressed(int /*x*/, int y)
{
   int relY = 0;

   relY = y - mLayerActive->getTop();
   relY -= mY;

   int activeElement = (float)relY / (mVerticalSpacing + mRowHeight);

   if (
          activeElement > -1
       && mElements.size() > activeElement
   )
   {
      mElements.at(activeElement)->setActive(true);

      // only one element can have focus
      if (activeElement != mActiveElement)
      {
         if (mActiveElement < mElements.size())
            mElements.at(mActiveElement)->setActive(false);
      }

      setActiveElement(activeElement);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
QList<MenuPageListItemElement*>* MenuPageListItem::getElements() const
{
   return &mElements;
}


//-----------------------------------------------------------------------------
/*!
*/
MenuPageListItemElement *MenuPageListItem::getElementAt(int i) const
{
   return mElements.at(i);
}


//-----------------------------------------------------------------------------
/*!
*/
const QString &MenuPageListItem::getElementText(int element)
{
   return mElements.at(element)->getText();
}


//-----------------------------------------------------------------------------
/*!
*/
int MenuPageListItem::getElementCount()
{
   return mElements.size();
}


//-----------------------------------------------------------------------------
/*!
*/
int MenuPageListItem::getActiveElement() const
{
   return mActiveElement;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setActiveElement(int element)
{
   mActiveElement = element;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setElementActive(int activeElement, bool active)
{
   if (activeElement < mElements.size())
      mElements.at(activeElement)->setActive(active);
}


//-----------------------------------------------------------------------------
/*!
*/
int MenuPageListItem::getFocussedElement() const
{
   return mFocussedElement;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setFocussedElement(int element)
{
   mFocussedElement = element;

   emit elementFocussed(element);
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setElementFocussed(int element, bool focussed)
{
   if (element < mElements.size())
      mElements.at(element)->setFocus(focussed);
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setLayerFirstElement(PSDLayer *layer)
{
   mLayerFirstElement = layer;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setLayerDefaultElement(PSDLayer *layer)
{
   mLayerDefaultElement = layer;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setLayerLastElement(PSDLayer *layer)
{
   mLayerLastElement = layer;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setLayerGradientElement(PSDLayer *layer)
{
   mLayerGradient = layer;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setLayerSelectedElement(PSDLayer *layer)
{
   mLayerSelectedElement = layer;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageListItem::setLayerFocussedElement(PSDLayer *layer)
{
   mLayerFocussedElement = layer;
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::getLayerFirstElement() const
{
   return mLayerFirstElement;
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::getLayerDefaultElement() const
{
   return mLayerDefaultElement;
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::getLayerLastElement() const
{
   return mLayerLastElement;
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::getLayerGradientElement() const
{
   return mLayerGradient;
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::getLayerSelectedElement() const
{
   return mLayerSelectedElement;
}


//-----------------------------------------------------------------------------
/*!
*/
PSDLayer* MenuPageListItem::getLayerFocussedElement() const
{
   return mLayerFocussedElement;
}

