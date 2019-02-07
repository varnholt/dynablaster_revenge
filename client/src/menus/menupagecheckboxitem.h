#ifndef MENUPAGECHECKBOXITEM_H
#define MENUPAGECHECKBOXITEM_H

#include "menupageitem.h"

class MenuPageCheckBoxItem : public MenuPageItem
{
   Q_OBJECT

   public:

      MenuPageCheckBoxItem(QObject *parent = 0);

      virtual void draw();

      virtual void setCheckedLayer(PSDLayer* layer);

      virtual void setUncheckedLayer(PSDLayer* layer);

      virtual PSDLayer* getCheckedLayer() const;

      virtual PSDLayer* getUncheckedLayer() const;

      virtual PSDLayer* getLayer() const;

      bool isChecked() const;

      void setChecked(bool checked);


   public slots:

      virtual void activated();

      virtual void deactivated();


   signals:

      void stateChanged();


   protected:

      virtual void toggleChecked();

      PSDLayer* mLayerChecked;

      PSDLayer* mLayerUnchecked;

      bool mChecked;

};

#endif // MENUPAGECHECKBOXITEM_H
