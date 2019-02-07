#ifndef MENUJOYSTICKHANDLER_H
#define MENUJOYSTICKHANDLER_H

// Qt
#include <QObject>
#include <QQueue>
#include <QMap>
#include <QSettings>

// forward declarations
class GameJoystickIntegration;
class MenuJoystickGraph;
class MenuPage;
class QKeyEvent;


class MenuJoystickHandler : public QObject
{
   Q_OBJECT

public:

   //! constructor
   explicit MenuJoystickHandler(QObject *parent = 0);

   //! getter for current graph
   MenuJoystickGraph* getCurrentGraph() const;

   //! setter for current graph
   void setCurrentGraph(MenuJoystickGraph *value);

   //! getter for graph by page psd name
   MenuJoystickGraph* getGraph(const QString& page) const;

   //! check if the gamepad has been used for the last mouseclick
   bool isGamepadUsed() const;


public slots:

   //! initialize graph
   virtual void initialize();

   //! activate a page
   void activatePage(const QString& page);

   //! add a joystick integration
   void addJoystickIntegration(GameJoystickIntegration *gji);

   //! menu page has been changed
   void focusDefaultElement();

   //! process key pressed event handed from gui
   void keyPressed(QKeyEvent*);


protected slots:

   //! go north
   void north();

   //! go south
   void south();

   //! go east
   void east();

   //! go west
   void west();

   //! button pressed
   void button();

   //! unit test 1
   void unitTest1();

   //! connect joystick signals
   void connectJoysticks();

   //! process key pressed (not given as key event)
   void processKeyPressed(int key);

   //! process key released
   void processKeyReleased(int key);

   //! process the key queue
   void processKeyQueue();


protected:

   //! update the graph that is currently being worked on
   void updateCurrentGraph();

   //! getter for menupage ptr by page name
   MenuPage *getPage(const QString &pageName);

   //! graph per page
   QMap<QString, MenuJoystickGraph*> mGraphMap;

   //! graph
   MenuJoystickGraph* mCurrentGraph;

   //! joystick mapping settings
   QSettings* mSettings;

   //! key queue
   QQueue<int> mKeyQueue;
};

#endif // MENUJOYSTICKHANDLER_H

