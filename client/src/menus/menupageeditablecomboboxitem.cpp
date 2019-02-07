// header
#include "menupageeditablecomboboxitem.h"

// menus
#include "clipper.h"
#include "menupagebuttonitem.h"

// Qt
#include <QApplication>

// static
QMap<QString, MenuPageTextEditItem*> MenuPageEditableComboBoxItem::sMapTextEdits;


//-----------------------------------------------------------------------------
/*!
*/
MenuPageEditableComboBoxItem::MenuPageEditableComboBoxItem()
 : mTextEditItem(0)
{
   mPageItemType = PageItemTypeEditableCombobox;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageEditableComboBoxItem::updateClipperBounds()
{
   // the same code could be used in the usual combobox item as well
   // void Clipper::setBounds(float left, float top, float right, float bottom)
   mClipper->setBounds(
      mLayerActive->getLeft(),
      mLayerActive->getTop(),
      mLayerActive->getLeft() + mLayerActive->getWidth() - 3,
      mLayerActive->getTop() + (mLayerActive->getHeight() * getElementCount())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuPageEditableComboBoxItem::initialize()
{
   MenuPageComboBoxItem::initialize();
}


//-----------------------------------------------------------------------------
/*!
   \param item item to set
*/
void MenuPageEditableComboBoxItem::setTextEditItem(MenuPageTextEditItem *item)
{
   mTextEditItem = item;
}


//-----------------------------------------------------------------------------
/*!
   \return textedit item
*/
MenuPageTextEditItem* MenuPageEditableComboBoxItem::getTextEditItem() const
{
   return mTextEditItem;
}


//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void MenuPageEditableComboBoxItem::setVisible(bool visible)
{
   if (visible != isVisible())
   {
      // change button visibility
      MenuPageButtonItem* button = getButtonItem();

      if (button)
         button->setVisible(!visible);

      // change textedit visibility
      MenuPageTextEditItem* textEdit = getTextEditItem();

      if (textEdit)
         textEdit->setVisible(!visible);

      MenuPageListItem::setVisible(visible);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param key item key
   \param item textedit item to add
*/
void MenuPageEditableComboBoxItem::addTextEdit(
   const QString & key,
   MenuPageTextEditItem * item
)
{
   sMapTextEdits.insert(key, item);
}


//-----------------------------------------------------------------------------
/*!
   \param item text to add
   \param color item's color
   \param overrideAlpha \c true if alpha is overriden
   \param outlineColor item's outline color
*/
void MenuPageEditableComboBoxItem::appendItem(
   const QString &item,
   const QColor &color,
   bool overrideAlpha,
   const QColor &outlineColor
)
{
   MenuPageListItem::appendItem(item, color, overrideAlpha, outlineColor);
   updateClipperBounds();
}


//-----------------------------------------------------------------------------
/*!
   \param labelKey textedit item key
   \param comboBoxKey combobox item key
*/
void MenuPageEditableComboBoxItem::linkComboBoxToTextEdit(
   const QString& textEditKey,
   const QString& comboBoxKey
)
{
   if (
         sMapTextEdits.contains(textEditKey)
      && sMapComboBoxes.contains(comboBoxKey)
   )
   {
      MenuPageTextEditItem* textEdit = sMapTextEdits[textEditKey];
      MenuPageComboBoxItem* comboBox = sMapComboBoxes[comboBoxKey];
      MenuPageEditableComboBoxItem* editableCombobBox
         = dynamic_cast<MenuPageEditableComboBoxItem*>(comboBox);

      editableCombobBox->setTextEditItem(textEdit);

      qApp->connect(
         editableCombobBox,
         SIGNAL(valueChanged(QString)),
         textEdit,
         SLOT(setText(QString))
      );
   }
}

