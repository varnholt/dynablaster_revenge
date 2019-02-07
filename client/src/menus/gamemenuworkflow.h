#ifndef GAMEMENUWORKFLOW_H
#define GAMEMENUWORKFLOW_H

// base
#include "menuworkflow.h"

// constants
#include "constants.h"

// game
#include "gameinformation.h"

// Qt
#include <QTimer>

// forward declarations
class GameMenuInterfaceCreate;
class GameMenuInterfaceLounge;
class GameMenuInterfaceMain;
class GameMenuInterfaceOptions;
class GameMenuInterfaceSelect;
class GameMenuInterfaceCredits;
class PlayerInfo;

class GameMenuWorkflow : public MenuWorkflow
{
   Q_OBJECT

   public:

      //! constructor
      GameMenuWorkflow(QObject* parent = 0);

      //! initializer
      virtual void initialize();


   public slots:

      // workflow

      //! action request (if required goes to interface)
      virtual void actionRequest(
         const QString& page,
         const QString& action
      );

      void actionKeyPressed(
         const QString& page,
         const QString& itemName,
         int key
      );


      virtual void playerInfoMapUpdated(QMap<int, PlayerInfo*>*);

      virtual void loginResponse(bool granted);

      //! called when server is online
      virtual void serverOnline(bool online);

      virtual void createGameResponse(bool granted, int gameId, bool owner);

      virtual void gamesListUpdated(const QList<GameInformation>&);

      virtual void joinGameResponse(bool success);

      //! page was changed
      virtual void pageChanged(const QString&);

      //! a message was received
      virtual void messageReceived(
         int senderId,
         const QString& message,
         bool typingFinished
      );

      //! show main menu
      virtual void showMainMenu();

      //! menu is visible or not
      virtual void menuVisible(bool visible);


   signals:

      //! request game start
      void gameStartRequest(int game);

      //! request list of games
      void gameListRequest();

      //! request login
      void loginRequest(const QString& host, const QString& nick);

      //! request to join the selected game
      void joinGame(int gameId);

      //! request to leave game
      void leaveGameRequest();

      //! request game creation
      void createGameRequest(
         const QString& gameName,
         int levelId,
         int maxPlayers,
         int numStones,
         int numPowerUpBombs,
         int numPowerUpFlames
      );

      //! start server
      void host();

      //! send message
      void sendMessage(
         const QString& message,
         bool finishedTyping,
         int receiver = -1
      );

      //! main menu shown or not
      void mainMenuShown(bool);

      //! update fullscreen mode
      void updateFullscreen();

      //! update show fps flag
      void updateShowFps();

      //! update vsync
      void updateVsync();

      //! add a headless player
      void addHeadlessPlayer();


   protected:

      //! send lounge message signal
      void processLoungeMessage(bool finishedTyping);

      //! check if the saved host is localhost
      bool isHostLocal();

      //! timer used to get the list of games updated
      QTimer mGameListUpdateTimer;

      //! game menu interface for main menu
      GameMenuInterfaceMain* mGameMenuInterfaceMain;

      //! game menu interface for game select
      GameMenuInterfaceSelect* mGameMenuInterfaceSelect;

      //! game menu interface for create game
      GameMenuInterfaceCreate* mGameMenuInterfaceCreate;

      //! game menu interface for lounge
      GameMenuInterfaceLounge* mGameMenuInterfaceLounge;

      //! game menu interface for options
      GameMenuInterfaceOptions* mGameMenuInterfaceOptions;

      //! game menu interface for credits
      GameMenuInterfaceCredits* mGameMenuInterfaceCredits;
};

#endif // GAMEMENUWORKFLOW_H
