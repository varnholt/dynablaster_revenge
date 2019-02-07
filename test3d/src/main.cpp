#include <QApplication>

#include "gui.h"
#include <QGLFormat>

int main( int argc, char ** argv )
{
   QApplication a( argc, argv );

   Gui gui(QGLFormat::defaultFormat());

   gui.show();

   a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
   return a.exec();
}

