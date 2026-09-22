#pragma once

#include <QObject>
#include <QMap>
#include <QSet>

class MenuPage;
class PlayerInfo;

/// \brief page-navigation + real BombermanClient wiring for the menu system.
///
/// Started as a pure page-navigation stand-in for the real GameMenuWorkflow
/// (client/src/menus/gamemenuworkflow.cpp); now that BombermanClient itself is ported (see
/// project memory, Phase 4), SINGLE/MULTI/GAME_CREATE-OK/LOUNGE-start drive the real
/// host()/loginRequest()/createGame()/joinGame()/startGame() calls and follow the real signal
/// chain (loginResponse -> createGame -> createGameResponse -> joinGame -> joinGameResponse ->
/// LOUNGE), matching GameMenuWorkflow's own logic in each of those handlers. The GAME_CREATE
/// page's dropdowns/checkboxes are populated and read the same way the real
/// GameMenuInterfaceCreate (client/src/menus/gamemenuinterfacecreate.cpp) does - folded in here
/// rather than porting that whole class + its trivial GameMenuInterface/MenuInterface base
/// classes for one page's worth of logic.
/// Still not ported: the actual gameplay handoff once StartGameResponse arrives (level loading,
/// HUD, in-game rendering) - gameStarted() is logged, not acted on. That's the separate,
/// not-yet-scoped "Phase 5" step.
///
/// Deliberately named differently from GameMenuWorkflow so a future full port of that class isn't
/// confused with this one.
class MenuPageNavigator : public QObject
{
   Q_OBJECT

public:
   explicit MenuPageNavigator(QObject* parent = nullptr);

signals:
   void pageChangeRequest(const QString& page);
   void quitRequest();

public slots:
   void onActionRequest(const QString& page, const QString& action);

   //! mirrors GameMenuWorkflow::pageChanged() - populates GAME_CREATE's controls once the page
   //! actually becomes current. Connect to MenuDrawable::pageChanged(QString).
   void onPageChanged(const QString& page);

private slots:
   void onLoginResponse(bool granted);
   void onCreateGameResponse(bool granted, int gameId, bool owner);
   void onJoinGameResponse(bool success);
   void onGameStarted();

   //! mirrors GameMenuInterfaceCreate::updateCreateGamePlayerCounts()/updateCreateGameLevelPreview()
   void updateCreateGamePlayerCounts();
   void updateCreateGameLevelPreview();

   //! mirrors GameMenuInterfaceLounge::playerInfoMapUpdated() - repopulates the lounge's player
   //! rows (nick/wins/rank/owner-icon) whenever the player set changes (join/leave/bot added).
   void onPlayerInfoMapUpdated(QMap<int, PlayerInfo*>* playerInfo);

private:
   //! mirrors GameMenuInterfaceCreate::initializeCreateGameOptions()
   void initializeCreateGameOptions();

   //! mirrors GameMenuInterfaceCreate::createGame()
   void createGame();

   void setMonitorCreateGameOptionsEnabled(bool enabled);

   //! mirrors GameMenuInterfaceLounge::playerInfoMapUpdated() - the actual row-population logic,
   //! factored out so it can be called both on the live signal and once on first reaching LOUNGE.
   void updateLoungePlayerList(QMap<int, PlayerInfo*>* playerInfo);

   QList<QString> mSortedLevelNames;
   QList<QString> mSortedLevelDirNames;
   QSet<MenuPage*> mCreateGamePagesInitialized;
   QMap<int, int> mPlayerIdToIndexMap;
};
