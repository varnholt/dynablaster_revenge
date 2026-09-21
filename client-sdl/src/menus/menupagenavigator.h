#pragma once

#include <QObject>

/// \brief page-navigation + real BombermanClient wiring for the menu system.
///
/// Started as a pure page-navigation stand-in for the real GameMenuWorkflow
/// (client/src/menus/gamemenuworkflow.cpp); now that BombermanClient itself is ported (see
/// project memory, Phase 4), SINGLE/MULTI/GAME_CREATE-OK/LOUNGE-start drive the real
/// host()/loginRequest()/createGameAutomatic()/joinGame()/startGame() calls and follow the real
/// signal chain (loginResponse -> createGame -> createGameResponse -> joinGame ->
/// joinGameResponse -> LOUNGE), matching GameMenuWorkflow's own logic in each of those handlers.
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

private slots:
   void onLoginResponse(bool granted);
   void onCreateGameResponse(bool granted, int gameId, bool owner);
   void onJoinGameResponse(bool success);
   void onGameStarted();
};
