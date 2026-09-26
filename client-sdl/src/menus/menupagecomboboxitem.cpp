// header
#include "menupagecomboboxitem.h"

// menus
#include "clipper.h"
#include "framework/gldevice.h"
#include "math/matrix.h"
#include "menupagebuttonitem.h"
#include "menupagelabelitem.h"
#include "menupagelistitemelement.h"

#include <cstring>

// init static maps
std::map<QString, MenuPageComboBoxItem*> MenuPageComboBoxItem::sMapComboBoxes;
std::map<QString, MenuPageLabelItem*> MenuPageComboBoxItem::sMapLabels;
std::map<QString, MenuPageButtonItem*> MenuPageComboBoxItem::sMapButtons;

MenuPageComboBoxItem::MenuPageComboBoxItem()
    : mVisibleAnimationTime(0.0), mInvisibleAnimationTime(0.0), mButtonItem(0), mLabelItem(0), mQuadVertexBuffer(0)
{
   mPageItemType = PageItemTypeCombobox;
}

void MenuPageComboBoxItem::initialize()
{
   MenuPageListItem::initialize();
   mClipper->setBounds(0, 0, 9999, 9999);

   // initially every combobox is invisible
   setVisible(false);

   mVerticalSpacing = 0;
}

void MenuPageComboBoxItem::setFocus(bool focus)
{
   MenuPageListItem::setFocus(focus);

   if (!focus)
      setVisible(false);
}

bool MenuPageComboBoxItem::isModal() const
{
   return true;
}

void MenuPageComboBoxItem::setVisible(bool visible)
{
   if (visible != isVisible())
   {
      // change button visibility
      MenuPageButtonItem* button = getButtonItem();

      if (button)
         button->setVisible(!visible);

      // change label visibility
      MenuPageLabelItem* label = getLabelItem();

      if (label)
         label->setVisible(!visible);

      MenuPageListItem::setVisible(visible);
   }
}

//-----------------------------------------------------------------------------
/*!
   Legacy glBegin(GL_QUADS)/glColor4ub draw becomes a small dynamic vertex buffer through the
   shared texalphaignore shader (already bound by MenuDrawable for the whole page-render pass) -
   same treatment as MenuPageBackgroundItem's animated quad.
*/
void MenuPageComboBoxItem::drawQuad(PSDLayer* layer, float x, float y, float width, float height, int opacity)
{
   glBindTexture(GL_TEXTURE_2D, layer->getTexture());

   float u = layer->getU();
   float v = layer->getV();

   const float quad[] = {
      x, y, 0.0f, 0.0f, 0.0f, x,         y + height, 0.0f, 0.0f, v, x + width, y + height, 0.0f, u, v,
      x, y, 0.0f, 0.0f, 0.0f, x + width, y + height, 0.0f, u,    v, x + width, y,          0.0f, u, 0.0f,
   };

   if (mQuadVertexBuffer == 0)
      mQuadVertexBuffer = activeDevice->createVertexBuffer(sizeof(quad), true);
   else
      activeDevice->allocateVertexBuffer(mQuadVertexBuffer, sizeof(quad), true);

   void* dst = activeDevice->lockVertexBuffer(mQuadVertexBuffer, sizeof(quad));
   memcpy(dst, quad, sizeof(quad));
   activeDevice->unlockVertexBuffer(mQuadVertexBuffer);

   activeDevice->push(Matrix());
   activeDevice->setParameter(activeDevice->getParameterIndex("alpha"), opacity / 255.0f);

   glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();
}

void MenuPageComboBoxItem::updateTableBounds()
{
   // call base to update maximum height
   MenuPageListItem::updateTableBounds();

   // update layer bounds
   int top = getCurrentLayer()->getTop();
   int maxH = getMaxTableHeight();

   getCurrentLayer()->setBottom(top + maxH);
}

