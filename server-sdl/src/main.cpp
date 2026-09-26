#include <QCoreApplication>
#include <QDebug>

// SDL
#include <SDL3_net/SDL_net.h>

// server
#include "server.h"

// shared
#include "timer.h"

#include <chrono>
#include <thread>


int main(int argc, char** argv)
{
   if (!NET_Init())
   {
      qWarning("Failed to initialize SDL_net: %s", SDL_GetError());
      return 1;
   }

   QCoreApplication app(argc, argv);

   Server server;
   server.startPolling();

   // no real Qt event loop is driving anything anymore - keep processEvents() around for
   // deleteLater() and any other lingering Qt-Core internals, tick Timer for everything else.
   while (true)
   {
      QCoreApplication::processEvents();
      Timer::update();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
   }
}
