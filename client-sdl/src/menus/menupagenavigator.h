#pragma once

#include "hosthistory.h"

#include <QObject>
#include <QMap>

#include <unordered_set>

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

   //! mirrors GameMenuInterfaceOptions::applyVolumeMusic()/applyVolumeSfx() - forwards a dragged
   //! slider's value straight to SoundManager (live volume change, not yet persisted).
   void applyVolumeMusic(float volume);
   void applyVolumeSfx(float volume);

   //! mirrors GameMenuWorkflow::messageReceived() - appends a finished chat line to the lounge's
   //! message table. Typing-in-progress notifications (finished == false) are intentionally
   //! ignored here (see the "typing bubble" comment in updateLoungePlayerList()).
   void onMessageReceived(int senderId, const QString& message, bool finished);

private:
   //! mirrors GameMenuInterfaceMain::deserializeLoginData() - repopulates the main menu's host
   //! combobox (from HostHistory) and nick/host text fields (from GameSettings) whenever the
   //! main menu becomes current, including once at startup (see the constructor).
   void deserializeLoginData();

   //! mirrors GameMenuInterfaceMain::updateLoginData() - captures whatever's currently typed in
   //! the nick/host fields into GameSettings + HostHistory, called before leaving the main menu.
   void updateLoginData();

   //! mirrors GameMenuInterfaceCreate::deserializeCreateGameData() - repopulates the "game name"
   //! text field from GameSettings (defaults to "Default" the very first time, then whatever was
   //! last typed) whenever the GAME_CREATE page becomes current.
   void deserializeCreateGameData();

   //! mirrors GameMenuInterfaceCreate::initializeCreateGameOptions()
   void initializeCreateGameOptions();

   //! mirrors GameMenuInterfaceCreate::createGame()
   void createGame();

   void setMonitorCreateGameOptionsEnabled(bool enabled);

   //! mirrors GameMenuInterfaceOptions::deserializeAudioSettings() - seeds the audio options
   //! page's sliders from SoundManager's current volume, called whenever OPTIONS_AUDIO becomes
   //! current.
   void deserializeAudioSettings();

   //! mirrors GameMenuInterfaceOptions::setMonitorAudioSettingsEnabled().
   void setMonitorAudioSettingsEnabled(bool enabled);

   //! mirrors GameMenuInterfaceOptions::restoreAudioDefaults() - resets GameSettings' audio
   //! volumes, re-applies them to SoundManager, and re-seeds the sliders' visual positions.
   void restoreAudioDefaults();

   //! mirrors GameMenuInterfaceLounge::playerInfoMapUpdated() - the actual row-population logic,
   //! factored out so it can be called both on the live signal and once on first reaching LOUNGE.
   void updateLoungePlayerList(QMap<int, PlayerInfo*>* playerInfo);

   //! mirrors GameMenuInterfaceLounge::addLoungeMessage() - word-wraps and appends one chat line
   //! (already formatted as "nick: text" by the server) to the lounge's message table.
   void addLoungeMessage(int senderId, const QString& message);

   QList<QString> mSortedLevelNames;
   QList<QString> mSortedLevelDirNames;
   std::unordered_set<MenuPage*> mCreateGamePagesInitialized;
   QMap<int, int> mPlayerIdToIndexMap;
   HostHistory mHostHistory;
};
