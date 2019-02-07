// header
#include "menupagecomboboxitem.h"

// Qt
#include <QApplication>
#include <QGLContext>

// menus
#include "clipper.h"
#include "menupagebuttonitem.h"
#include "menupagelabelitem.h"
#include "menupagelistitemelement.h"
#include "gldevice.h"

// engine
#include "gldevice.h"

// init static maps
QMap<QString, MenuPageComboBoxItem*> MenuPageComboBoxItem::sMapComboBoxes;
QMap<QString, MenuPageLabelItem*> MenuPageComboBoxItem::sMapLabels;
QMap<QString, MenuPageButtonItem*> MenuPageComboBoxItem::sMapButtons;


MenuPageComboBoxItem::MenuPageComboBoxItem()
 : mVisibleAnimationTime(0.0),
   mInvisibleAnimationTime(0.0),
   mButtonItem(0),
   mLabelItem(0)
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


void MenuPageComboBoxItem::drawQuad(
   PSDLayer* layer,
   float x, float y,
   float width, float height,
   int opacity
)
{
   glBindTexture( GL_TEXTURE_2D, layer->getTexture() );

   glColor4ub(
      255,
      255,
      255,
      opacity
   );

   float u= layer->getU();
   float v= layer->getV();
   glBegin(GL_QUADS);

   glTexCoord2f(0.0f, 0.0f); glVertex3f(x,         y,          0.0f);
   glTexCoord2f(0.0f,    v); glVertex3f(x,         y + height, 0.0f);
   glTexCoord2f(   u,    v); glVertex3f(x + width, y + height, 0.0f);
   glTexCoord2f(   u, 0.0f); glVertex3f(x + width, y,          0.0f);

   glEnd();

   glColor4f(1,1,1,1);
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

      drawQuad(getLayerFirstElement(), x, y, width, height+1);

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

      drawQuad(
         getLayerGradientElement(),
         x, y,
         width, height,
         getLayerGradientElement()->getOpacity()
      );

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


void MenuPageComboBoxItem::addComboBox(const QString & key, MenuPageComboBoxItem * item)
{
   sMapComboBoxes.insert(key, item);
}


void MenuPageComboBoxItem::addButton(const QString & key, MenuPageButtonItem * item)
{
   sMapButtons.insert(key, item);
}


void MenuPageComboBoxItem::addLabel(const QString & key, MenuPageLabelItem * item)
{
   sMapLabels.insert(key, item);
}


void MenuPageComboBoxItem::linkComboBoxToButton(
   const QString & buttonKey,
   const QString &comboBoxKey
)
{  
   if (
         sMapButtons.contains(buttonKey)
      && sMapComboBoxes.contains(comboBoxKey)
   )
   {
      MenuPageButtonItem* button = sMapButtons[buttonKey];
      MenuPageComboBoxItem* comboBox = sMapComboBoxes[comboBoxKey];

      if (!comboBox->getButtonItem())
      {
         comboBox->setButtonItem(button);

         qApp->connect(
            button,
            SIGNAL(action(QString)),
            comboBox,
            SLOT(setVisible())
         );
      }
   }
}


void MenuPageComboBoxItem::linkComboBoxToLabel(
   const QString& labelKey,
   const QString& comboBoxKey
)
{
   if (
         sMapLabels.contains(labelKey)
      && sMapComboBoxes.contains(comboBoxKey)
   )
   {
      MenuPageLabelItem* label = sMapLabels[labelKey];
      MenuPageComboBoxItem* comboBox = sMapComboBoxes[comboBoxKey];
      comboBox->setLabelItem(label);

      qApp->connect(
         comboBox,
         SIGNAL(valueChanged(QString)),
         label,
         SLOT(setText(QString))
      );
   }
}


void MenuPageComboBoxItem::setButtonItem(MenuPageButtonItem *item)
{
   mButtonItem = item;
}


MenuPageButtonItem *MenuPageComboBoxItem::getButtonItem() const
{
   return mButtonItem;
}


MenuPageButtonItem *MenuPageComboBoxItem::getButtonItem(const QString &name)
{
   MenuPageButtonItem* button = 0;

   QMap<QString, MenuPageButtonItem*>::iterator it;

   it = sMapButtons.find(name);
   if (it != sMapButtons.end())
   {
      button = it.value();
   }

   return button;
}


void MenuPageComboBoxItem::setLabelItem(MenuPageLabelItem *item)
{
   mLabelItem = item;
}


MenuPageLabelItem *MenuPageComboBoxItem::getLabelItem() const
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
            emit valueChanged(value);
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


void MenuPageComboBoxItem::setValue(const QString & value)
{
   if (getLabelItem())
   {
      getLabelItem()->setText(value);
   }
}
