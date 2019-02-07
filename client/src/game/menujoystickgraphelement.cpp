#include "menujoystickgraphelement.h"


MenuJoystickGraphElement::MenuJoystickGraphElement()
 : mNorth(0),
   mSouth(0),
   mEast(0),
   mWest(0),
   mItem(0),
   mNorthItem(0),
   mSouthItem(0),
   mEastItem(0),
   mWestItem(0)
{
}


MenuJoystickGraphElement *MenuJoystickGraphElement::getNorth() const
{
   return mNorth;
}


void MenuJoystickGraphElement::setNorth(MenuJoystickGraphElement *value)
{
   mNorth = value;
}


MenuJoystickGraphElement *MenuJoystickGraphElement::getSouth() const
{
   return mSouth;
}


void MenuJoystickGraphElement::setSouth(MenuJoystickGraphElement *value)
{
   mSouth = value;
}


MenuJoystickGraphElement *MenuJoystickGraphElement::getWest() const
{
   return mWest;
}


void MenuJoystickGraphElement::setWest(MenuJoystickGraphElement *value)
{
   mWest = value;
}


MenuPageItem *MenuJoystickGraphElement::getItem() const
{
   return mItem;
}


void MenuJoystickGraphElement::setItem(MenuPageItem *value)
{
   mItem = value;
}


MenuPageItem *MenuJoystickGraphElement::getNorthItem() const
{
   return mNorthItem;
}


void MenuJoystickGraphElement::setNorthItem(MenuPageItem *value)
{
   mNorthItem = value;
}


MenuPageItem *MenuJoystickGraphElement::getSouthItem() const
{
   return mSouthItem;
}


void MenuJoystickGraphElement::setSouthItem(MenuPageItem *value)
{
   mSouthItem = value;
}


MenuPageItem *MenuJoystickGraphElement::getEastItem() const
{
   return mEastItem;
}


void MenuJoystickGraphElement::setEastItem(MenuPageItem *value)
{
   mEastItem = value;
}


MenuPageItem *MenuJoystickGraphElement::getWestItem() const
{
   return mWestItem;
}


void MenuJoystickGraphElement::setWestItem(MenuPageItem *value)
{
   mWestItem = value;
}


MenuJoystickGraphElement *MenuJoystickGraphElement::getEast() const
{
   return mEast;
}


void MenuJoystickGraphElement::setEast(MenuJoystickGraphElement *value)
{
   mEast = value;
}




