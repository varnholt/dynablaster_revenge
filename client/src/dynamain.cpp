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

//   QString version;
//   if (!checkQtVersion(5,8, &version))
//   {
//      qWarning(
//            "The installed Qt version (%s) is invalid!\nRequired is version 4.8.x.",
//            qPrintable( version )
//      );
//      return 0;
//   }

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