void MenuPageComboBoxItem::draw()
{
   if (isVisible())
   {
      float yStep = mRowHeight + mVerticalSpacing;
      float yOffset = 0.0f;
      float height = 0.0f;
      float width = 0.0f;
      float x = 0.0f;
      float y = 0.0f;

      // draw first element layer
      height = mRowHeight;
      width = getLayerFirstElement()->getWidth();
      x = getLayerFirstElement()->getLeft();
      y = getLayerFirstElement()->getTop();

      drawQuad(getLayerFirstElement(), x, y, width, height + 1);

      // draw n-1th element layer
      height = mRowHeight;
      width = getLayerDefaultElement()->getWidth();
      x = getLayerDefaultElement()->getLeft();
      y = getLayerFirstElement()->getTop();

      for (int i = 1; i < mElements.size() - 1; i++)
      {
         yOffset += yStep;
         drawQuad(getLayerDefaultElement(), x, y + yOffset, width, height);
      }

      // draw last element layer
      height = mRowHeight;
      width = getLayerLastElement()->getWidth();
      x = getLayerLastElement()->getLeft();
      y = getLayerFirstElement()->getTop();

      yOffset += yStep;
      drawQuad(getLayerLastElement(), x, y + yOffset, width, height);

      // draw gradient
      height = mElements.size() * mRowHeight;
      width = getLayerGradientElement()->getWidth();
      x = getLayerGradientElement()->getLeft();
      y = getLayerFirstElement()->getTop();

      drawQuad(getLayerGradientElement(), x, y, width, height, static_cast<int>(getLayerGradientElement()->getOpacity() * 255.0f));

      // call parent
      MenuPageListItem::draw();
   }
}

void MenuPageComboBoxItem::animate(float time)
{
   MenuPageListItem::animate(time);
}

void MenuPageComboBoxItem::dropDownEnabled(bool /*enabled*/)
{
}

void MenuPageComboBoxItem::addComboBox(const QString& key, MenuPageComboBoxItem* item)
{
   sMapComboBoxes[key] = item;
}

void MenuPageComboBoxItem::addButton(const QString& key, MenuPageButtonItem* item)
{
   sMapButtons[key] = item;
}

void MenuPageComboBoxItem::addLabel(const QString& key, MenuPageLabelItem* item)
{
   sMapLabels[key] = item;
}

void MenuPageComboBoxItem::linkComboBoxToButton(const QString& buttonKey, const QString& comboBoxKey)
{
   if (sMapButtons.contains(buttonKey) && sMapComboBoxes.contains(comboBoxKey))
   {
      MenuPageButtonItem* button = sMapButtons[buttonKey];
      MenuPageComboBoxItem* comboBox = sMapComboBoxes[comboBoxKey];

      if (!comboBox->getButtonItem())
      {
         comboBox->setButtonItem(button);

         button->actionSignal.connect([comboBox](const std::string&) { comboBox->setVisible(true); });
      }
   }
}

void MenuPageComboBoxItem::linkComboBoxToLabel(const QString& labelKey, const QString& comboBoxKey)
{
   if (sMapLabels.contains(labelKey) && sMapComboBoxes.contains(comboBoxKey))
   {
      MenuPageLabelItem* label = sMapLabels[labelKey];
      MenuPageComboBoxItem* comboBox = sMapComboBoxes[comboBoxKey];
      comboBox->setLabelItem(label);

      comboBox->valueChangedSignal.connect([label](const std::string& value) { label->setText(QString::fromStdString(value)); });
   }
}

void MenuPageComboBoxItem::setButtonItem(MenuPageButtonItem* item)
{
   mButtonItem = item;
}

MenuPageButtonItem* MenuPageComboBoxItem::getButtonItem() const
{
   return mButtonItem;
}

MenuPageButtonItem* MenuPageComboBoxItem::getButtonItem(const QString& name)
{
   MenuPageButtonItem* button = 0;

   auto it = sMapButtons.find(name);
   if (it != sMapButtons.end())
   {
      button = it->second;
   }

   return button;
}

void MenuPageComboBoxItem::setLabelItem(MenuPageLabelItem* item)
{
   mLabelItem = item;
}

MenuPageLabelItem* MenuPageComboBoxItem::getLabelItem() const
{
   return mLabelItem;
}

void MenuPageComboBoxItem::mousePressed(int x, int y)
{
   if (isVisible())
   {
      MenuPageListItem::mousePressed(x, y);

      if (mActiveElement < mElements.size())
      {
         if (mElements.at(mActiveElement)->isActive())
         {
            setFocus(false);

            QString value = mElements.at(mActiveElement)->getText();
            valueChangedSignal(value.toStdString());
         }
      }
   }
}

QString MenuPageComboBoxItem::getValue() const
{
   QString value;

   if (getLabelItem())
   {
      value = getLabelItem()->getText();
   }

   return value;
}

void MenuPageComboBoxItem::setValue(const QString& value)
{
   if (getLabelItem())
   {
      getLabelItem()->setText(value);
   }
}
