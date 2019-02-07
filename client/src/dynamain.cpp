/***************************************************************************
 *   Copyright (C) 2008 by Matthias Varnholt   *
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

// Qt
#include <QApplication>

// game
#include "game/bombermanclientgui.h"

// tools
#include "tools/array.h"
#include "tools/string.h"
#include "systemtools.h"

#ifdef WIN32
   #include <windows.h>
#endif


int main( int argc, char ** argv )
{
   // make xlib and glx thread safe under x11
   QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

   QString version;
   if (!checkQtVersion(4,8, &version))
   {
      qWarning(
            "The installed Qt version (%s) is invalid!\nRequired is version 4.8.x.",
            qPrintable( version )
      );
      return 0;
   }

   // tell windows that we need a screen for our thread execution
   #ifdef WIN32
      SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
   #endif

   QApplication a( argc, argv );

   BombermanClientGui gui(&a /*, 60.0f */ );
   gui.initialize();

   a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
   return a.exec();
}

