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


#ifndef SERVERGUI_H
#define SERVERGUI_H

#include <QMainWindow>

class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class ServerGui: public QMainWindow
{
    Q_OBJECT

public:

   //! constructor
    ServerGui();

   //! destructor
   ~ServerGui();


public slots:

   //! add debug message
   void addDebugMessage(const QString& text, QtMsgType msgType);

   //! new player connected to server
   void newPlayer(int playerId, const QString& nickName);

   //! player disconnected to server
   void removePlayer(int playerId, const QString& nickName);

   //! new game created on server
   void newGame(int gameId, const QString& gameName);

   //! game removed on server
   void removeGame(int gameId, const QString& gameName);

private slots:

   //! start server
   void startServer();

   //! stop server
   void stopServer();

   //! exit 
   void exitServer();

   //! add player
   void addPlayer();

   //! item selected in player list
   void playerItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

   //! item selected in player list
   void gameItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

   //! remove player button clicked
   void removePlayer();


private:

   QTreeWidget* mPlayerList;     //!< player list
   QTreeWidget* mGameList;       //!< player list
   QListWidget* mLog;            //!< debug output
   QPushButton* mAddPlayer;      //!< add player button
   QPushButton* mRemovePlayer;   //!< remove player button
   QPushButton* mExit;           //!< exit button
   
   QString      mSelectedPlayer; //!< nick name of currently selected player

   QColor       mDebugColor;
   QColor       mWarningColor;
   QColor       mCriticalColor;
};


#endif
