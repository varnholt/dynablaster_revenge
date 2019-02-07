// header
#include "menujoystickgraph.h"

// game
#include "charcycling.h"
#include "menujoystickgraphelement.h"
#include "menujoystickmouseanimation.h"

// menus
#include "menu.h"
#include "menupage.h"
#include "menupageitem.h"
#include "menupagebuttonitem.h"
#include "menupagecheckboxitem.h"
#include "menupagecomboboxitem.h"
#include "menupageeditablecomboboxitem.h"
#include "menupagelistitem.h"
#include "menupagelistitemelement.h"
#include "menupageslideritem.h"

// framework
#include "gldevice.h"

// Qt
#include <QApplication>
#include <QCursor>
#include <QMouseEvent>
#include <QWidget>

// defines
#define GOLDEN_RATIO 0.6180339887f


/*
   TODO:

      - TABLES
      - MAKE LINEEDIT CHARCYCLING WORK FOR THE LAST CHAR IF CHAR EXCEEDS LINEEDIT
*/


//-----------------------------------------------------------------------------
/*!
*/
MenuJoystickGraph::MenuJoystickGraph()
 : mDefaultPageItem(0),
   mDefaultJoystickGraphElement(0),
   mCharCycling(0)
{
   mCharCycling = new CharCycling();
}


//-----------------------------------------------------------------------------
/*!
*/
MenuJoystickGraph::~MenuJoystickGraph()
{
   delete mCharCycling;
}


//-----------------------------------------------------------------------------
/*!
   \param cx x position
   \param cy y position
*/
void MenuJoystickGraph::mouseMove(int cx, int cy) const
{
   QWidget* activeWindow = qApp->activeWindow();

   if (activeWindow)
   {
      QPoint topLeft = activeWindow->mapToGlobal(QPoint(0,0));

      // retrieve viewport information
      int vpx = 0;
      int vpy = 0;
      int vpWidth = 0;
      int vpHeight = 0;
      activeDevice->getViewPort(&vpx, &vpy, &vpWidth, &vpHeight);

      // menus are done in hd
      float w = vpWidth / 1920.0f;
      float h = vpHeight / 1080.0f;

      // adapt layer position to screen coordinates
      int x = topLeft.x() + vpx + w * cx;
      int y = topLeft.y() + vpy + h * cy;

      // if a negative number is given for cx or cy, just use
      // the current cursor position
      QPoint current = QCursor::pos();
      if (cx < 0)
         x = current.x();
      if (cy < 0)
         y = current.y();

      // update cursor
      // QCursor::setPos(x, y);
      MenuJoystickMouseAnimation::add(QPoint(x,y));
   }
}


