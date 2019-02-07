// header
#include "menupagecheckboxgroup.h"

// menus
#include "menupagecheckboxitem.h"


MenuPageCheckBoxGroup::MenuPageCheckBoxGroup(QObject *parent) :
    QObject(parent),
    mMode(ModeCheckBox)
{
}


void MenuPageCheckBoxGroup::add(MenuPageCheckBoxItem* item)
{
   mGroup.push_back(item);

   connect(
      item,
      SIGNAL(stateChanged()),
      this,
      SLOT(stateChanged())
   );
}


void MenuPageCheckBoxGroup::remove(MenuPageCheckBoxItem* item)
{
   mGroup.removeAll(item);

   disconnect(
      item,
      SIGNAL(stateChanged()),
      this,
      SLOT(stateChanged())
   );
}


void MenuPageCheckBoxGroup::setMode(MenuPageCheckBoxGroup::Mode mode)
{
   mMode = mode;
}


MenuPageCheckBoxGroup::Mode MenuPageCheckBoxGroup::getMode() const
{
   return mMode;
}


void MenuPageCheckBoxGroup::stateChanged()
{
   blockSignals(true);

   MenuPageCheckBoxItem* currentItem =
      dynamic_cast<MenuPageCheckBoxItem*>(sender());

   if (currentItem)
   {
      // radiobutton mode
      //
      // if another checkbox/radiobutton was selected
      // => all others are de-selected
      //
      // if current is deselected and no other is currently selected
      // => block deselection
      if (mMode == ModeRadioButton)
      {
         if (currentItem->isChecked())
         {
            foreach (MenuPageCheckBoxItem* item, mGroup)
            {
               if (item != currentItem)
               {
                  item->setChecked(false);
               }
            }
         }
         else
         {
            bool allowed = false;

            foreach (MenuPageCheckBoxItem* item, mGroup)
            {
               if (item->isChecked() && item != currentItem)
               {
                  allowed = true;
                  break;
               }
            }

            // set back to checked if not allowed
            if (!allowed)
               currentItem->setChecked(true);
         }
      }
   }

   blockSignals(false);
}

