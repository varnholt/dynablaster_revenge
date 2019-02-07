#ifndef GAMEMENUINTERFACECREATE_H
#define GAMEMENUINTERFACECREATE_H

// base
#include "gamemenuinterface.h"

class GameMenuInterfaceCreate : public GameMenuInterface
{
   Q_OBJECT

public:

   GameMenuInterfaceCreate(QObject* parent = 0);

   //! create a game
   void createGame();

   //! deserialize create game data
   void deserializeCreateGameData();

   //! initialize create game options
   void initializeCreateGameOptions();

   //! enable/disable create game option monitoring
   void setMonitorCreateGameOptionsEnabled(bool enabled);


private slots:

   //! update create game options in case they're changed
   void updateCreateGamePlayerCounts();

   //! update level preview
   void updateCreateGameLevelPreview();



private:

   QList<QString> mSortedLevelNames;
   QList<QString> mSortedLevelDirNames;

};

#endif // GAMEMENUINTERFACECREATE_H
