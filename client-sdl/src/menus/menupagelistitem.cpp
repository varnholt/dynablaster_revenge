// header
#include "menupagelistitem.h"

// math
#include <algorithm>
#include <cmath>
#include <cstring>
#include <numbers>

// menus
#include "clipper.h"
#include "menupagelistitemelement.h"

#include "defaultshader.h"
#include "framework/gldevice.h"
#include "math/matrix.h"

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
      mShader(0),
      mParamTextureClamp(0),
      mParamTextureHighlight(0),
      mParamRowAlpha(0),
      mRowVertexBuffer(0),
      mBlendDuration(0.0f),
      mYOffsetSource(0.0f),
      mYOffsetDest(0.0f),
      mYDest(0.0f),
      mMouseY(0)
{
   mPageItemType = PageItemTypeList;
   mInteractive = true;

   mRowAlpha[0] = 15;
   mRowAlpha[1] = 20;
}

//-----------------------------------------------------------------------------
/*!
 */
MenuPageListItem::~MenuPageListItem()
{
   delete mClipper;
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
void MenuPageListItem::initializeItem(MenuPageListItemElement* element, int index)
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
void MenuPageListItem::appendItem(const QString& text, const Color& color, bool overrideAlpha, const Color& outlineColor)
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
   mElements.push_back(element);

   // reinit table bounds
   updateTableBounds();
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::updateTableBounds()
{
   // init list item's bounds
   mHeightAllElements = (mElements.size() * mRowHeight)               // pixels per element
                        + (mElements.size() - 1) * mVerticalSpacing;  // pixels between elements
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

   // init shader - ShaderPool is not ported (fully superseded by GLDevice::loadShader/setShader,
   // see project memory), so load it the same way every material in engine/materials does.
   mShader = activeDevice->loadShader("data/shaders/listhighlight-vert.glsl", "data/shaders/listhighlight-frag.glsl");
   mParamTextureClamp = activeDevice->getParameterIndex("textureClamp");
   mParamTextureHighlight = activeDevice->getParameterIndex("textureHighlight");
   mParamRowAlpha = activeDevice->getParameterIndex("rowAlpha");
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::updateScrollbars()
{
   float percent = (float)(mY) / (-mHeightAllElements + mLayerActive->getHeight());
   scrollAnimationSignal(percent);
}

//-----------------------------------------------------------------------------
/*!
   \param y without limit
   \return limited y
*/
void MenuPageListItem::limitY(float& y)
{
   if (y > 0.0f)
   {
      y = 0.0f;
   }

   else if (mHeightAllElements + y < mLayerActive->getHeight() && mHeightAllElements >= mLayerActive->getHeight())
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

   float val = mScrollingActive ? scrollValueMoving : scrollValueStopping;

   mY += val;

   // smooth scrolling animation to target position
   float duration = getBlendDuration() * 1000.0f;
   if (mBlendTimer.elapsed() < duration)
   {
      float elapsed = mBlendTimer.elapsed();

      float a = 0.5f * (1.0f + cos(std::numbers::pi_v<float> * (elapsed / duration)));
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

   limitY(mY);

   // scroll animation
   if ((val < 0.1f && val > 0) || (val > -0.1f && val < 0))
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
   selectAlpha used to be a glColor4ub call (fixed-function per-vertex color); the row quad has
   no other color channel that varies, so this now just computes the alpha that will go into the
   "rowAlpha" uniform right before the row is drawn.
*/
void MenuPageListItem::selectAlpha(int rowToggle, MenuPageListItemElement* element)
{
   float alpha;

   if (mHighlightingActive)
   {
      if (element->isFadingOut())
      {
         alpha = (mRowAlpha[rowToggle] + 30 * element->getFadeOutValue()) / 255.0f;
      }
      else if (element->isFocussed() || element->isActive())
      {
         alpha = (mRowAlpha[rowToggle] + 30) / 255.0f;
      }
      else
      {
         alpha = mRowAlpha[rowToggle] / 255.0f;
      }
   }
   else
   {
      alpha = mRowAlpha[0] / 255.0f;
   }

   activeDevice->setParameter(mParamRowAlpha, alpha);
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
   if (getLayerFirstElement() || getLayerLastElement())
   {
      activeDevice->setShader(mShader);

      activeDevice->bindSampler(mParamTextureClamp, 0);
      activeDevice->bindSampler(mParamTextureHighlight, 1);
   }
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::releaseShader()
{
   if (getLayerFirstElement() || getLayerLastElement())
   {
      // restore the shared menu shader rather than "no shader" - see defaultshader.h and
      // BitmapFont::draw(), which needs the same fix for the same reason.
      activeDevice->setShader(getDefaultMenuShader());
   }
}

//-----------------------------------------------------------------------------
/*!
 */
PSDLayer* MenuPageListItem::bindRowTexture(int row, float& u, float& v, float& s, float& t)
{
   PSDLayer* layer = 0;

   if (row == 0 && getLayerFirstElement())
   {
      layer = getLayerFirstElement();
   }
   else if (row == mElements.size() - 1 && getLayerLastElement())
   {
      layer = getLayerLastElement();
   }
   else if (row > 0 && row < (mElements.size() - 1) && getLayerDefaultElement())
   {
      layer = getLayerDefaultElement();
   }

   glActiveTexture(GL_TEXTURE0);

   if (layer)
   {
      glBindTexture(GL_TEXTURE_2D, layer->getTexture());
      u = layer->getU();
      v = layer->getV();
   }
   else
   {
      glBindTexture(GL_TEXTURE_2D, 0);
      u = 0.0f;
      v = 0.0f;
   }

   // set selected element textures
   glActiveTexture(GL_TEXTURE1);

   if (getLayerSelectedElement())
   {
      glBindTexture(GL_TEXTURE_2D, getLayerSelectedElement()->getTexture());
      s = getLayerSelectedElement()->getU();
      t = getLayerSelectedElement()->getV();
   }
   else
   {
      glBindTexture(GL_TEXTURE_2D, 0);
      s = 0.0f;
      t = 0.0f;
   }

   glActiveTexture(GL_TEXTURE0);

   return layer;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::drawText()
{
   for (MenuPageListItemElement* element : mElements)
   {
      float opacity = (element->isFocussed() || element->isActive() || element->isOverrideAlphaActive()) ? 1.0f : 0.5882f;

      Array<Vertex> bound = element->getBoundingRectVertices(mLayerActive->getLeft(), mLayerActive->getTop() + mY);

      if (mClipper->enable(bound))
      {
         element->draw(mLayerActive->getLeft(), mLayerActive->getTop() + mY, opacity);

         mClipper->disable();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   The legacy glBegin(GL_QUADS)/glMultiTexCoord2f per-row draw becomes a small dynamic vertex
   buffer (pos + uvClamp + uvHighlight) drawn as two triangles - same treatment as
   MenuPageBackgroundItem's animated quad. The quad's 4-vertex loop (bottom-left, bottom-right,
   top-right, top-left - see MenuPageListItemElement::getBoundingRectVertices) is triangulated
   as (0,1,2)/(0,2,3).
*/
void MenuPageListItem::drawRows()
{
   // a plain (non-combobox) list - e.g. the lounge's chat table_lounge_main - never gets first/
   // last/default row-background layers assigned (see MenuPage::processTableMain() vs. the
   // combobox-only setLayerFirstElement()/setLayerLastElement()/setLayerDefaultElement() calls),
   // so bindRowTexture() would bind "no texture" (texture id 0) for every row. Under the original
   // desktop-GL renderer this was harmless - bindShader()/releaseShader() below never bound a
   // shader either, so the fixed-function pipeline just passed through untouched, rendering
   // nothing extra. GLES3 has no such fallback: without an explicit early-out here, this loop
   // would draw a fully opaque, garbage-shaded quad over every row using whatever shader was left
   // bound by the previous draw call (BitmapFont's SDF text shader) - a real, previously-dormant
   // bug that only ever showed up once a plain (non-combobox) list actually got real content
   // (this port's chat feature, added later than the already-working combobox dropdown).
   if (!getLayerFirstElement() && !getLayerLastElement())
      return;

   bindShader();

   // draw rows
   int rowToggle = 0;
   int row = 0;
   for (MenuPageListItemElement* element : mElements)
   {
      Array<Vertex> boundingRect = element->getBoundingRectVertices(mLayerActive->getLeft(), mLayerActive->getTop() + mY);

      if (mClipper->enable(boundingRect))
      {
         float u, v, s, t;
         bindRowTexture(row, u, v, s, t);

         selectAlpha(rowToggle, element);

         const int order[6] = {0, 1, 2, 0, 2, 3};
         float quad[6 * 7];

         for (int i = 0; i < 6; i++)
         {
            const Vertex& vtx = boundingRect[order[i]];
            float* dst = quad + i * 7;
            dst[0] = vtx.x;
            dst[1] = vtx.y;
            dst[2] = 0.0f;
            dst[3] = vtx.u * u;
            dst[4] = vtx.v * v;
            dst[5] = vtx.u * s;
            dst[6] = vtx.v * t;
         }

         if (mRowVertexBuffer == 0)
            mRowVertexBuffer = activeDevice->createVertexBuffer(sizeof(quad), true);
         else
            activeDevice->allocateVertexBuffer(mRowVertexBuffer, sizeof(quad), true);

         void* dst = activeDevice->lockVertexBuffer(mRowVertexBuffer, sizeof(quad));
         memcpy(dst, quad, sizeof(quad));
         activeDevice->unlockVertexBuffer(mRowVertexBuffer);

         activeDevice->push(Matrix());

         glBindBuffer(GL_ARRAY_BUFFER, mRowVertexBuffer);
         glEnableVertexAttribArray(0);
         glEnableVertexAttribArray(1);
         glEnableVertexAttribArray(2);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (GLvoid*)0);
         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (GLvoid*)(sizeof(float) * 3));
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (GLvoid*)(sizeof(float) * 5));

         glDrawArrays(GL_TRIANGLES, 0, 6);

         glDisableVertexAttribArray(0);
         glDisableVertexAttribArray(1);
         glDisableVertexAttribArray(2);

         activeDevice->pop();

         if (element->isFadingOut() && element->getFadeOutValue() < 0.1f)
         {
            element->stopFadeOut();
         }

         mClipper->disable();
      }

      rowToggle ^= 1;
      row++;
   }

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
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE0);
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::setFontName(const QString& fontName)
{
   mFontName = fontName;
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
   int count = std::max(getElementCount(), 1);
   float percent = index / (float)count;

   if (percent < 0.0f)
      percent = 0.0f;
   if (percent > 1.0f)
      percent = 1.0f;
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

   float dest = tableVerticalCenter - mHeightAllElements + ((elementCount - index) * oneRowHeight);

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

   int tableTop = mLayerActive->getTop();
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
      scrollAnimationSignal(percent);
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

   if (focussedElement > -1 && mElements.size() > focussedElement)
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

   if (activeElement > -1 && mElements.size() > activeElement)
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
std::vector<MenuPageListItemElement*>* MenuPageListItem::getElements() const
{
   return &mElements;
}

//-----------------------------------------------------------------------------
/*!
 */
MenuPageListItemElement* MenuPageListItem::getElementAt(int i) const
{
   return mElements.at(i);
}

//-----------------------------------------------------------------------------
/*!
 */
const QString& MenuPageListItem::getElementText(int element)
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

   elementFocussedSignal(element);
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
void MenuPageListItem::setLayerFirstElement(PSDLayer* layer)
{
   mLayerFirstElement = layer;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::setLayerDefaultElement(PSDLayer* layer)
{
   mLayerDefaultElement = layer;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::setLayerLastElement(PSDLayer* layer)
{
   mLayerLastElement = layer;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::setLayerGradientElement(PSDLayer* layer)
{
   mLayerGradient = layer;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::setLayerSelectedElement(PSDLayer* layer)
{
   mLayerSelectedElement = layer;
}

//-----------------------------------------------------------------------------
/*!
 */
void MenuPageListItem::setLayerFocussedElement(PSDLayer* layer)
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
