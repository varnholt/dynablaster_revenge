#ifndef BOTFACTORY_H
#define BOTFACTORY_H

#include <string>
#include <vector>

// Qt
#include <QObject>
#include <QList>

// forward declarations
class Bot;
class BotClient;

class BotFactory : public QObject
{
   Q_OBJECT

public:

   explicit BotFactory(QObject *parent = 0);

   virtual ~BotFactory();

   void add(int count = 1);

   void setHostname(const std::string& hostname);

   const std::string& getHostname() const;

   void setGameId(int gameId);

   int getGameId() const;

   void removeAll();


protected slots:


   void removeBot();


protected:

   //! create a bot and a client instance
   void createBotClientPair();

   //! host to connect to
   std::string mHostname;

   //! game to join
   int mGameId;

   //! list of bot clients
   QList<BotClient*> mClients;

   //! list of bots
   QList<Bot*> mBots;

   //! list of given names
   std::vector<std::string> mGivenNames;
};

#endif // BOTFACTORY_H
