// header
#include "menujoystickhandler.h"

// game
#include "bombermanview.h"
#include "gamejoystickintegration.h"
#include "gamejoystickmapping.h"
#include "gamestatemachine.h"
#include "joystickinterface.h"
#include "menujoystickgraph.h"
#include "menujoystickgraphelement.h"
#include "menujoystickmouseanimation.h"
#include "gamesettings.h"

// menu
#include "menu.h"
#include "menupagebuttonitem.h"
#include "menupagecomboboxitem.h"

// Qt
#include <QTimer>
#include <QKeyEvent>


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
MenuJoystickHandler::MenuJoystickHandler(QObject *parent)
 : QObject(parent),
   mCurrentGraph(0),
   mSettings(0)
{
   mSettings =
      new QSettings(
         "data/menus/menu_gamepad.ini",
         QSettings::IniFormat
      );
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::initialize()
{
   // assign 1 psd for each page
   mSettings->beginGroup("pages");
   QStringList pages = mSettings->childKeys();
   mSettings->endGroup();

   foreach (const QString &pageKey, pages)
   {
      mSettings->beginGroup("pages");
      QString psdName = mSettings->value(pageKey).toString();
      mSettings->endGroup();

      MenuPage* menuPage = getPage(psdName);

      if (menuPage)
      {
         MenuJoystickGraph* graph = new MenuJoystickGraph();

         mSettings->beginGroup(pageKey);

         // qDebug("page: %s", qPrintable(page));

         QStringList elementsNames = mSettings->childKeys();
         mSettings->endGroup();

         // pass 1
         QMap<MenuPageItem*, MenuJoystickGraphElement*> elements;
         foreach (const QString& elementName, elementsNames)
         {
            // qDebug("elements: %s", qPrintable(elementName));

            QStringList items =
               mSettings->value(QString("%1/%2")
                  .arg(pageKey)
                  .arg(elementName)).toStringList();

            if (items.size() == 4)
            {
               // qDebug("items: %s", qPrintable(items.join(",")));

               MenuPageItem* self  = 0;
               MenuPageItem* north = 0;
               MenuPageItem* south = 0;
               MenuPageItem* east  = 0;
               MenuPageItem* west  = 0;

               self  = menuPage->getPageItem(elementName);
               north = menuPage->getPageItem(items.at(0));
               south = menuPage->getPageItem(items.at(1));
               east  = menuPage->getPageItem(items.at(2));
               west  = menuPage->getPageItem(items.at(3));

               /*
               // editable comboboxes hide their buttons
               QList<MenuPageItem*> editableComboboxButtons;
               QList<QString> editableComboboxNames;
               editableComboboxButtons << self;
               editableComboboxButtons << north;
               editableComboboxButtons << south;
               editableComboboxButtons << east;
               editableComboboxButtons << west;
               editableComboboxNames << elementName;
               editableComboboxNames << items.at(0);
               editableComboboxNames << items.at(1);
               editableComboboxNames << items.at(2);
               editableComboboxNames << items.at(3);
               for (int i = 0; i < 5; i++)
               {
                  MenuPageItem* cbItem = editableComboboxButtons.at(i);
                  QString cbName = editableComboboxNames.at(i);
                  if (cbItem && cbName.contains("editablecombobox") && cbName.contains("button"))
                  {
                     MenuPageButtonItem* button = MenuPageComboBoxItem::getButtonItem(cbName);
                     switch (i)
                     {
                        case 0:
                        self = button;
                        break;

                        case 1:
                        north = button;
                        break;

                        case 2:
                        south = button;
                        break;

                        case 3:
                        east = button;
                        break;

                        case 4:
                        west = button;
                        break;
                     }
                  }
               }
               */

               // create graph elements
               MenuJoystickGraphElement* element = new MenuJoystickGraphElement();

               element->setItem(self);
               element->setNorthItem(north);
               element->setSouthItem(south);
               element->setEastItem(east);
               element->setWestItem(west);

               elements.insert(self, element);

               graph->add(element);
            }
            else if (items.size() == 1)
            {
               // fields with only 1 item are the default ones
               if (elementName == "default")
               {
                  MenuPageItem* defaultItem =
                     menuPage->getPageItem(items.first());

                  graph->setDefaultPageItem(defaultItem);
               }
            }
         }

         // pass on default element
         // also initialize the 1st active element with the default element
         QMap<MenuPageItem*, MenuJoystickGraphElement*>::iterator it;
         it = elements.find(graph->getDefaultPageItem());
         if (it != elements.end())
         {
            MenuJoystickGraphElement* defaultElement = it.value();
            graph->setDefaultElement(defaultElement);
         }

         // pass 2
         foreach (MenuJoystickGraphElement* element, graph->getElements())
         {
            MenuPageItem* north = element->getNorthItem();
            MenuPageItem* south = element->getSouthItem();
            MenuPageItem* east  = element->getEastItem();
            MenuPageItem* west  = element->getWestItem();

            // north
            if (north)
            {
               it = elements.find(north);

               if (it != elements.end())
                  element->setNorth(it.value());
            }

            // south
            if (south)
            {
               it = elements.find(south);

               if (it != elements.end())
                  element->setSouth(it.value());
            }

            // east
            if (east)
            {
               it = elements.find(east);

               if (it != elements.end())
                  element->setEast(it.value());
            }

            // west
            if (west)
            {
               it = elements.find(west);

               if (it != elements.end())
                  element->setWest(it.value());
            }
         }

         // insert finalized graph
         mGraphMap.insert(psdName, graph);
      }
   }

   // unitTest1();

   // link all joysticks here
   connectJoysticks();

   // focus default item
   focusDefaultElement();
}


//-----------------------------------------------------------------------------
/*!
   \param pageName page to get
   \return ptr to page
*/
MenuPage* MenuJoystickHandler::getPage(const QString& pageName)
{
   MenuPage* page = Menu::getInstance()->getPageByName(pageName);
   return page;
}


//-----------------------------------------------------------------------------
/*!
   \param page page to activate
*/
void MenuJoystickHandler::activatePage(const QString &page)
{
   QMap<QString, MenuJoystickGraph*>::iterator it = mGraphMap.find(page);

   if (it != mGraphMap.end())
   {
      mCurrentGraph = it.value();
   }
}


//-----------------------------------------------------------------------------
/*!
   \param gji ptr to joystick integration
*/
void MenuJoystickHandler::addJoystickIntegration(GameJoystickIntegration* gji)
{
   connect(
      gji->getJoystickMapping(),
      SIGNAL(keyPressed(int)),
      this,
      SLOT(processKeyPressed(int))
   );
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if the gamepad has been used for the last mouse click
*/
bool MenuJoystickHandler::isGamepadUsed() const
{
   bool gamepad = false;

   MainDrawable* drawable = GameView::getInstance();
   GameView* gameView = dynamic_cast<GameView*>(drawable);

   Qt::KeyboardModifiers modifiers = Qt::NoModifier;

   if (gameView)
   {
      modifiers = gameView->getMousePressKeyboardModifiers();
      gamepad = (modifiers == Qt::ShiftModifier);
   }

   return gamepad;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::focusDefaultElement()
{
   // move the mouse cursor to the page default element
   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);

   // only do that if we have a joystick connected
   if (gji)
   {
      if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
      {
         if (isGamepadUsed())
         {
            // retrieve current menu page
            MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
            QString psdName = currentPage->getFilename();

            MenuJoystickGraph* current = getGraph(psdName);

            if (current)
            {
               current->changeFocus(
                  0,
                  current->getDefaultPageItem()
               );
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param key key event's key
*/
void MenuJoystickHandler::keyPressed(QKeyEvent * /*event*/)
{
   /*

   if (!event->isAutoRepeat())
   {
      updateCurrentGraph();

      GameSettings::ControllerSettings* controllerSettings =
         GameSettings::getInstance()->getControllerSettings();

      int key = event->key();

      if (key == controllerSettings->getUpKey())
      {
         north();
      }
      else if (key == controllerSettings->getDownKey())
      {
         south();
      }
      else if (key == controllerSettings->getLeftKey())
      {
         west();
      }
      else if (key == controllerSettings->getRightKey())
      {
         east();
      }
      else if (key == controllerSettings->getBombKey())
      {
         button();
      }
   }

   */
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::north()
{
   MenuJoystickGraph* graph = getCurrentGraph();

   if (graph)
   {
      graph->walk(MenuJoystickGraph::NavigationDirectionNorth);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::south()
{
   MenuJoystickGraph* graph = getCurrentGraph();

   if (graph)
   {
      graph->walk(MenuJoystickGraph::NavigationDirectionSouth);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::east()
{
   MenuJoystickGraph* graph = getCurrentGraph();

   if (graph)
   {
      graph->walk(MenuJoystickGraph::NavigationDirectionEast);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::west()
{
   MenuJoystickGraph* graph = getCurrentGraph();

   if (graph)
   {
      graph->walk(MenuJoystickGraph::NavigationDirectionWest);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::button()
{
   MenuJoystickGraph* graph = getCurrentGraph();

   if (graph)
   {
      graph->button();
   }
}


//-----------------------------------------------------------------------------
/*!
   \return ptr to graph
*/
MenuJoystickGraph *MenuJoystickHandler::getCurrentGraph() const
{
   return mCurrentGraph;
}


//-----------------------------------------------------------------------------
/*!
   \param value ptr to graph
*/
void MenuJoystickHandler::setCurrentGraph(MenuJoystickGraph *value)
{
   mCurrentGraph = value;
}


//-----------------------------------------------------------------------------
/*!
   \param page page name
*/
MenuJoystickGraph *MenuJoystickHandler::getGraph(const QString &page) const
{
   MenuJoystickGraph* graph = 0;

   QMap<QString, MenuJoystickGraph*>::const_iterator it =
      mGraphMap.constFind(page);

   if (it != mGraphMap.end())
   {
      graph = it.value();
   }

   return graph;
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::unitTest1()
{
   MenuJoystickGraph* current = getGraph("page_1");
   setCurrentGraph(current);

   QTimer::singleShot(10000, this, SLOT(east()));
   QTimer::singleShot(12000, this, SLOT(east()));
   QTimer::singleShot(14000, this, SLOT(north()));
   QTimer::singleShot(16000, this, SLOT(south()));

   QTimer::singleShot(18000, this, SLOT(west()));
   QTimer::singleShot(20000, this, SLOT(west()));
   QTimer::singleShot(22000, this, SLOT(west()));
   QTimer::singleShot(24000, this, SLOT(west()));

   QTimer::singleShot(26000, this, SLOT(button()));
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::connectJoysticks()
{
   // check if add headless can be successful
   int joystickCount = 0;

   GameJoystickIntegration* gji = GameJoystickIntegration::getInstance(0);

   if (gji)
   {
      joystickCount = gji->getJoystickInterface()->getJoystickCount();
   }

   for (int i = 0; i < joystickCount; i++)
   {
      GameSettings::DevelopmentSettings * devSettings =
         GameSettings::getInstance()->getDevelopmentSettings();

      if (devSettings->isJoysticksEnabled())
      {
         connect(
            gji->getJoystickMapping(),
            SIGNAL(keyPressed(int)),
            this,
            SLOT(processKeyPressed(int))
         );

         connect(
            gji->getJoystickMapping(),
            SIGNAL(keyReleased(int)),
            this,
            SLOT(processKeyReleased(int))
         );
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param key key to process
*/
void MenuJoystickHandler::processKeyQueue()
{
   // next key to process
   int key = mKeyQueue.dequeue();

   // process button
   GameSettings::ControllerSettings* controllerSettings =
      GameSettings::getInstance()->getControllerSettings();

   Qt::Key upKey = controllerSettings->getUpKey();
   Qt::Key downKey = controllerSettings->getDownKey();
   Qt::Key leftKey = controllerSettings->getLeftKey();
   Qt::Key rightKey = controllerSettings->getRightKey();
   Qt::Key bombKey = controllerSettings->getBombKey();

   if (key == upKey)
   {
      north();
   }
   else if (key == downKey)
   {
      south();
   }
   else if (key == leftKey)
   {
      west();
   }
   else if (key == rightKey)
   {
      east();
   }
   else if (key == bombKey)
   {
      button();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void MenuJoystickHandler::updateCurrentGraph()
{
   MenuPage* currentPage = Menu::getInstance()->getCurrentPage();
   QString psdName = currentPage->getFilename();
   MenuJoystickGraph* current = getGraph(psdName);
   setCurrentGraph(current);
}


//-----------------------------------------------------------------------------
/*!
   \param key key to process
*/
void MenuJoystickHandler::processKeyPressed(int key)
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      // retrieve current menu page
      updateCurrentGraph();

      // process button
      GameSettings::ControllerSettings* controllerSettings =
         GameSettings::getInstance()->getControllerSettings();

      Qt::Key upKey = controllerSettings->getUpKey();
      Qt::Key downKey = controllerSettings->getDownKey();
      Qt::Key leftKey = controllerSettings->getLeftKey();
      Qt::Key rightKey = controllerSettings->getRightKey();
      Qt::Key bombKey = controllerSettings->getBombKey();

      if (
            key == upKey
         || key == downKey
         || key == leftKey
         || key == rightKey
         || key == bombKey
      )
      {
         // limit the movement to 5 positions
         if (mKeyQueue.size() < 5)
         {
            mKeyQueue.enqueue(key);

            if (MenuJoystickMouseAnimation::isBusy())
            {
               connect(
                  MenuJoystickMouseAnimation::getLast(),
                  SIGNAL(done()),
                  this,
                  SLOT(processKeyQueue())
               );
            }
            else
            {
               processKeyQueue();
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param key key to process
*/
void MenuJoystickHandler::processKeyReleased(int key)
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      GameSettings::ControllerSettings* controllerSettings =
         GameSettings::getInstance()->getControllerSettings();

      if (key == controllerSettings->getUpKey())
      {
      }
      else if (key == controllerSettings->getDownKey())
      {
      }
      else if (key == controllerSettings->getLeftKey())
      {
      }
      else if (key == controllerSettings->getRightKey())
      {
      }
      else if (key == controllerSettings->getBombKey())
      {
      }
   }
}


