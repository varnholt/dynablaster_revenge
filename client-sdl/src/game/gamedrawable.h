#ifndef GAMEDRAWABLE_H
#define GAMEDRAWABLE_H

// Qt
#include <QObject>
#include <QSet>
#include <QString>
#include <QMap>
#include <QQueue>

// tools
#include "tools/map2d.h"
#include "tools/array.h"

// game
#include "constants.h"
#include "playerboundingrect.h"

// engine
#include "drawable.h"
#include "math/matrix.h"

// forward declarations
class DetonationManager;
class ExtraMapItem;
class GamePlayerNameDisplay;
class Level;
class MapItem;
class Material;
class Mesh;
class Node;
class PlayerDeathEffect;
class PlayerItem;
class QKeyEvent;
class RenderDevice;
class SceneGraph;
class Skull;

// Trimmed GLES3 port of client/src/game/gamedrawable.{h,cpp} (see project memory - Phase 5).
// Real map/players/bombs/extras/HUD rendering. PlayerDeathEffect (the GPU flowfield-particle
// death burst) and GamePlayerNameDisplay (Tab overlay) were ported for real - see
// playerdeatheffect.{h,cpp} / gameplayernamedisplay.{h,cpp}. Still deliberately dropped
// (see project memory for the full reasoning per item, all deferred - not missing by oversight):
// ExtraAnimation/ExtraRevealAnimation (immediate-mode-GL particle reveal effects), MushroomAnimation+
// ShroomFilter, InvisiblePlayerEffect, PlayerInfectedEffect/PlayerInvincibleEffect (the same class
// of flowfield-particle status-effect visual, just for infection/invincibility instead of death -
// the actual game state still flows through BombermanClient regardless), RibbonAnimationFactory/StarTalersFactory/
// LensFlareFactory (round-end/bonus visual flourishes), GamePlaybackDisplay (moot - GamePlayback
// is a no-op stub), FuseParticle/FuseParticleEmitter (bomb fuse sparks),
// HeadlessIntegration (bot/multi-instance tooling), the MainDrawable-owned offscreen
// gameFb/BlendQuad blit (MainDrawable doesn't exist in this port - renders straight to the
// default framebuffer instead, which is safe now that the mushroom/invisibility effects that
// needed to read it back as a texture are both dropped), drawTestQuad/drawBoundingBox/
// drawBoundingRect/playerBoundingRect/loadExplosion (all genuinely dead code in the original -
// declared/defined but never called), the INSPECT_SCENE debug-only mouse/wheel handling, and the
// scene .obj export feature (mExportScene - unreachable in this port, nothing sets it).
class GameDrawable : public QObject, public Drawable
{
   Q_OBJECT

public:

   //! constructor
   GameDrawable(RenderDevice*);

   //! destructor
   virtual ~GameDrawable();

   //! initialize gl context
   void initializeGL();

   //! overwrite paint
   void paintGL();

   //! animate scene
   void animate(float time);

   //! set visibility
   void setVisible(bool visible);

   //! getter for level path
   const QString& getLevelPath() const;

   // event handles

   //! key release event
   void keyPressEvent(QKeyEvent* event);

   //! key press event
   void keyReleaseEvent(QKeyEvent* event);

   //! show player-name overlay + own-position arrow (Tab key)
   void displayPlayerNames();

   //! getter for camera following player
   bool isCameraFollowingPlayer() const;

   //! setter for camera following player
   void setCameraFollowingPlayer(bool value);


public slots:

   void clear();

   void setPlayfieldScale(float xScale = 1.0, float yScale = 1.0f);
   void setPlayfieldSize(int width,int height);

   void createMapItem(MapItem *item);
   void removeMapItem(MapItem *item);
   void destroyMapItem(MapItem *item, float flameCount);
   void addDetonation(int x, int y, int up, int down, int left, int right, float intense);
   void loadLevel(const QString& level);

   void addPlayer(int id, const QString& nick, Constants::Color);
   void removePlayer(int id);
   void setPlayerPosition(int id, float x, float y, float dir);
   void setPlayerSpeed(int id, float dx, float dy, float da);
   void setPlayerId(int id);
   void setMapItemPosition(MapItem*, float x, float y, float z);

   //! extra has been removed
   void extraRemoved(
      int x,
      int y,
      bool destroyed,
      Constants::ExtraType extra,
      int playerId
   );

   //! shake a block
   void shakeBlock(MapItem* item);

   //! a player has been infected
   void playerInfected(
      int id,
      Constants::SkullType,
      int infectorId,
      int extraX,
      int extraY
   );

   //! set zoom factor of camera. default is 1.0f
   void setCameraZoom(float zoom);

signals:

   void keyPressed(QKeyEvent*);
   void keyReleased(QKeyEvent*);
   void levelLoadingStarted(const QString& path);
   void levelLoaded(const QString& path);


private slots:

   //! game state was changed
   void gameStateChanged();

   void playWinAnimation();


private:

   //! getter for player
   PlayerItem* getPlayer(int id) const;

   //! getter for level dimensions
   Constants::Dimension getDimensions(
      float& width,
      float& height
   ) const;

   Mesh* getMesh(MapItem *item) const;
   Mesh* getSkullMesh(MapItem* item) const;
   void updateNeighbouringBlocks(int itemX, int itemY);
   void addBlock(MapItem *item);
   Mesh* createBlock(SceneGraph* scene, Material* mat, float x, float y, float scale);
   Mesh* createBomb(MapItem *item);
   Mesh* createSkull(MapItem* item);
   Mesh* createExtra(ExtraMapItem *extra);
   void removeBlock(MapItem *item);
   Node* createDestruction(SceneGraph *scene, float x, float y, Constants::Direction direction, float flameCount);
   void shakeBoxes(float delta);
   void animateSkulls(float frame);

   void updateBlock(int itemX, int itemY);

   void resetPlayers();
   void deleteLevelData();
   void deleteMesh(Mesh *mesh);

   Level* mLevel;
   SceneGraph* mPlayfield;
   SceneGraph* mLevelSceneGraph;
   SceneGraph* mPlayers;
   Array<Node*> mDestructAnim;
   DetonationManager* mDetonations;
   PlayerDeathEffect* mPlayerDeathEffect;
   GamePlayerNameDisplay* mPlayerNameDisplay;

   float mTime;
   float mTimePrev;

   QSet<MapItem*> mMapItems;
   QList<MapItem*> mStoneList;
   QMap<int,PlayerItem*> mPlayerList;
   QMap<MapItem*,Mesh*> mMeshes;
   QMap<MapItem*,Skull*> mSkullMap;
   QMap<int,Material*> mExtraMaterials;
   QMap<MapItem*,float> mShakingBoxes;

   Material *mStones;
   Material *mBlocks;
   Material *mSkulls;
   Material *mDestruction;
   Material *mExtraFlame;
   Material *mExtraBomb;
   Material *mExtraSpeedup;
   Material *mExtraKick;
   Material *mExtraSkull;
   Material *mBombs;
   Material *mShadowBillboards;
   Material *mShadowBlocks;

   QList<Node*>   mDestructions;
   int     mPlayerId;
   float   mBounce;
   QString mLevelPath;
   float   mPlayfieldScaleX;
   float   mPlayfieldScaleY;
   Map2d<MapItem> mMap;

   float mCameraAnim;
   float mCameraZoom;

   bool mTimeReset;

   //! win animation has been started flag
   bool mWinAnimationStarted;

   //! camera follows player
   bool mCameraFollowsPlayer;

   //! camera shakes on detonation event;
   float mCameraShakeIntensity;
};

#endif // GAMEDRAWABLE_H
