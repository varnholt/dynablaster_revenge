#include "sound.h"

#include <QSound>


Sound::Sound()
   : QObject()
{
}

Sound::~Sound()
{
}

void Sound::detonation(int x, int y, int up, int down, int left, int right)
{
   QSound::play("data/bomb.wav");
}

void Sound::extra()
{
   QSound::play("data/extra.wav");
}
