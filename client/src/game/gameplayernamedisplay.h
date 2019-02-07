#ifndef GAMEPLAYERNAMEDISPLAY_H
#define GAMEPLAYERNAMEDISPLAY_H

// Qt
#include <QObject>
#include <QMap>

// game
#include "playerboundingrect.h"
#include "framework/frametimer.h"

// render
#include "render/texture.h"

// forward declarations
class BitmapFont;
class Material;
class Matrix;
class PlayerItem;

class GamePlayerNameDisplay : public QObject
{
   Q_OBJECT

public:

   //! constructor
   explicit GamePlayerNameDisplay(QObject *parent = 0);

   //! check if display is active
   bool isActive() const;


signals:
   

public slots:

   //! setter for player data
   void setPlayerData(QMap<int,PlayerItem*>& players);

   //! draw
   void draw() const;

   //! draw player texts
   void drawPlayTexts() const;

   //! draw arrow
   void drawArrow() const;

   //! start showing texts
   void start();

   //! initialize gl
   void initialize();


protected:

   //! draw a bounding box around a player (for debugging purposes)
   void drawBoundingBox(const Vector& min, const Vector& max) const;

   //! init gl context
   void initGlParameters() const;

   //! cleanup gl context
   void cleanupGlParameters() const;

   //! compute alpha
   float computeFontAlpha() const;

   //! compute alpha
   float computeArrowAlpha() const;

   //! frame time
   FrameTimer mActiveTime;

   //! positions
   QList<Vector> mPositions;

   //! names
   QList<QString> mNames;

   //! bitmap font
   BitmapFont* mFont;

   //! show arrow
   bool mShowArrow;

   //! arrow texture
   Texture mArrowTexture;

   //! own player position
   Vector mArrowPosition;
};

#endif // GAMEPLAYERNAMEDISPLAY_H
