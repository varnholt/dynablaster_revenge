#ifndef GAMEMENUINTERFACESELECT_H
#define GAMEMENUINTERFACESELECT_H

// base
#include "gamemenuinterface.h"

// game
#include "gameinformation.h"


class GameMenuInterfaceSelect : public GameMenuInterface
{
   Q_OBJECT

public:

   // constructor
   GameMenuInterfaceSelect(QObject* parent = 0);

   //! store current game list
   void gamesListUpdated(const QList<GameInformation>& list);

   //! getter for selected game id
   int getSelectedGame();

   //! update server properties
   void updateServerProperties();

   //! update game properties
   void updateGameProperties();

   //! generate test list
   QList<GameInformation> generateTestList();


public slots:

   //! unit test
   void prepareUnitTests();

   //! unit test 1
   void unitTest1();

   //! unit test 2
   void unitTest2();

   //! unit test 3
   void unitTest3();

   //! unit test 4
   void unitTest4();


private:

   QMap<int, int> mGameMapIndexGameId;
   QMap<int, int> mGameMapGameIdIndex;

   // unit testing

   //! dummy list
   QList<GameInformation> mDummyList;

   //! dummy move counter
   int mDummyMoveCounter;

   //! dummy move direction
   int mDummyMoveDirection;
};

#endif // GAMEMENUINTERFACESELECT_H
