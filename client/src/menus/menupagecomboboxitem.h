#ifndef MENUPAGECOMBOBOXITEM_H
#define MENUPAGECOMBOBOXITEM_H

// base
#include "menupagelistitem.h"

// Qt
#include <QMap>

// forward declarations
class MenuPageButtonItem;
class MenuPageLabelItem;

class MenuPageComboBoxItem : public MenuPageListItem
{
   Q_OBJECT

   public:

      MenuPageComboBoxItem();

      /*

         combobox_time_label              // finally displays value

         combobox_time_button_active      // clicked first
         combobox_time_button_inactive    //

         combobox_time_table_bg_first     // activated after button was clicked
         combobox_time_table_bg_last      //
         combobox_time_table_bg_default   //
         combobox_time_table_gradient     //


         concept

         ----------------------
         | dropdown_button \/ |     < standard menupagebuttonitem
         ======================
  /|\    | item 1 (first)     |     | menupagelistitem  | menupagelistitemelement 1
   |     +--------------------+     |                   |
   |     | item 2             |     |                   | menupagelistitemelement 2
 height  +--------------------+     |                   |
   |     | item ...           |     |                   | menupagelistitemelement 3
   |     +--------------------+     |                   |
  \|/    | item n (last)      |     |                   | menupagelistitemelement 4 (last)
         ----------------------
         \--------------------/     < additional quad to terminate the selection (?)


      */

      //! extend base
      virtual void draw();

      //! extend base
      virtual void animate(float time);

      //! add combobox to id/ptr-map
      static void addComboBox(const QString&, MenuPageComboBoxItem*);

      //! add button to id/ptr-map
      static void addButton(const QString&, MenuPageButtonItem*);

      //! add label to id/ptr-map
      static void addLabel(const QString&, MenuPageLabelItem*);

      //! link combobox to related button
      static void linkComboBoxToButton(
         const QString& buttonKey,
         const QString& comboBoxKey
      );

      //! link combobox to related label
      static void linkComboBoxToLabel(
         const QString& labelKey,
         const QString& comboBoxKey
      );

      //! extend base
      virtual void initialize();

      //! extend base
      virtual void setFocus(bool);

      //! getter for modal flag
      virtual bool isModal() const;

      //! setter for button item
      virtual void setButtonItem(MenuPageButtonItem* item);

      //! getter for button item
      MenuPageButtonItem* getButtonItem() const;

      //! getter for button item
      static MenuPageButtonItem* getButtonItem(const QString& name);

      //! setter for label item
      virtual void setLabelItem(MenuPageLabelItem* item);

      //! getter for label item
      MenuPageLabelItem* getLabelItem() const;


      //! extend base
      virtual void mousePressed(int /*x*/, int y);

      //! getter for value
      QString getValue() const;

      //! setter for value
      void setValue(const QString&);


   public slots:

      //! called when button pressed, item selected
      virtual void setVisible(bool visible = true);

      //! dropdown was enabled/disabled
      virtual void dropDownEnabled(bool enabled);


   signals:

      //! new value is passed to dropdown label on select
      void updateDropDown(const QString&);

      //! value changed
      void valueChanged(const QString&);


   protected:

      //! draws single quad
      virtual void drawQuad(PSDLayer* layer, float x, float y, float width, float height, int opacity = 255);

      //! get notified on table bound changes, resize layer
      virtual void updateTableBounds();

      //! make combobox visible
      float mVisibleAnimationTime;

      //! make combobox invisible again
      float mInvisibleAnimationTime;

      //! linked button item
      MenuPageButtonItem* mButtonItem;

      //! linked label item
      MenuPageLabelItem* mLabelItem;

      //! map of comboboxes by id
      static QMap<QString, MenuPageComboBoxItem*> sMapComboBoxes;

      //! map of buttons by id
      static QMap<QString, MenuPageButtonItem*> sMapButtons;

      //! map of labels by id
      static QMap<QString, MenuPageLabelItem*> sMapLabels;
};

#endif // MENUPAGECOMBOBOXITEM_H
