#pragma once

#include <QObject>

/// \brief minimal stand-in for the pure page-navigation subset of the real GameMenuWorkflow
/// (client/src/menus/gamemenuworkflow.cpp) - i.e. purely "which button switches to which page",
/// with none of that class's real networking (login/create/join-game via BombermanClient, which
/// isn't ported yet - see project memory, Phase 4). Deliberately named differently from
/// GameMenuWorkflow so a future real port of that class isn't confused with this one.
///
/// Listens to Menu::actionRequest(page, action) and re-emits its own pageChangeRequest(QString)
/// for the actions that are pure navigation - wire that up to MenuDrawable::pageChangeRequest the
/// same way the real client/src/game/bombermanclientgui.cpp already connects
/// GameMenuWorkflow::pageChangeRequest to it (a signal-to-protected-slot connection; Qt's
/// string-based SIGNAL/SLOT connect bypasses normal C++ access control, which is exactly why the
/// original code could leave that slot protected).
///
/// Actions this doesn't recognize (SINGLE/MULTI/JOIN/CREATE-OK/POUET/FACEBOOK/etc, all of which
/// need real networking or an external browser launch this port doesn't have) are logged via
/// qDebug(), not silently swallowed - so the console makes it clear the click really was
/// received, only the follow-up behavior is unimplemented.
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
};
