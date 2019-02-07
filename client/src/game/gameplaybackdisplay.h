#ifndef GAMEPLAYBACKDISPLAY_H
#define GAMEPLAYBACKDISPLAY_H

// Qt
#include <QObject>

// forward declarations
class BitmapFont;

class GamePlaybackDisplay : public QObject
{
   Q_OBJECT

public:
   explicit GamePlaybackDisplay(QObject *parent = 0);
   
signals:
   
public slots:

   //! draw
   void draw(float time) const;

   //! initialize font
   void initialize();


protected:

   //! init gl parameters
   void initGlParameters() const;

   //! init gl parameters
   void cleanupGlParameters() const;

   //! draw text overlay
   void drawTextOverlay() const;

   //! bitmap font
   BitmapFont* mFont;
   
};

#endif // GAMEPLAYBACKDISPLAY_H