//-----------------------------------------------------------------------------
/*!
   \param current current joystick graph element
   \param next next joystick graph element
*/
void MenuJoystickGraph::changeFocus(
   MenuPageItem* currentItem,
   MenuPageItem* nextItem
)
{
   if (nextItem)
   {
      // mouse cursor movement
      PSDLayer* layer = 0;

      // layer information
      int top    = 0;
      int bottom = 0;
      int left   = 0;
      int right  = 0;

      switch (nextItem->getPageItemType())
      {
         case MenuPageItem::PageItemTypeList:
         {
            if (nextItem->isActive())
            {
               // there's no need for any further slider handling
               // there's no need for any further textedit handling
               return;
            }
            else
            {
               layer = nextItem->getActiveLayer();
            }

            break;
         }

         case MenuPageItem::PageItemTypeCheckbox:
         {
            MenuPageCheckBoxItem* cb = dynamic_cast<MenuPageCheckBoxItem*>(nextItem);
            layer = cb->getCheckedLayer();
            break;
         }

         case MenuPageItem::PageItemTypeListElement:
         {
            /*
                    base top left
                   /
                   +------------------------------+--+
                   |                              |\/|
                   +------------------------------+--+
                   |                              |
                   +------------------------------+
                   |                              |
                   +------------------------------+
                   |                              |
                   +------------------------------+

            */

            MenuPageListItemElement* elem = dynamic_cast<MenuPageListItemElement*>(nextItem);
            MenuPageItem* parent = elem->getParent();

            int baseTop = 0;
            int baseLeft = 0;

            // for comboboxes, the button on "currenItem" is the whole range behind the lineedit
            // => so i have to check if the button belongs to a editablecombobox and use the layer
            //    of the lineedit instead.
            if (
                  parent
               && parent->getPageItemType() == MenuPageItem::PageItemTypeEditableCombobox
            )
            {
               MenuPageEditableComboBoxItem* combo =
                  dynamic_cast<MenuPageEditableComboBoxItem*>(parent);

               baseTop = combo->getTextEditItem()->getActiveLayer()->getTop();
               baseLeft = combo->getTextEditItem()->getActiveLayer()->getLeft();
            }
            else
            {
               baseTop = currentItem->getActiveLayer()->getTop();
               baseLeft = currentItem->getActiveLayer()->getLeft();
            }

            top    = baseTop + elem->getY();
            bottom = baseTop + elem->getY() + elem->getHeight();
            left   = baseLeft + elem->getX();
            right  = baseLeft + elem->getX() + elem->getWidth();

            /*
            qDebug(
               "baseTop: %d, baseLeft: %d, top: %d, bottom: %d, left: %d, right: %d",
               baseTop,
               baseLeft,
               top,
               bottom,
               left,
               right
            );
            */

            break;
         }

         case MenuPageItem::PageItemTypeSlider:
         case MenuPageItem::PageItemTypeTextedit:
         {
            if (nextItem->isActive())
            {
               // there's no need for any further slider handling
               // there's no need for any further textedit handling
               return;
            }
            else
            {
               layer = nextItem->getActiveLayer();
            }
            break;
         }

         default:
         {
            layer = nextItem->getActiveLayer();
            break;
         }
      }

      if (layer && layer->getLayer())
      {
         top    = layer->getTop();
         bottom = layer->getBottom();
         left   = layer->getLeft();
         right  = layer->getRight();

         qDebug(
            "MenuJoystickGraph::changeFocus: %s"
            "(top: %d, left: %d, bottom: %d, right: %d)",
            layer->getLayer()->getName(),
            top,
            left,
            bottom,
            right
         );
      }

      int cx = left + ( abs(left - right) * GOLDEN_RATIO);
      int cy = top  + ( abs(top - bottom) * GOLDEN_RATIO);

      // set mouse cursor
      mouseMove(cx, cy);
   }
}


//-----------------------------------------------------------------------------
/*!
   \return default page item
*/
MenuPageItem *MenuJoystickGraph::getDefaultPageItem() const
{
   return mDefaultPageItem;
}


//-----------------------------------------------------------------------------
/*!
   \param value default page item
*/
void MenuJoystickGraph::setDefaultPageItem(MenuPageItem *value)
{
   mDefaultPageItem = value;
}


//-----------------------------------------------------------------------------
/*!
   \return default element
*/
MenuJoystickGraphElement *MenuJoystickGraph::getDefaultElement() const
{
   return mDefaultJoystickGraphElement;
}


//-----------------------------------------------------------------------------
/*!
   \param value default element
*/
void MenuJoystickGraph::setDefaultElement(MenuJoystickGraphElement *value)
{
   mDefaultJoystickGraphElement = value;
}


