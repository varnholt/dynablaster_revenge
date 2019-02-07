#ifndef SOUND_H
#define SOUND_H

#include <QObject>

class Sound : public QObject
{
Q_OBJECT

public:
    Sound();
    virtual ~Sound();

public slots:
   void detonation(int x, int y, int up, int down, int left, int right);
   void extra();
};

#endif // SOUND_H
