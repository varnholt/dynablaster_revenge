#ifndef GAMEWINDRAWABLE_H
#define GAMEWINDRAWABLE_H

// base
#include "drawable.h"

// framework
#include "math/matrix.h"

// shared
#include "weighted.h"

// game
#include "gamestatemachine.h"

// menus
#include "psdlayer.h"

// Qt
#include <QColor>
#include <QObject>
#include <QString>

// forward declarations
class BitmapFont;
class BlurFilter;
class GameInformation;
class Mesh;
class MotionMixer;
class PlayerMaterial;
class PlayerInfo;
class PlayerItem;
class SceneGraph;

class GameWinDrawable : public QObject, public Drawable
{
   Q_OBJECT

   public:

      //! constructor
      GameWinDrawable(RenderDevice* dev, bool visible = false);

      //! destructor
      virtual ~GameWinDrawable();

      //! initialzie gl context
      virtual void initializeGL();

      //! paint
      virtual void paintGL();

      // set visible
      void setVisible(bool visible);

      //! animate
      void animate(float time);

      //! setter for color
      void setColor(const QColor& color);

      //! getter for color
      const QColor& getColor();

      //! setter for color enum
      void setColorEnum(Constants::Color color);

      //! getter for color enum
      Constants::Color getColorEnum() const;

      //! setter for winner name
      void setWinnerName(const QString& name);

      //! getter for winner name
      const QString& getWinnerName();

      //! setter for draw flag
      void setDrawGame(bool draw);

      //! getter for draw flag
      bool isDrawGame() const;


   protected slots:

      //! game state changed
      void stateChanged();

      //! start win animation
      void startWinAnimation();


   private:

      //! initialize gl parameters
      void initGlParameters();

      //! cleanup gl parameters
      void cleanupGlParameters();

      //! draw winner text
      void drawWinnerText();

      //! init game information
      void initGameData();

      //! draw game information
      void drawGameData();

      //! draw layers
      void drawLayers(float alpha);

      //! initialize psd layers
      void initializeLayers();

      //! compute alpha
      float getContentsAlpha() const;

      //! compute drawable alpha
      float getDrawableAlpha() const;

      //! compute blur radius
      float getRadius() const;

      //! hide all layers
      void hideLayers();

      //! computer player score
      int computeScore(PlayerInfo* info) const;

      //! load winner scene
      void initializeWinnerScene();

      //! update winner scene
      void initializePlayerMaterial();

      //! update win animation
      void updateWinAnimation(float time, float dt);

      //! play win/draw sound
      void playSound();

      //! debug a matrix
      void debugMatrix(const Matrix&, const char* name = "");

      //! draw the back buffer
      void drawBackBuffer(float alpha);

      //! draw the 3d scene
      void drawScene();

      //! draw psd contents
      void drawPsdContents(float alpha);

      //! winner color
      QColor mColor;

      //! winner color enum type
      Constants::Color mColorEnum;

      //! winner name
      QString mWinnerName;

      //! large bitmap font
      BitmapFont* mLargeFont;

      //! default font
      BitmapFont* mDefaultFont;

      //! winner cup
      SceneGraph* mScene;

      //! cup matrix
      // Matrix mTransform;

      //! application time
      float mRenderTime;

      //! normalize time
      float mTime;

      //! delta time
      float mDeltaTime;

      //! application time
      float mStartTime;

      //! draw game flag
      bool mDrawGame;

      //! blur filter
      BlurFilter* mBlur;

      //! game information ptr
      GameInformation* mGameInformation;

      //! player scores
      QList<Weighted<PlayerInfo*, int> > mPlayerScores;

      //! player scores
      float mPlayerScoresAnimated[10];

      // win animation

      //! player item
      PlayerItem* mPlayerItem;

      //! motion mixer
      MotionMixer* mMotionMixer;

      //! player mesh
      Mesh* mPlayerMesh;

      //! player material
      PlayerMaterial* mPlayerMaterial;

      //! all player textures
      Texture mPlayerTextures[10];


      // psd handling

      //! psd filename
      QString mFilename;

      //! psd instance
      PSD mPsd;

      //! psd layer list
      QList<PSDLayer*> mPsdLayers;

      PSDLayer* mRanks[10];
      PSDLayer* mIcons[10];
      PSDLayer* mNames[10];
      PSDLayer* mPoints[10];
      PSDLayer* mBars[10];
};

#endif // GAMEWINDRAWABLE_H
