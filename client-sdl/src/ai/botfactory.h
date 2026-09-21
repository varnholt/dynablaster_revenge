#ifndef BOTFACTORY_H
#define BOTFACTORY_H

// Qt
#include <QObject>
#include <QString>

// Deferred subsystem (see project memory - Phase 4): the real BotFactory (top-level ai/, ~25
// files - A* pathfinding, bot behavior state machine) is a whole separate feature, not required
// to get a live human client<->server game running. No-op stand-in with the exact real interface
// BombermanClient calls into - add()/removeAll() do nothing, so games simply start with no bots.
class BotFactory : public QObject
{
   Q_OBJECT

public:

   explicit BotFactory(QObject* parent = nullptr);

   void add(int count = 1);
   void setHostname(const QString& hostname);
   void setGameId(int gameId);
   void removeAll();
};

#endif // BOTFACTORY_H