//-----------------------------------------------------------------------------
/*!
   \param direction direction to try internal navigation on
*/
MenuPageItem* MenuJoystickGraph::internalNavigation(
   NavigationDirection direction
) const
{
   MenuPageItem* internal = 0;
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*>* items = page->getPageItems();

   foreach (MenuPageItem* item, *items)
   {
      MenuPageItem::PageItemType type = item->getPageItemType();

      // check if there's a visible combobox item on the screen
      if (
            type == MenuPageItem::PageItemTypeCombobox
         || type == MenuPageItem::PageItemTypeEditableCombobox
      )
      {
         if (item->isVisible())
         {
            MenuPageComboBoxItem* combo =
               dynamic_cast<MenuPageComboBoxItem*>(item);

            if (combo)
            {
               int focussed = combo->getFocussedElement();
               int next = focussed;

               switch (direction)
               {
                  case NavigationDirectionNorth:
                     next = qMax(0, next - 1);
                     break;

                  case NavigationDirectionSouth:
                     next = qMin(combo->getElementCount() - 1, next + 1);
                     break;

                  default:
                     break;
               }

               qDebug(
                  "MenuJoystickGraph::internalNavigation: focussed item: %d, next: %d",
                  focussed,
                  next
               );

               internal = combo->getElementAt(next);
               break;
            }
         }
      }

      // list navigation
      else if (type == MenuPageItem::PageItemTypeList)
      {
         if (item->isActive())
         {
            internal = item;

            MenuPageListItem* list = dynamic_cast<MenuPageListItem*>(item);

            int focussed = list->getFocussedElement();
            bool move = false;

            switch (direction)
            {
               case NavigationDirectionNorth:
                  move = true;
                  focussed--;
                  break;

               case NavigationDirectionSouth:
                  move = true;
                  focussed++;
                  break;

               case NavigationDirectionEast:
                  move = true;
                  focussed = list->getElementCount() - 1;
                  break;

               case NavigationDirectionWest:
                  move = true;
                  focussed = 0;
                  break;

               default:
                  break;
            }

            if (move)
            {
               if (
                     focussed > -1
                  && focussed < list->getElementCount()
               )
               {
                  int mouseY = list->scrollSmoothToIndex(focussed);
                  int tableY = 0;
                  int offsetY = 20;
                  int posY = tableY + mouseY + offsetY;
                  mouseMove(-1, posY);
               }
            }
         }
      }

      // check if there is an active slider
      else if (type == MenuPageItem::PageItemTypeSlider)
      {
         if (item->isActive())
         {
            MenuPageSliderItem* slider = dynamic_cast<MenuPageSliderItem*>(item);

            int mX = 0;
            int mY = 0;
            getMousePosInPsdCoords(mX, mY);

            int step = 40;
            int fineStep = 10;

            switch (direction)
            {
               // large steps
               case NavigationDirectionEast:
                  mX += step;
                  mX = qMin(mX, slider->getMaximum());
                  break;

               case NavigationDirectionWest:
                  mX -= step;
                  mX = qMax(mX, slider->getMinimum());
                  break;

               // fine tuning
               case NavigationDirectionNorth:
                  mX += fineStep;
                  mX = qMin(mX, slider->getMaximum());
                  break;

               case NavigationDirectionSouth:
                  mX -= fineStep;
                  mX = qMax(mX, slider->getMinimum());
                  break;

               default:
                  break;
            }

            // qDebug("move mouse to: %d", mX);
            mouseMove(mX, -1);

            internal = item;
            break;
         }
      }
      else if (type == MenuPageItem::PageItemTypeTextedit)
      {
         MenuPageTextEditItem* textEdit =
            dynamic_cast<MenuPageTextEditItem*>(item);

         if (textEdit->isEditingActive())
         {
            // init
            bool updateTextEdit = false;
            QString text = textEdit->getText();
            int cursorPosition = textEdit->getCursorPosition();

            QChar currentChar;
            if (cursorPosition > text.length() - 1)
               currentChar = ' ';
            else
               currentChar = text.at(cursorPosition);

            switch (direction)
            {
               case NavigationDirectionEast:
                  if (textEdit->getText().length() < textEdit->getMaxLength() - 1)
                  {
                     textEdit->keyPressed(Qt::Key_Right, "");
                  }
                  break;

               case NavigationDirectionWest:
                  textEdit->keyPressed(Qt::Key_Right, "");
                  textEdit->keyPressed(Qt::Key_Backspace, ""); // Key_Left
                  break;

               case NavigationDirectionNorth:
                  if (textEdit->getText().length() < textEdit->getMaxLength())
                  {
                     mCharCycling->setChar(currentChar);
                     mCharCycling->up();
                     updateTextEdit = true;
                  }
                  break;

               case NavigationDirectionSouth:
                  if (textEdit->getText().length() < textEdit->getMaxLength())
                  {
                     mCharCycling->setChar(currentChar);
                     mCharCycling->down();
                     updateTextEdit = true;
                  }
                  break;

               default:
                  break;
            }

            if (updateTextEdit)
            {
               QString altered = mCharCycling->modify(
                  text,
                  textEdit->getCursorPosition()
               );

               textEdit->setText(altered);
               textEdit->setCursorPosition(cursorPosition);
            }

            internal = item;
         }
      }
   }

   qDebug("MenuJoystickGraph::internalNavigation: %s", internal ? "yup" : "nuh");

   return internal;
}


