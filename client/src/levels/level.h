#pragma once

#include "tools/array.h"
#include "math/matrix.h"
#include "camerainterpolation.h"

// Qt
#include <QString>
#include <QReadWriteLock>

// forward declarations
class SceneGraph;
class Material;
class Node;

#define MAX_PLAYERS 10


class Level
{
public:

   enum LevelType
   {
      LevelCastle,
      LevelMansion,
      LevelSpace,
      LevelDummy
   };

   Level(LevelType levelType);
   virtual ~Level();

   static QString getLevelDirectoryName(LevelType levelType);
   static QString getLevelName(LevelType levelType);

   void abort();
   bool isAborted() const;
   void load();

   virtual void initialize();
   virtual void draw();
   virtual void drawBackground();
   virtual void animate(float dt);
   virtual void reset();
   virtual void loadData();

   QString path() const;
   SceneGraph* getScene() const;
   SceneGraph* getLevel() const;
   SceneGraph* getPlayers() const;
   Material* getKickExtra() const;
   Material* getSpeedupExtra() const;
   Material* getBombExtra() const;
   Material* getFlameExtra() const;
   Material* getSkullExtra() const;

   const Array<Node*>& getDestructions() const;
   Material* getShadowBillboard() const;
   Material* getShadowBlockBillboard() const;
   Material* getBombMaterial() const;
   Material* getSkullMaterial() const;
   Material* getStoneMaterial() const;
   Material* getBlockMaterial() const;
   Material* getDestructionMaterial() const;

   bool isPlayerMapEmpty() const;
   void resetPlayerPositions();
   void startPositionUpdate(float width, float height, float dt);
   void addPlayerPosition( PlayerInfo* player );
   void endPlayerPositionUpdate();
   Matrix getCameraMatrix(float time);

protected:
   mutable QReadWriteLock mLock;
   LevelType    mLevelType;
   bool         mAborted;

   SceneGraph*  mScene;
   SceneGraph*  mLevel;
   SceneGraph*  mPlayers;
   CameraInterpolation* mCamInterp;
   Array<Node*> mDestructAnim;

   Material*    mStones;
   Material*    mBlocks;
   Material*    mSkulls;
   Material*    mDestruction;
   Material*    mExtraFlame;
   Material*    mExtraBomb;
   Material*    mExtraSpeedup;
   Material*    mExtraKick;
   Material*    mExtraSkull;
   Material*    mOutlines;
   Material*    mBombs;
   Material*    mShadowBillboards;
   Material*    mShadowBlocks;
};
