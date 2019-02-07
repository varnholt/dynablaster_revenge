
// base
#include "controllerui.h"
#include "ui_controllerui.h"

// Qt
#include <QSettings>
#include <QTimer>

#define COMPANY "Haujobb/Titan"
#define GAMENAME "Zone X0"


//-----------------------------------------------------------------------------
/*!
   \param parent parent widget
*/
ControllerUi::ControllerUi(QWidget *parent) :
    QFrame(parent),
    mUi(new Ui::controllerui)
{
   mUi->setupUi(this);

   QSettings settings(COMPANY, GAMENAME);
   QString nick = settings.value("controlui/nick").toString();
   QString color = settings.value("controlui/color").toString();
   mUi->mNick->setText(nick);
   mUi->mColor->setText(color);

   // hide id column
   mUi->mGames->setColumnHidden(0, true);

   connect(
      mUi->mLogin,
      SIGNAL(clicked()),
      this,
      SLOT(login())
   );

   connect(
      mUi->mJoin,
      SIGNAL(clicked()),
      this,
      SLOT(joinGame())
   );

   connect(
      mUi->mLeave,
      SIGNAL(clicked()),
      this,
      SLOT(leaveGame())
   );

   connect(
      mUi->mCreate,
      SIGNAL(clicked()),
      this,
      SLOT(createGame())
   );

   connect(
      mUi->mStart,
      SIGNAL(clicked()),
      this,
      SLOT(startGame())
   );

   connect(
      mUi->mStop,
      SIGNAL(clicked()),
      this,
      SLOT(stopGame())
   );

   mUpdateTableTimer = new QTimer();
   connect(
      mUpdateTableTimer,
      SIGNAL(timeout()),
      this,
      SIGNAL(updateTable())
   );
   mUpdateTableTimer->start(5000);
}


//-----------------------------------------------------------------------------
/*!
*/
ControllerUi::~ControllerUi()
{
    delete mUi;
}


//-----------------------------------------------------------------------------
/*!
*/
void ControllerUi::login()
{
   QSettings settings(COMPANY, GAMENAME);

   settings.setValue("controlui/nick", mUi->mNick->text());
   settings.setValue("controlui/color", mUi->mColor->text());

   emit login(mUi->mNick->text(), QColor(mUi->mColor->text()));
}


//-----------------------------------------------------------------------------
/*!
*/
void ControllerUi::joinGame()
{
   int row= mUi->mGames->currentRow();
   QTableWidgetItem *item= mUi->mGames->item(row, 0);
   if (item)
   {
      int game = item->text().toInt();
      emit joinGame(game);
   }
}



//-----------------------------------------------------------------------------
/*!
*/
void ControllerUi::leaveGame()
{
   int game = mUi->mGames->item(mUi->mGames->currentRow(), 0)->text().toInt();
   emit leaveGame(game);
}


//-----------------------------------------------------------------------------
/*!
*/
void ControllerUi::createGame()
{
   emit createGame(mUi->mGame->text());
}


//-----------------------------------------------------------------------------
/*!
*/
void ControllerUi::startGame()
{
   int row= mUi->mGames->currentRow();
   QTableWidgetItem *item= mUi->mGames->item(row, 0);
   if (item)
   {
      int game = item->text().toInt();
      emit startGame(game);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void ControllerUi::stopGame()
{
   int game = mUi->mGames->item(mUi->mGames->currentRow(), 0)->text().toInt();
   emit stopGame(game);
}


//-----------------------------------------------------------------------------
/*!
   \param table game information table
*/
void ControllerUi::updateTable(const QList<GameInformation>& table)
{
   QTableWidgetItem* id = 0;
   QTableWidgetItem* name = 0;
   QTableWidgetItem* count = 0;
   QTableWidgetItem* max = 0;

   int gameId = -1;
   bool found = false;

   // go through each element of the received table
   foreach (const GameInformation& game, table)
   {
      // find game id within the current table
      gameId = game.getId();

      found = false;
      for (int t = 0; t < mUi->mGames->rowCount(); t++)
      {
         // game already in list
         if (
               gameId
            == mUi->mGames->item(t, 0)->text().toInt())
         {
            found = true;
         }
      }

      // insert game information if not found
      if (!found)
      {
         mUi->mGames->insertRow(0);

         id = new QTableWidgetItem(
            QString("%1").arg(game.getId())
         );

         name = new QTableWidgetItem(
            QString(game.getGameName())
         );

         count = new QTableWidgetItem(
            QString("%1").arg(game.getPlayerCount())
         );

         max = new QTableWidgetItem(
            QString("%1").arg(game.getPlayerMaximumCount())
         );

         mUi->mGames->setItem(0, 0, id);
         mUi->mGames->setItem(0, 1, name);
         mUi->mGames->setItem(0, 2, count);
         mUi->mGames->setItem(0, 3, max);
      }
   }

   // remove obsolete items
   for (int i = 0; i < mUi->mGames->rowCount(); i++)
   {
      int gameId = mUi->mGames->item(i, 0)->text().toInt();

      bool found = false;

      for (int t = 0; t < table.size(); t++)
      {
         if (table.at(t).getId() == gameId)
            found = true;
      }

      if (!found)
      {
         mUi->mGames->removeRow(i);
      }
   }

   mUi->mGames->resizeColumnsToContents();
   mUi->mGames->resizeRowsToContents();
}


