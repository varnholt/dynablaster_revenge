#include "menupageradiobuttonitem.h"

MenuPageRadioButtonItem::MenuPageRadioButtonItem(QObject* parent)
   : MenuPageCheckBoxItem(parent)
{
   mPageItemType = PageItemTypeRadioButton;
}

void MenuPageRadioButtonItem::toggleChecked()
{
   // only check if not previously checked
   // unchecking is only done by the radio-button group
   if (!isChecked())
      setChecked(true);
}

