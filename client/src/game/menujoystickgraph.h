#ifndef MENUJOYSTICKGRAPH_H
#define MENUJOYSTICKGRAPH_H

// Qt
#include <QList>
#include <QObject>

// menus
#include "menupageitem.h"

// forward declarations
class CharCycling;
class MenuJoystickGraphElement;
class MenuPageComboBoxItem;
class MenuPageListItem;
class MenuPageSliderItem;
class MenuPageTextEditItem;


class MenuJoystickGraph : public QObject
{

Q_OBJECT

public:

   //! navigation direction
   enum NavigationDirection
   {
      NavigationDirectionNone,
      NavigationDirectionNorth,
      NavigationDirectionSouth,
      NavigationDirectionEast,
      NavigationDirectionWest
   };

   //! constructor
   MenuJoystickGraph();

   //! destructor
   virtual ~MenuJoystickGraph();

   //! button pressed
   void button();

   //! go direction
   void walk(NavigationDirection direction);

   //! getter for current element
   MenuJoystickGraphElement *getFocussedElement() const;

   //! add an element
   void add(MenuJoystickGraphElement* element);

   //! getter for all element
   const QList<MenuJoystickGraphElement*>& getElements() const;

   //! change focus
   void changeFocus(MenuPageItem* currentItem, MenuPageItem *nextItem);

   //! getter for default page item
   MenuPageItem *getDefaultPageItem() const;

   //! setter for default page item
   void setDefaultPageItem(MenuPageItem *value);

   //! getter for default element
   MenuJoystickGraphElement *getDefaultElement() const;

   //! setter for default element
   void setDefaultElement(MenuJoystickGraphElement *value);

   //! element internal movement
   MenuPageItem* internalNavigation(NavigationDirection direction) const;



protected slots:

   //! update combobox focus
   void updateComboboxFocus(bool visible);


protected:

   //! mouse press
   void mousePress();

   //! move the mouse (in psd coordinates)
   void mouseMove(int cx, int cy) const;

   //! get the mouse position in psd coords
   void getMousePosInPsdCoords(int& x, int&y ) const;

   //! button for combobox
   void buttonForComboBox(
      MenuJoystickGraphElement* current,
      MenuPageComboBoxItem* visibleCombobox
   );

   //! button for slider
   void buttonForSlider(MenuPageSliderItem* activatedSlider);

   //! button for list
   void buttonForLists(MenuJoystickGraphElement* current);

   //! button for textedit
   void buttonForTextEdit(MenuPageTextEditItem* textedit);

   //! autoadjust walk direction if no element has been found
   void autoAdjust();

   //! check if there is a modal item currently active that is privileged
   MenuPageItem* getModalItem() const;

   //! getter for visible combobox
   MenuPageComboBoxItem *getVisibleCombobox() const;

   //! check if a button relates to a combobox
   MenuPageComboBoxItem *getComboBoxForButton(MenuPageItem*) const;

   //! getter for active list
   MenuPageListItem* getActiveList() const;

   //! getter for active slider
   MenuPageSliderItem* getActiveSlider() const;

   //! getter for active textedit
   MenuPageTextEditItem* getEditingActiveTextEdit() const;

   //! getter for active page item
   MenuPageItem* getActiveItem(MenuPageItem::PageItemType type) const;

   //! list of all elements for this graph
   QList<MenuJoystickGraphElement*> mElements;

   //! default page item
   MenuPageItem* mDefaultPageItem;

   //! default graph element
   MenuJoystickGraphElement* mDefaultJoystickGraphElement;

   //! char cycling class
   CharCycling* mCharCycling;
};

#endif // MENUJOYSTICKGRAPH_H
