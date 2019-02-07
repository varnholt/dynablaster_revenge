#ifndef MENUPAGECHECKBOXGROUP_H
#define MENUPAGECHECKBOXGROUP_H

#include <QObject>

class MenuPageCheckBoxItem;

class MenuPageCheckBoxGroup : public QObject
{
   Q_OBJECT

public:

   //! two different buttongroup modes
   enum Mode
   {
      ModeCheckBox,     // non-exclusive
      ModeRadioButton   // exclusive
   };

   //! constructor
   MenuPageCheckBoxGroup(QObject *parent = 0);

   //! add a checkbox to the group
   void add(MenuPageCheckBoxItem*);

   //! remove an item from the group
   void remove(MenuPageCheckBoxItem*);

   //! set buttongroup mode
   void setMode(Mode mode);

   //! getter for buttongroup mode
   Mode getMode() const;


signals:


public slots:


   //! a checkbox was activated
   virtual void stateChanged();


protected:

   //! checkboxes within this group
   QList<MenuPageCheckBoxItem*> mGroup;

   //! group mode
   Mode mMode;
    
};

#endif // MENUPAGECHECKBOXGROUP_H
