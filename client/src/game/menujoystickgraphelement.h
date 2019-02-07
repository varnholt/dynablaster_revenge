#ifndef MENUJOYSTICKGRAPHELEMENT_H
#define MENUJOYSTICKGRAPHELEMENT_H

// Qt
#include <QString>

// forward declarations
class MenuPageItem;

class MenuJoystickGraphElement
{

public:

   //! constructor
   MenuJoystickGraphElement();

   //! getter for north element
   MenuJoystickGraphElement *getNorth() const;

   //! setter for north element
   void setNorth(MenuJoystickGraphElement *value);

   //! getter for south element
   MenuJoystickGraphElement *getSouth() const;

   //! setter for south element
   void setSouth(MenuJoystickGraphElement *value);

   //! getter for east element
   MenuJoystickGraphElement *getEast() const;

   //! setter for east element
   void setEast(MenuJoystickGraphElement *value);

   //! getter for west element
   MenuJoystickGraphElement *getWest() const;

   //! setter for west element
   void setWest(MenuJoystickGraphElement *value);

   //! getter for item
   MenuPageItem *getItem() const;

   //! setter for item
   void setItem(MenuPageItem *value);

   //! getter for north item
   MenuPageItem *getNorthItem() const;

   //! setter for north item
   void setNorthItem(MenuPageItem *value);

   //! getter for south item
   MenuPageItem *getSouthItem() const;

   //! setter for south item
   void setSouthItem(MenuPageItem *value);

   //! getter for east item
   MenuPageItem *getEastItem() const;

   //! setter for east item
   void setEastItem(MenuPageItem *value);

   //! getter for west item
   MenuPageItem *getWestItem() const;

   //! setter for west item
   void setWestItem(MenuPageItem *value);


protected:

   //! north
   MenuJoystickGraphElement* mNorth;

   //! south
   MenuJoystickGraphElement* mSouth;

   //! east
   MenuJoystickGraphElement* mEast;

   //! west
   MenuJoystickGraphElement* mWest;

   //! element name
   QString mName;

   //! item
   MenuPageItem* mItem;

   //! north menupage item
   MenuPageItem* mNorthItem;

   //! south menupage item
   MenuPageItem* mSouthItem;

   //! east menupage item
   MenuPageItem* mEastItem;

   //! west menupage item
   MenuPageItem* mWestItem;
};

#endif // MENUJOYSTICKGRAPHELEMENT_H

