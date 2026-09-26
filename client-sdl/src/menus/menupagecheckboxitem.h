#pragma once

#include "menupageitem.h"
#include "signal.h"

class MenuPageCheckBoxItem : public MenuPageItem
{
public:
   MenuPageCheckBoxItem();

   virtual void draw();

   virtual void setCheckedLayer(PSDLayer* layer);

   virtual void setUncheckedLayer(PSDLayer* layer);

   virtual PSDLayer* getCheckedLayer() const;

   virtual PSDLayer* getUncheckedLayer() const;

   virtual PSDLayer* getLayer() const;

   bool isChecked() const;

   void setChecked(bool checked);

   virtual void activated();

   virtual void deactivated();

   Signal<> stateChangedSignal;

protected:
   virtual void toggleChecked();

   PSDLayer* mLayerChecked;

   PSDLayer* mLayerUnchecked;

   bool mChecked;
};
