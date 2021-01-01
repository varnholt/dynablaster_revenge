/***************************************************************************
 *   Copyright (C) 2005 by Matthias Varnholt   *
 *   matto@gmx.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef USE_GUI

// header
#include "servergui.h"

// server
#include "server.h"
#include "dummyplayer.h"

// Qt
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTcpSocket>

#define MAXENTRIES 100

//-----------------------------------------------------------------------------
/*!
   constructor
*/
ServerGui::ServerGui()
 : QMainWindow(),
   mPlayerList(0),
   mGameList(0),
   mLog(0),
   mAddPlayer(0),
   mRemovePlayer(0),
   mExit(0)
{
   mDebugColor.setRgb(0x0e, 0x0e, 0x0e, 255);
   mWarningColor.setRgb(0xff, 0x60, 0x00, 255);
   mCriticalColor.setRgb(0xff, 0x1b, 0x1b, 255);

   QWidget* window = new QWidget(this);
   setCentralWidget(window);

   QHBoxLayout *mainLayout = new QHBoxLayout(window);
   mainLayout->setMargin(2);

   // left side
   QVBoxLayout *left = new QVBoxLayout();
   mainLayout->addLayout(left);

   // player list
   mPlayerList = new QTreeWidget();
   mPlayerList->headerItem()->setText(0, "Players");
   mPlayerList->setRootIsDecorated(false);
   mPlayerList->setAlternatingRowColors(true);

   // game list
   mGameList= new QTreeWidget();
   mGameList->headerItem()->setText(0, "Games");
   mGameList->setRootIsDecorated(false);
   mGameList->setAlternatingRowColors(true);

#ifdef CODER
   mAddPlayer = new QPushButton("add player");
   mAddPlayer->setDisabled(true);
#endif

   mRemovePlayer = new QPushButton("remove player");
   mExit = new QPushButton("exit");

   mRemovePlayer->setDisabled(true);

   left->addWidget(mGameList);
   left->addWidget(mPlayerList);
#ifdef CODER
   left->addWidget(mAddPlayer);
#endif
   left->addWidget(mRemovePlayer);
   left->addWidget(mExit);

   // right side
   QVBoxLayout *right= new QVBoxLayout();
   mainLayout->addLayout(right);

   mLog = new QListWidget();
   right->addWidget(mLog);

   connect(
      mPlayerList,
      SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
      this,
      SLOT(playerItemChanged(QTreeWidgetItem*, QTreeWidgetItem*))
   );

   connect(
      mGameList,
      SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
      this,
      SLOT(gameItemChanged(QTreeWidgetItem*, QTreeWidgetItem*))
   );

   connect(
      mExit,
      SIGNAL(clicked()),
      this,
      SLOT(exitServer())
   );

#ifdef CODER
   connect(
      mAddPlayer,
      SIGNAL(clicked()),
      this,
      SLOT(addPlayer())
   );
#endif

   connect(
      mRemovePlayer,
      SIGNAL(clicked()),
      this,
      SLOT(removePlayer())
   );
}


//-----------------------------------------------------------------------------
/*!
   destructor
*/
ServerGui::~ServerGui()
{
}


//-----------------------------------------------------------------------------
/*!
   slot: new player logged into server
   \param playerId player id
   \param nickName nick name
*/
void ServerGui::newPlayer(int playerId, const QString& nickName)
{
   QTreeWidgetItem *item= new QTreeWidgetItem();
   item->setText(0, nickName);
   item->setData(0, Qt::UserRole, QVariant(playerId));
   mPlayerList->addTopLevelItem(item);
}


//-----------------------------------------------------------------------------
/*!
   slot: player disconnected from server
   \param playerId player id
   \param nickName nick name
*/
void ServerGui::removePlayer(int playerId, const QString&)
{
   for (int i=0; i < mPlayerList->topLevelItemCount(); )
   {
      QTreeWidgetItem *item = mPlayerList->topLevelItem(i);
      QVariant id = item->data(0, Qt::UserRole);
      if (id.toInt() == playerId)
         delete item;
      else
         i++;
   }
}


//-----------------------------------------------------------------------------
/*!
   slot: new game created on server
   \param gameId game id
   \param gameName game name
*/
void ServerGui::newGame(int gameId, const QString& gameName)
{
   QTreeWidgetItem *item = new QTreeWidgetItem();
   item->setText(0, gameName);
   item->setData(0, Qt::UserRole, QVariant(gameId));
   mGameList->addTopLevelItem(item);
}


//-----------------------------------------------------------------------------
/*!
   slot: game removed on server
   \param gameId game id
   \param gameName name of the game
*/
void ServerGui::removeGame(int gameId, const QString&)
{
   for (int i=0; i<mGameList->topLevelItemCount(); )
   {
      QTreeWidgetItem *item= mGameList->topLevelItem(i);
      QVariant id= item->data(0, Qt::UserRole);
      if (id.toInt() == gameId)
         delete item;
      else
         i++;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param playerItem currently selected player item
   \param prevItem previously selected player item
*/
void ServerGui::playerItemChanged(QTreeWidgetItem* playerItem, QTreeWidgetItem*)
{
   mRemovePlayer->setEnabled( playerItem!=0 );
}


//-----------------------------------------------------------------------------
/*!
   \param playerItem currently selected game item
   \param prevItem previously selected game item
*/
void ServerGui::gameItemChanged(QTreeWidgetItem* gameItem, QTreeWidgetItem*)
{
   mAddPlayer->setEnabled( gameItem!=0 );
}

//-----------------------------------------------------------------------------
/*!
   \param text text to add to debug output
   \param msgType msg type
*/
void ServerGui::addDebugMessage(const QString &text, QtMsgType msgType)
{
   QColor color;

   switch (msgType)
   {
      case QtDebugMsg:
         color = mDebugColor;
         break;
      case QtWarningMsg:
         color = mWarningColor;
         break;
      case QtCriticalMsg:
         color = mCriticalColor;
         break;
      default:
         break;
   }

   QTime time = QTime::currentTime();

   mLog->addItem(
      QString("%1: %2")
         .arg(time.toString("hh:mm:ss:zzz"))
         .arg(text)
   );

   while (mLog->count() > MAXENTRIES)
      mLog->takeItem(0);

   mLog->scrollToBottom();
}


//-----------------------------------------------------------------------------
/*!
   start the server
*/
void ServerGui::startServer()
{
}


//-----------------------------------------------------------------------------
/*!
   stop the server
*/
void ServerGui::stopServer()
{
}


//-----------------------------------------------------------------------------
/*!
   exit application
*/
void ServerGui::exitServer()
{
   qApp->exit();
}


//-----------------------------------------------------------------------------
/*!
   add a player to selected game
*/
void ServerGui::addPlayer()
{
   QTreeWidgetItem *item= mGameList->currentItem();
   if (item)
   {
      QVariant id = item->data(0, Qt::UserRole);
      QString nick = QString::number(qrand() % 99999999);
      DummyPlayer* player = new DummyPlayer(nick, id.toInt());
      Q_UNUSED(player);
   }
}


//-----------------------------------------------------------------------------
/*!
   remove selected player
*/
void ServerGui::removePlayer()
{
   QTreeWidgetItem *item= mPlayerList->currentItem();
   if (item)
   {
      QVariant id= item->data(0, Qt::UserRole);
      QTcpSocket* socket= Server::getInstance()->getPlayerSocket( id.toInt() );

      if (socket)
         socket->disconnectFromHost();
   }
}

#endif
