#include "botfactory.h"

// Qt
#include <QDateTime>
#include <QFile>
#include <QtCore/QCoreApplication>
#include <QStringList>

// bot
#include "botclient.h"
#include "botmap.h"
#include "protobot.h"
#include "protobotinsults.h"


//-----------------------------------------------------------------------------
/*!
*/
BotFactory::BotFactory(QObject *parent)
 : QObject(parent),
   mGameId(-1)
{
   setHostname("127.0.0.1");
}


//-----------------------------------------------------------------------------
/*!
*/
BotFactory::~BotFactory()
{
   // bots are deleted in client destructor
   qDeleteAll(mClients);
   mClients.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotFactory::createBotClientPair()
{
   // todo read data from command line
   // - bot type
   // - host
   // - game id or command to create game
   // - start game flag or command to wait
   BotClient* client = new BotClient();
   ProtoBot* bot = new ProtoBot();

   bot->setPlayerInfoMap(client->getPlayerInfoMap());

   QStringList nicks;

   QFile nicksFile("data/server/botnicks.txt");

   if (nicksFile.open(QFile::ReadOnly))
   {
      QTextStream stream(&nicksFile);
      QString line;
      do
      {
         line = stream.readLine();

         if (!line.trimmed().isEmpty())
            nicks << line;

      } while (!line.isNull());
   }
   else
   {
      nicks << "r2d2" << "c3po" << "bender" << "no5" << "t-1000"
            << "t-800" << "cyclon" << "ramrod" << "h8" << "gort"
            << "astroboy" << "clank" << "rosie" << "hal9000" << "sentinel"
            << "vision" << "cyberman" << "alpha" << "wall-e" << "asimo"
            << "cylon" << "voltron" << "wheatley" << "megaman" << "brainiac"
            << "eve" << "pneuman" << "optimus" << "robby" << "awesome-o";
   }

   qsrand(QTime::currentTime().msec());

   QString nick;

   if (nicks.size() >= 9)
   {
      // if we have sufficient nicks in that list we'll keep choosing new
      // nicks until we'll have found a non-duplicate nick
      bool duplicate = true;

      while (duplicate)
      {
         nick = nicks[qrand() % (nicks.size()-1)];
         duplicate = mGivenNames.contains(nick);
      }

      mGivenNames.push_back(nick);
   }
   else
   {
      // if the nick list size is smaller than the possible bot maximum,
      // just choose any of the names
      nick = nicks[qrand() % (nicks.size()-1)];
   }

   client->setBot(bot);
   client->setGameId(getGameId());
   client->setHost(getHostname());
   client->setNick(nick);
   client->setAutoJoin(true);
   client->setAutoStart(false);
   client->initialize();
   client->connectToServer();

   bot->start();

//   // link client to bot and vice versa
//   connect(
//      bot->getInsults(),
//      SIGNAL(sendMessage(QString)),
//      client,
//      SLOT(sendMessage(QString))
//   );

   connect(
      client,
      SIGNAL(updatePlayerId(int)),
      bot,
      SLOT(updatePlayerId(int))
   );

   connect(
      client,
      SIGNAL(updatePlayerPosition(int,float,float,float)),
      bot,
      SLOT(updatePlayerPosition(int,float,float,float))
   );

   connect(
      client,
      SIGNAL(extraShake(int)),
      bot,
      SLOT(extraShake(int))
   );

   connect(
      client,
      SIGNAL(gameStarted()),
      bot,
      SLOT(wakeUp())
   );

   connect(
      bot,
      SIGNAL(walk(qint8)),
      client,
      SLOT(walk(qint8))
   );

   connect(
      bot,
      SIGNAL(bomb()),
      client,
      SLOT(bomb())
   );

   connect(
      client,
      SIGNAL(markHazardousTemporary(int,int,int,int)),
      bot,
      SLOT(markHazardousTemporary(int,int,int,int))
   );

   connect(
      client,
      SIGNAL(bombKicked(int,int,Constants::Direction,int)),
      bot,
      SLOT(bombKicked(int,int,Constants::Direction,int))
   );

   connect(
      bot,
      SIGNAL(sync()),
      client,
      SLOT(deleteObsoleteMapItems()),
      Qt::DirectConnection
   );

   // be notified if bot is to be removed
   connect(
      client,
      SIGNAL(remove()),
      this,
      SLOT(removeBot())
   );

   mClients << client;
   mBots << bot;
}


//-----------------------------------------------------------------------------
/*!
   \param count number of bots to add
*/
void BotFactory::add(int count)
{
   mGivenNames.clear();

   for (int i = 0; i < count; i++)
      createBotClientPair();

   mGivenNames.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotFactory::setHostname(const QString &hostname)
{
   mHostname = hostname;
}


//-----------------------------------------------------------------------------
/*!
*/
const QString &BotFactory::getHostname() const
{
   return mHostname;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotFactory::setGameId(int gameId)
{
   mGameId = gameId;
}


//-----------------------------------------------------------------------------
/*!
*/
int BotFactory::getGameId() const
{
   return mGameId;
}


//-----------------------------------------------------------------------------
/*!
*/
void BotFactory::removeAll()
{
   foreach (BotClient* client, mClients)
   {
      client->deleteLater();
   }

   mClients.clear();
   mBots.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void BotFactory::removeBot()
{
   BotClient* client = dynamic_cast<BotClient*>(sender());

   if (client)
   {
      client->deleteLater();
      mClients.removeOne(client);
   }
}
