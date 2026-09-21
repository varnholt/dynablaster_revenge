#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

// Qt
#include <QObject>

// shared
#include "constants.h"

// Deferred subsystem (see project memory - Phase 4): the real SoundManager
// (client/src/game/soundmanager.{h,cpp}) pulls in a whole separate audio backend
// (client/soundsystem/) that hasn't been ported to client-sdl. Not required to get a game
// running - BombermanClient's sound calls are all fire-and-forget sfx triggers, so this is a
// no-op stand-in with the exact real interface, not a reimplementation. Real audio is a later,
// separate piece of work.
class SoundManager : public QObject
{
   Q_OBJECT

public:

   static SoundManager* getInstance();

   void fadeOut(float fadeOutTime);
   void restartPlayListAfterFadeOut(int delay);


public slots:

   void playSoundKilled();
   void playSoundBomb();
   void playSoundStart();
   void playSoundExtra();
   void playSoundTime(int time);
   void playSoundPlayerJoined();
   void playSoundPlayerLeft();
   void playSoundKick();
   void playSoundHurryUp();
   void playSoundMessageSent();
   void playSoundMessageReceived();
   void playSoundBombBounce();
   void playSoundBoxShake();
   void playSoundExtraRevealed();
   void playSkullSound(Constants::SkullType skullType);


protected:

   SoundManager();

   static SoundManager* sInstance;
};

#endif // SOUNDMANAGER_H
