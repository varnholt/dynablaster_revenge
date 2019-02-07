#pragma once

// base
#include "menupagecomboboxitem.h"

// menus
#include "menupagetextedit.h"

class MenuPageEditableComboBoxItem : public MenuPageComboBoxItem
{

public:

   //! constructor
   MenuPageEditableComboBoxItem();

   //! initialize item
   virtual void initialize();

   //! setter for visible flag
   void setVisible(bool visible);

   //! setter for textedit item
   void setTextEditItem(MenuPageTextEditItem *item);

   //! getter for textedit item
   MenuPageTextEditItem *getTextEditItem() const;

   //! link combobox to related textedit
   static void linkComboBoxToTextEdit(
      const QString& textEditKey,
      const QString& comboBoxKey
   );

   //! add label to id/ptr-map
   static void addTextEdit(const QString&, MenuPageTextEditItem*);

   //! append an item to the list
   void appendItem(
      const QString& item,
      const QColor& color = QColor("#FFFFFF"),
      bool overrideAlpha = false,
      const QColor& outlineColor = QColor()
   );


protected:

   //! update clipper boundaries
   void updateClipperBounds();

   //! linked textedit item
   MenuPageTextEditItem* mTextEditItem;

   //! map of labels by id
   static QMap<QString, MenuPageTextEditItem*> sMapTextEdits;
};


