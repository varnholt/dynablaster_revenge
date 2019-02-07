#ifndef MENUPAGERADIOBUTTONITEM_H
#define MENUPAGERADIOBUTTONITEM_H

#include "menupagecheckboxitem.h"

class MenuPageRadioButtonItem : public MenuPageCheckBoxItem
{

Q_OBJECT

public:

   MenuPageRadioButtonItem(QObject* parent = 0);


protected:

   virtual void toggleChecked();



};

#endif // MENUPAGERADIOBUTTONITEM_H
