#pragma once

// GLES3 port of client/src/game/gamewindrawable.cpp.

#include "drawable.h"
#include "math/matrix.h"
#include "weighted.h"
#include "gamestatemachine.h"
#include "menus/psdlayer.h"
#include "render/texture.h"

#include <string>

#include <QObject>

#include "math/color.h"

class BitmapFont;
class BlurFilter;
class FrameBuffer;
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
   GameWinDrawable(RenderDevice* dev, bool visible = false);
   virtual ~GameWinDrawable();

   virtual void initializeGL();
   virtual void paintGL();

   void setVisible(bool visible);
   void animate(float time);

   void setColor(const Color& color);
   const Color& getColor();

   void setColorEnum(Constants::Color color);
   Constants::Color getColorEnum() const;

   void setWinnerName(const std::string& name);
   const std::string& getWinnerName();

   void setDrawGame(bool draw);
   bool isDrawGame() const;

protected slots:
   void stateChanged();
   void startWinAnimation();

private:
   void initGlParameters();
   void cleanupGlParameters();

   void drawWinnerText();
   void initGameData();
   void drawGameData();
   void drawLayers(float alpha);
   void initializeLayers();

   float getContentsAlpha() const;
   float getDrawableAlpha() const;
   float getRadius() const;

   void hideLayers();
   int computeScore(PlayerInfo* info) const;

   void initializeWinnerScene();
   void initializePlayerMaterial();
   void updateWinAnimation(float time, float dt);
   void playSound();

   void drawBackBuffer(float alpha);
   void drawScene();
   void drawSceneToFramebuffer(float alpha);
   void drawPsdContents(float alpha);

   Color mColor;
   Constants::Color mColorEnum;
   std::string mWinnerName;

   BitmapFont* mLargeFont;
   BitmapFont* mDefaultFont;

   SceneGraph* mScene;

   float mRenderTime;
   float mTime;
   float mDeltaTime;
   float mStartTime;

   bool mDrawGame;

   BlurFilter* mBlur;

   // dedicated offscreen targets this port needs in place of the original's persistent
   // MainDrawable-owned game framebuffer (MainDrawable doesn't exist here - see gamedrawable.h).
   // mSnapshotTexture is a plain (non-FBO-attached) copy of the just-rendered frame - BlurFilter
   // reads from it while writing into mBackdropFb, avoiding a read/write feedback loop on the
   // same texture. mSceneFb holds the real 3D cup+player render, composited on top afterward.
   FrameBuffer* mBackdropFb;
   FrameBuffer* mSceneFb;
   unsigned int mSnapshotTexture;

   GameInformation* mGameInformation;
   QList<Weighted<PlayerInfo*, int>> mPlayerScores;
   float mPlayerScoresAnimated[10];

   PlayerItem* mPlayerItem;
   MotionMixer* mMotionMixer;
   Mesh* mPlayerMesh;
   PlayerMaterial* mPlayerMaterial;
   Texture mPlayerTextures[10];

   std::string mFilename;
   PSD mPsd;
   QList<PSDLayer*> mPsdLayers;

   PSDLayer* mRanks[10];
   PSDLayer* mIcons[10];
   PSDLayer* mNames[10];
   PSDLayer* mPoints[10];
   PSDLayer* mBars[10];
};
