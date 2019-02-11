// Qt
#include <QApplication>

// bot
#include "botfactory.h"
#include "botgui.h"

//-----------------------------------------------------------------------------
/*!
*/
int main(int argc, char *argv[])
{
   bool useGui = false;

   #ifdef Q_WS_X11
      useGui = getenv("DISPLAY") != 0;
   #else
      useGui = true;
   #endif

   QApplication app(argc, argv, useGui);

   if (useGui)
   {
      BotGui* gui = new BotGui();
      gui->show();
   }
   else
   {
      BotFactory factory;
      factory.add();
   }

   return app.exec();
}
