#include "botfactory.h"

BotFactory::BotFactory(QObject* parent)
   : QObject(parent)
{
}

void BotFactory::add(int) {}
void BotFactory::setHostname(const QString&) {}
void BotFactory::setGameId(int) {}
void BotFactory::removeAll() {}
