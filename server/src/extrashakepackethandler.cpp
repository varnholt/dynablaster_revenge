#include "extrashakepackethandler.h"

// Qt
#include <QTimer>

// server
#include "game.h"

// shared
#include "constants.h"
#include "extramapitem.h"
#include "extrashakepacket.h"
#include "map.h"
#include "stonemapitem.h"


ExtraShakePacketHandler::ExtraShakePacketHandler(QObject *parent) :
    QObject(parent)
{
   mCheckTimer = new QTimer(this);
   mCheckTimer->setInterval(SERVER_SHAKE_CHECK_INTERVAL);

   connect(
      mCheckTimer,
      SIGNAL(timeout()),
      this,
      SLOT(check())
   );
}


void ExtraShakePacketHandler::setEnabled(bool enabled)
{
   if (enabled)
      mCheckTimer->start();
   else
      mCheckTimer->stop();
}


void ExtraShakePacketHandler::setGame(Game *game)
{
   mGame = game;
}


Game *ExtraShakePacketHandler::getGame() const
{
   return mGame;
}


void ExtraShakePacketHandler::check()
{
   if (getGame()->getState() == Constants::GameActive)
   {
      Map* map = getGame()->getMap();

      if (map)
      {
         int w = map->getWidth();
         int h = map->getHeight();

         int x = qrand() % (w-1);
         int y = qrand() % (h-1);

         StoneMapItem* stone = dynamic_cast<StoneMapItem*>(map->getItem(x, y));

         if (stone)
         {
            ExtraMapItem* extra = stone->getExtraMapItem();

            if (extra)
            {
                // qDebug("ExtraShakePacketHandler::check(): extra found at %d,%d", x, y);
                ExtraShakePacket* packet = new ExtraShakePacket(stone->getUniqueId());
                getGame()->addOutgoingPacket(packet);
            }
         }
      }
   }
}





