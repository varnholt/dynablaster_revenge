#ifndef GAMESTATSDRAWABLE_H
#define GAMESTATSDRAWABLE_H

// Qt
#include <QColor>
#include <QImage>
#include <QMap>
#include <QObject>

// base
#include "drawable.h"

// game
#include "constants.h"

// framework
#include "tools/array.h"
#include "image/psd.h"
#include "vertex.h"

// forward declarations
class BitmapFont;
class QImage;
class PSDLayer;

class GameStatsDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

       //! constructor
       GameStatsDrawable(RenderDevice*);

       //! destructor
       virtual ~GameStatsDrawable();

       //! initialize
       void initializeGL();

       //! draw
       void paintGL();

       //! animate
       virtual void animate(float globalTime);

       //! setter for skull type
       void setSkullType(Constants::SkullType);

       //! getter for skull type
       Constants::SkullType getSkullType() const;

       //! setter for visible
       virtual void setVisible(bool visible);

       //! getter for last duration
       int getLastDuration() const;


   public slots:

       //! setter for player score
       void setPlayerScore(int);

       //! setter for game time
       void setGameTimeLeft(int left, int duration);

       //! a player has been infected
       void playerInfected(
          int id,
          Constants::SkullType,
          int infectorId,
          int extraX,
          int extraY
       );


   protected slots:

       //! reset grayscales on game start
       void resetGrayscales();


   protected:

      //! initialize stats layers
      void initializeLayers();

      //! initialize orb gradient
      void initializeOrb();

      //! draw main overlay
      void drawOverlay();

      //! draw player icons
      void drawPlayerIcons();

      //! draw player name
      void drawPlayerInfo();

      //! draw game time
      void drawGameTime();

      //! draw orb
      void drawOrb();

      //! draw skull
      void drawSkull();

      //! draw skull progress
      void drawSkullProgress();

      //! draw mute icons
      void drawMuteIcons();

      //! init gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();

      //! animate death gray scales
      void animateDeathGrayScales(float dt);


      // testing

      //! unit test 1
      void unitTest1();


      // skull functionality

      //! animate skulls
      void animateSkulls(float dt);

      //! getter for skull progress
      float getSkullProgress() const;


      // overlay members

      //! psd instance
      PSD mPsd;

      //! list of layer ids
      QList<PSDLayer*> mRenderLayers;

      //! filename to load from
      QString mFilename;

      //! font time font
      BitmapFont* mTimeFont;

      //! outline font
      BitmapFont* mOutline;

      //! scaling to renderdevice dimensions
      float mScaleFactor;

      //! orb image
      QImage mOrbPalette;


      // layers

      //! player layers
      PSDLayer* mLayersPlayer[10];

      //! orb layer
      PSDLayer* mLayerOrb;

      //! orb panel layer
      PSDLayer* mLayerPanelOrb;

      //! mute music layer
      PSDLayer* mLayerMuteMusic;

      //! mute sfx layer
      PSDLayer* mLayerMuteSfx;

      //! skull layers
      PSDLayer* mLayersSkulls[Constants::SkullReset + 1];

      //! skull progress layer
      PSDLayer* mLayerSkullProgressStart;

      //! skull progress layer end part
      PSDLayer* mLayerSkullProgressMiddle;


      // data to display

      //! player's game score
      int mPlayerScore;

      //! server game time
      int mTimeLeft;

      //! overall time
      int mDuration;


      // vertices of the displayed data

      //! time vertices
      Array<Vertex> mGameTimeVertices;

      //!
      float mPlayerNameOffsetX;
      float mPlayerNameOffsetY;

      float mPlayerScoreOffsetX;
      float mPlayerScoreOffsetY;

      float mGameTimeOffsetX;
      float mGameTimeOffsetY;
      float mGameTimeHeight;


      // dead player grayscale

      //! grayscale shader
      unsigned int mGrayscaleShader;

      //! grayscale intensity shader uniform
      int mParamGrayscale;

      //! list of grayscales
      float mGrayscales[11];

      //! last global time
      float mLastGlobalTime;


      // skull handling

      //! current skull type
      Constants::SkullType mSkullType;

      //! skull animation duration
      float mSkullDuration;

      //! skull animation start time
      float mSkullAnimationStartTime;

      //! skull alpha
      float mSkullAlpha;

      //! skull aborted
      bool mSkullAborted;

      //! progress shader
      unsigned int mProgressShader;

      //! progress shader uniform
      int mParamProgress;

      //! progress bar width
      int mProgressBarWidth;

};

#endif // GAMESTATSDRAWABLE_H
