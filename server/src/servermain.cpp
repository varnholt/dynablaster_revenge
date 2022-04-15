// Qt
#ifdef USE_GUI
#include <QApplication>
#else
#include <QCoreApplication>
#endif

// server
#include "server.h"
#include "servergui.h"
#include "serverversion.h"

// shared
#include "systemtools.h"


namespace
{
static constexpr auto minimum_version_major = 5;
static constexpr auto minimum_version_minor = 0;
}

#ifdef USE_GUI
ServerGui* gui = nullptr;

void debugHandler(QtMsgType type, const QMessageLogContext& /*context*/, const QString &msg)
{
   if (gui)
   {
      gui->addDebugMessage(msg, type);
   }
}
#endif

int main( int argc, char ** argv )
{
   // check qt version
   QString version;
   if (!checkQtVersion(minimum_version_major, minimum_version_minor, &version))
   {
      qWarning(
         "The installed Qt version (%s) is invalid!\nRequired is a version >=%d.%d.",
         qPrintable(version),
         minimum_version_major,
         minimum_version_minor
      );

      return 0;
   }

#ifdef USE_GUI
   new QApplication(argc, argv, true);
#else
   new QCoreApplication(argc, argv, false);
#endif

   // install debug msg handler before the server is initialized
#ifdef USE_GUI
   gui = new ServerGui();
   gui->setMinimumSize(640, 480);
   gui->show();
   gui->setWindowTitle(
      QString("Dynablaster Revenge Server v%1 rev. %2")
         .arg(SERVER_VERSION)
         .arg(SERVER_REVISION)
   );
   qInstallMessageHandler(debugHandler);
#endif

   auto server = new Server();

   // connect ui to server
#ifdef USE_GUI
   gui->connect(
      server,
      SIGNAL(newPlayer(int,const QString&)),
      SLOT(newPlayer(int,const QString&))
   );

   gui->connect(
      server,
      SIGNAL(removePlayer(int,const QString&)),
      SLOT(removePlayer(int,const QString&))
   );

   gui->connect(
      server,
      SIGNAL(newGame(int,const QString&)),
      SLOT(newGame(int,const QString&))
   );

   gui->connect(
      server,
      SIGNAL(removeGame(int,const QString&)),
      SLOT(removeGame(int,const QString&))
   );
#else
   (void)server;
#endif

   return qApp->exec();
}



