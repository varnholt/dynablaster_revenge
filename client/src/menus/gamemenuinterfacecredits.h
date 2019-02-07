#pragma once

#include "gamemenuinterface.h"

class GameMenuInterfaceCredits : public GameMenuInterface
{
   Q_OBJECT
public:
   explicit GameMenuInterfaceCredits(QObject *parent = 0);

signals:

public slots:

   void reset();

};

