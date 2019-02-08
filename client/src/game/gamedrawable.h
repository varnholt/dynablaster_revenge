#ifndef GAMEDRAWABLE_H
#define GAMEDRAWABLE_H

// Qt
#include <QObject>
#include <QSet>
#include <QString>
#include <QMap>
#include <QQueue>
#include <QPoint>
#include <QVector2D>

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
class AnimatedMesh;
class BlendQuad;
class DetonationManager;
class ExtraAnimation;
class ExtraMapItem;
class ExtraRevealAnimation;
class FuseParticleEmitter;
class GamePlaybackDisplay;
class GamePlayerNameDisplay;
class InvisiblePlayerEffect;
class Level;
class MapItem;
class Material;
class MushroomAnimation;
class Mesh;
class MotionMixer;
class Node;
class PlayerItem;
class QImage;
class QFocusEvent;
class QKeyEvent;
class RenderDevice;
class RibbonAnimationFactory;
class SceneGraph;
class ShroomFilter;
class Skull;
class StarTalersFactory;
class SnowAnimation;
class PlayerDeathEffect;
class PlayerInfectedEffect;
class PlayerInvincibleEffect;

class GameDrawable : public QObject, public Drawable
{
   Q_OBJECT

public:

   struct MaterialMesh
   {
      Material *material;
      Mesh *mesh;
   };

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

   void setExportScene(bool exp);

   // event handles

   //! key release event
   void keyPressEvent(QKeyEvent* event);

   //! key press event
   void keyReleaseEvent(QKeyEvent* event);

   //! event filter
   bool eventFilter(QObject* obj, QEvent* event);

   //! mouse press event
   virtual void mousePressEvent(
      int x,
      int y,
      Qt::MouseButton button = Qt::LeftButton
   );

   //! mouse move event
   virtual void mouseMoveEvent(int x, int y);

   //! mouse release event
   virtual void mouseReleaseEvent(QMouseEvent* event);

   //! mouse wheel event
   virtual void wheelEvent(QWheelEvent *);

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

   //! display player names
   void displayPlayerNames();

   //! set zoom factor of camera. default is 1.0f
   void setCameraZoom(float zoom);

signals:

   void keyPressed(QKeyEvent*);
   void keyReleased(QKeyEvent*);
   void releaseAllKeys();
   void levelLoadingStarted(const QString& path);
   void levelLoaded(const QString& path);
   void requestInit(Drawable* drawable);


private slots:

   void levelFinished(const QString& levelPath);
   void playWinAnimation();

   //! game state was changed
   void gameStateChanged();
   // void cleanupInfections();


private:

   //! getter for player
   PlayerItem* getPlayer(int id) const;

   //! load an explosion
   AnimatedMesh* loadExplosion(SceneGraph *scene, const char *filename);

   //! draw a test quad
   void drawTestQuad();

   void drawExtraAnimations(float dt);
   void drawExtraRevealAnimations(float dt);
   void deleteMesh(Mesh *mesh);

   //! getter for level dimensions
   Constants::Dimension getDimensions(
      float& width,
      float& height
   ) const;

   void drawBoundingRect();
   void drawBoundingBox(Vector min, Vector max);
   void playerBoundingRect(Material* material, Vector& min, Vector& max, const Matrix& projMat);
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
   void clearFuseParticleEmitters();

   Level* mLevel;
   SceneGraph* mPlayfield;
   SceneGraph* mLevelSceneGraph;
   SceneGraph* mPlayers;
   Array<Node*> mDestructAnim;
   DetonationManager* mDetonations;

   float mTime;
   float mTimePrev;

   QSet<MapItem*> mMapItems;
   QList<MapItem*> mStoneList; // debug only!
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
   Material *mOutlines;
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

   QMap<MapItem*, FuseParticleEmitter*> mEmitters;

   QList<ExtraAnimation*> mExtraAnimations;

   QList<ExtraRevealAnimation*> mExtraRevealAnimations;

   float mCameraAnim;
   float mCameraZoom;

   bool mTimeReset;

   bool mFlowFieldAnimDebug;

   //! win animation has been started flag
   bool mWinAnimationStarted;

   //! show player names on game startup
   GamePlayerNameDisplay* mPlayerNameDisplay;

   //! text display for game playback
   GamePlaybackDisplay* mGamePlaybackDisplay;

   //! camera follows player
   bool mCameraFollowsPlayer;

   //! camera shakes on detonation event;
   float mCameraShakeIntensity;

   //! identity matrix for disabled camera rotation
   Matrix mIdentityMatrix;

   //! shroom effect
   ShroomFilter* mShroomFilter;

   //! blend quads
   BlendQuad* mBlendQuad;

   //! mushroom animation
   MushroomAnimation* mMushroomAnimation;

   //! player invisibility
   InvisiblePlayerEffect* mInvisiblePlayers;

   //! player death effect
   PlayerDeathEffect* mPlayerDeathEffect;

   //! player invincible effect
   PlayerInfectedEffect* mPlayerInfectedEffect;

   //! player invincible effect
   PlayerInvincibleEffect* mPlayerInvincibleEffect;

   //! ribbon effect
   RibbonAnimationFactory* mRibbonAnimationFactory;

   //! star talers effect
   StarTalersFactory* mStarTalersFactory;

#ifdef INSPECT_SCENE
   bool mMouseButtons[2];
   int mMouseX;
   int mMouseY;
   float mZoom;
   float mRotX;
   float mRotY;
#endif

   bool mExportScene;
};

#endif // GAMEDRAWABLE_H
