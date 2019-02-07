#include "menupagecheckboxitem.h"

MenuPageCheckBoxItem::MenuPageCheckBoxItem(QObject* parent)
   : MenuPageItem(parent),
     mLayerChecked(0),
     mLayerUnchecked(0),
     mChecked(false)
{
   mPageItemType = PageItemTypeCheckbox;
   mInteractive = true;
}


void MenuPageCheckBoxItem::setCheckedLayer(PSDLayer* layer)
{
   mLayerChecked = layer;
}


void MenuPageCheckBoxItem::setUncheckedLayer(PSDLayer* layer)
{
   mLayerUnchecked = layer;
}


PSDLayer *MenuPageCheckBoxItem::getCheckedLayer() const
{
   return mLayerChecked;
}


PSDLayer *MenuPageCheckBoxItem::getUncheckedLayer() const
{
   return mLayerUnchecked;
}


PSDLayer* MenuPageCheckBoxItem::getLayer() const
{
   if (isChecked())
      return mLayerChecked;
   else
      return mLayerUnchecked;
}


bool MenuPageCheckBoxItem::isChecked() const
{
   return mChecked;
}


void MenuPageCheckBoxItem::setChecked(bool checked)
{
   mChecked = checked;

   // notify others
   emit stateChanged();
}


void MenuPageCheckBoxItem::toggleChecked()
{
   setChecked(!isChecked());
}


void MenuPageCheckBoxItem::draw()
{
   // call base
   MenuPageItem::draw();
}


void MenuPageCheckBoxItem::activated()
{
   toggleChecked();
   MenuPageItem::activated();
}


void MenuPageCheckBoxItem::deactivated()
{
   MenuPageItem::deactivated();
}