//-----------------------------------------------------------------------------
/*!
   \param index focussed index
*/
void MenuJoystickGraph::updateComboboxFocus(bool visible)
{
   MenuPageComboBoxItem* combobox = 0;

   combobox = dynamic_cast<MenuPageComboBoxItem*>(sender());

   disconnect(
      combobox,
      SIGNAL(visible(bool)),
      this,
      SLOT(updateComboboxFocus(bool))
   );

   if (combobox && visible)
   {
      // once the combobox default value has been set, adjust the mouse
      // cursor position depending on the focussed element in the combobox
      MenuJoystickGraphElement* current = getFocussedElement();

      if (current)
      {
         int index = 0;
         for (int i = 0; i < combobox->getElementCount(); i++)
         {
            if (combobox->getValue() == combobox->getElementAt(i)->getText())
               index = i;
         }

         MenuPageItem* focussedElement = combobox->getElementAt(index);

         if (focussedElement)
         {
            qDebug(
               "MenuJoystickGraph::updateComboboxFocus: focus index %d",
               index
            );

            changeFocus(
               current->getItem(),
               focussedElement
            );
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to modal item if available
*/
MenuPageItem *MenuJoystickGraph::getModalItem() const
{
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*>* items = page->getPageItems();
   MenuPageItem * pageItem = 0;

   foreach (MenuPageItem* item, *items)
   {
      MenuPageItem::PageItemType type = item->getPageItemType();

      // the represents an opened combobox
      if (
            type == MenuPageItem::PageItemTypeCombobox
         || type == MenuPageItem::PageItemTypeEditableCombobox
      )
      {
         if (item->isVisible())
         {
            MenuPageComboBoxItem* combo =
               dynamic_cast<MenuPageComboBoxItem*>(item);

            if (combo)
            {
               // we're actually interested in the button
               // since this one is used as parent element for the joystick navigation
               pageItem = combo->getButtonItem();
               break;
            }
         }
      }

      // this represents a clicked slider
      else if (type == MenuPageItem::PageItemTypeSlider)
      {
         if (item->isActive())
         {
            pageItem = item;
            break;
         }
      }
   }

   return pageItem;
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to modal item if available
*/
MenuPageComboBoxItem *MenuJoystickGraph::getVisibleCombobox() const
{
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*>* items = page->getPageItems();
   MenuPageComboBoxItem * pageItem = 0;

   foreach (MenuPageItem* item, *items)
   {
      MenuPageItem::PageItemType type = item->getPageItemType();

      if (
            type == MenuPageItem::PageItemTypeCombobox
         || type == MenuPageItem::PageItemTypeEditableCombobox
      )
      {
         if (item->isVisible())
         {
            pageItem = dynamic_cast<MenuPageComboBoxItem*>(item);
            break;
         }
      }
   }

   return pageItem;
}


//-----------------------------------------------------------------------------
/*!
   \param button pageitem
   \return combobox related to this button
*/
MenuPageComboBoxItem *MenuJoystickGraph::getComboBoxForButton(
   MenuPageItem * button
) const
{
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*>* items = page->getPageItems();
   MenuPageComboBoxItem * combobox = 0;

   foreach (MenuPageItem* item, *items)
   {
      MenuPageItem::PageItemType type = item->getPageItemType();
      if (
            type == MenuPageItem::PageItemTypeCombobox
         || type == MenuPageItem::PageItemTypeEditableCombobox
      )
      {
         MenuPageComboBoxItem* tmp =
            dynamic_cast<MenuPageComboBoxItem*>(item);

         if (tmp->getButtonItem() == button)
         {
            combobox = tmp;
         }
      }
   }

   return combobox;
}


//-----------------------------------------------------------------------------
/*!
   \return active slider item if exists
*/
MenuPageSliderItem *MenuJoystickGraph::getActiveSlider() const
{
   MenuPageItem* item = getActiveItem(MenuPageItem::PageItemTypeSlider);
   MenuPageSliderItem* slider = dynamic_cast<MenuPageSliderItem*>(item);
   return slider;
}


//-----------------------------------------------------------------------------
/*!
   \return active slider item if exists
*/
MenuPageTextEditItem *MenuJoystickGraph::getEditingActiveTextEdit() const
{
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*>* items = page->getPageItems();
   MenuPageTextEditItem * pageItem = 0;

   foreach (MenuPageItem* item, *items)
   {
      if (item->getPageItemType() == MenuPageItem::PageItemTypeTextedit)
      {
         MenuPageTextEditItem* tmp = dynamic_cast<MenuPageTextEditItem*>(item);

         if (tmp->isEditingActive())
         {
            pageItem = tmp;
            break;
         }
      }
   }

   return pageItem;
}


//-----------------------------------------------------------------------------
/*!
   \param type pageitem type
   \return ptr to page item
*/
MenuPageItem *MenuJoystickGraph::getActiveItem(MenuPageItem::PageItemType type) const
{
   MenuPageItem* item = 0;
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*>* items = page->getPageItems();

   foreach (MenuPageItem* tmp, *items)
   {
      if (tmp->getPageItemType() == type)
      {
         if (tmp->isActive())
         {
            item = tmp;
            break;
         }
      }
   }

   return item;
}


//-----------------------------------------------------------------------------
/*!
   \return active list item if exists
*/
MenuPageListItem *MenuJoystickGraph::getActiveList() const
{
   MenuPageItem* item = getActiveItem(MenuPageItem::PageItemTypeList);
   MenuPageListItem* list = dynamic_cast<MenuPageListItem*>(item);
   return list;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickGraph::autoAdjust()
{
   // if we did not find an element here
   // => pick the closest element when the mouse is moved
   //    by iterating through all elements in the current graph and looking
   //    for the one with the smallest manhattan distance
   QPoint storedMinumPos = QPoint(9999, 9999);
   MenuJoystickGraphElement* element = 0;
   QWidget* activeWindow = qApp->activeWindow();
   int mousePsdX = 0;
   int mousePsdY = 0;

   if (activeWindow)
   {
      getMousePosInPsdCoords(mousePsdX, mousePsdY);

      QPoint mousePos = QPoint(mousePsdX, mousePsdY);

      foreach (MenuJoystickGraphElement* tmpElement, mElements)
      {
         MenuPageItem* tmpItem = tmpElement->getItem();
         PSD::Layer* layer = tmpItem->getCurrentLayer();
         QPoint currentItemPos = QPoint(
               (layer->getLeft() + layer->getLeft() + layer->getWidth()) * 0.5f,
               (layer->getTop() + layer->getTop() + layer->getHeight()) * 0.5f
            );

         int distToItem = qAbs((currentItemPos - mousePos).manhattanLength());
         int distMinimum = qAbs((storedMinumPos - mousePos).manhattanLength());

         if (distToItem < distMinimum)
         {
            storedMinumPos = currentItemPos;
            element = tmpElement;
         }
      }

      if (element)
      {
         changeFocus(
            0,
            element->getItem()
         );
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param direction direction to walk
*/
void MenuJoystickGraph::walk(NavigationDirection direction)
{
   MenuJoystickGraphElement* current = getFocussedElement();

   if (current)
   {
      MenuJoystickGraphElement* next = 0;

      switch (direction)
      {
         case NavigationDirectionNorth:
            next = current->getNorth();
            break;
         case NavigationDirectionSouth:
            next = current->getSouth();
            break;
         case NavigationDirectionEast:
            next = current->getEast();
            break;
         case NavigationDirectionWest:
            next = current->getWest();
            break;
         default:
            break;
      }

      MenuPageItem* internal = internalNavigation(direction);

      changeFocus(
         current->getItem(),
         internal
            ? internal
            : (next
               ? next->getItem()
               : 0
            )
      );
   }
   else
   {
      autoAdjust();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickGraph::mousePress()
{
   QWidget* activeWindow = qApp->activeWindow();

   if (activeWindow)
   {
      QPoint cursor = QCursor::pos();
      QPoint relative = activeWindow->mapFromGlobal(cursor);

      QMouseEvent* mouseEvent = new QMouseEvent(
         QEvent::MouseButtonPress,
         relative,
         Qt::LeftButton,
         Qt::LeftButton,
         Qt::ShiftModifier
      );

      qDebug(
         "MenuJoystickGraph::button(): generate mouse event at: %d, %d",
         relative.x(),
         relative.y()
      );

      QCoreApplication::postEvent(activeWindow, mouseEvent);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param current current combobox
   \param visibleCombobox combobox that is currently visible
*/
void MenuJoystickGraph::buttonForComboBox(
   MenuJoystickGraphElement* current,
   MenuPageComboBoxItem* visibleCombobox
)
{
   if (current)
   {
      // if the combobox was just opened, move the cursor to the focussed element
      MenuPageComboBoxItem* openedCombo = getComboBoxForButton(current->getItem());

      if (openedCombo)
      {
         connect(
            openedCombo,
            SIGNAL(visible(bool)),
            this,
            SLOT(updateComboboxFocus(bool))
         );
      }
   }

   if (visibleCombobox)
   {
      // if the combobox was just closed, move the cursor back to the parent element
      // focus the button if we're dealing with a combobox
      MenuPageItem::PageItemType type = visibleCombobox->getPageItemType();

      if (
            type == MenuPageItem::PageItemTypeCombobox
         || type == MenuPageItem::PageItemTypeEditableCombobox
      )
      {
         // move the mouse cursor there
         changeFocus(current->getItem(), visibleCombobox->getButtonItem());
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param current current graph element
   \param activeSlider active slider
*/
void MenuJoystickGraph::buttonForSlider(
   MenuPageSliderItem* activatedSlider
)
{
   // slider handling
   if (activatedSlider)
   {
      activatedSlider->mouseReleased();
   }
}


//-----------------------------------------------------------------------------
/*!
   \param current current graph element
*/
void MenuJoystickGraph::buttonForLists(MenuJoystickGraphElement* current)
{
   if (
         current
      && current->getItem()->getPageItemType() == MenuPageItem::PageItemTypeList
   )
   {
      MenuPageItem* item = current->getItem();
      MenuPageListItem* list = dynamic_cast<MenuPageListItem*>(item);

      bool active = item->isActive();

      // and deactivate it afterwards
      // or rather activate it for further item selection
      current->getItem()->setActive(!active);

      if (item->isActive())
      {
         // dummy behavior
         // list->setFocussedElement(18);

         int activeIndex = list->getActiveElement();
         list->scrollSmoothToIndex(activeIndex);
      }
      else
      {
         int focussed = list->getFocussedElement();
         list->setActiveElement(focussed);

         // TODO: move cursor back to table idle position
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param textedit textedit to process
*/
void MenuJoystickGraph::buttonForTextEdit(MenuPageTextEditItem *textedit)
{
   // since it was active before, it only needs to be deactivated in the end
   if (textedit && textedit->isEditingActive())
      textedit->deactivated();
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickGraph::button()
{
   // get current graph element
   MenuJoystickGraphElement* current = getFocussedElement();

   // get modal items
   MenuPageComboBoxItem* visibleCombobox = getVisibleCombobox();
   MenuPageSliderItem* activatedSlider = getActiveSlider();
   MenuPageTextEditItem* activeTextEdit = getEditingActiveTextEdit();

   buttonForLists(current);

   // handle mostly ever other item
   if (
         current
      && !activatedSlider
      && !activeTextEdit
   )
   {
      mousePress();
   }

   buttonForTextEdit(activeTextEdit);
   buttonForSlider(activatedSlider);
   buttonForComboBox(current, visibleCombobox);
}


//-----------------------------------------------------------------------------
/*!
   \param x mouse x coord to fill
   \param y mouse y coord to fill
*/
void MenuJoystickGraph::getMousePosInPsdCoords(int& x, int&y) const
{
   QWidget* activeWindow = qApp->activeWindow();
   QPoint mousePos = QCursor::pos();
   QPoint vpp = activeWindow->mapFromGlobal(mousePos);

   x = vpp.x();
   y = vpp.y();

   activeDevice->convertFromViewPort(&x, &y, 1920, 1080);

   qDebug(
      "MenuJoystickGraph::getMousePosInPsdCoords(): absolute x,y: %d, %d "
      "-> transformed: %d, %d",
      mousePos.x(),
      mousePos.y(),
      x,
      y
   );
}


//-----------------------------------------------------------------------------
/*!
   \return active element
*/
MenuJoystickGraphElement *MenuJoystickGraph::getFocussedElement() const
{
   // the active element needs to be retrieved from the menu system
   // i.e. the item that is currently active / focussed needs to be
   // returned here.

   // init
   MenuJoystickGraphElement* element = 0;
   Menu* menu = Menu::getInstance();
   MenuPage* page = menu->getCurrentPage();
   QList<MenuPageItem*> focussedItems;
   QWidget* activeWindow = qApp->activeWindow();
   int mousePsdX = 0;
   int mousePsdY = 0;

   // maybe there's a modal item currently opened (a combobox for example)
   MenuPageItem* modalItem = getModalItem();

   if (activeWindow)
   {
      if (modalItem)
      {
         focussedItems.push_back(modalItem);
      }
      else
      {
         getMousePosInPsdCoords(mousePsdX, mousePsdY);
         focussedItems = page->getItemsAt(mousePsdX, mousePsdY);
      }
   }

   // locate corresponding node
   foreach (MenuPageItem* pageItem, focussedItems)
   {
      foreach (MenuJoystickGraphElement* tmpElement, mElements)
      {
         if (tmpElement->getItem() == pageItem)
         {
            // found it
            element = tmpElement;
            break;
         }
      }
   }

   return element;
}


//-----------------------------------------------------------------------------
/*!
   \param element element to add
*/
void MenuJoystickGraph::add(MenuJoystickGraphElement *element)
{
   mElements.append(element);
}


//-----------------------------------------------------------------------------
/*!
   \return list of elements
*/
const QList<MenuJoystickGraphElement *> &MenuJoystickGraph::getElements() const
{
   return mElements;
}


