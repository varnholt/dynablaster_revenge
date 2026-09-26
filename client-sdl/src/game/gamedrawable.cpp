// header
#include "gamedrawable.h"

// framework
#include "gldevice.h"
#include "framework/timerhandler.h"
#include "nodes/camera.h"
#include "nodes/dummy.h"
#include "nodes/mesh.h"
#include "nodes/scenegraph.h"
#include "materials/blockmaterial.h"
#include "tools/filestream.h"

// game
#include "animation/motionmixer.h"
#include "bombermanclient.h"
#include "detonationmanager.h"
#include "gameplayernamedisplay.h"
#include "playerdeatheffect.h"
#include "extra.h"
#include "extramapitem.h"
#include "gamesettings.h"
#include "gamestatemachine.h"
#include "levels/levelfactory.h"
#include "mapitem.h"
#include "playeritem.h"
#include "skull.h"
#include "sdlglobaltime.h"

// std
#include <math.h>
#include <cstdint>

// qt
#include <QKeyEvent>


//-----------------------------------------------------------------------------
/*!
*/
GameDrawable::GameDrawable(RenderDevice* device)
 : QObject(),
   Drawable(device),
   mLevel(nullptr),
   mPlayfield(nullptr),
   mLevelSceneGraph(nullptr),
   mPlayers(nullptr),
   mDestructAnim(),
   mDetonations(nullptr),
   mPlayerDeathEffect(nullptr),
   mPlayerNameDisplay(nullptr),
   mTime(0.0f),
   mTimePrev(0.0f),
   mStones(nullptr),
   mBlocks(nullptr),
   mSkulls(nullptr),
   mDestruction(nullptr),
   mExtraFlame(nullptr),
   mExtraBomb(nullptr),
   mExtraSpeedup(nullptr),
   mExtraKick(nullptr),
   mExtraSkull(nullptr),
   mBombs(nullptr),
   mShadowBillboards(nullptr),
   mShadowBlocks(nullptr),
   mPlayerId(-1),
   mBounce(0.0),
   mPlayfieldScaleX(1.0),
   mPlayfieldScaleY(1.0),
   mCameraAnim(0.0f),
   mCameraZoom(1.0f),
   mTimeReset(true),
   mWinAnimationStarted(false),
   mCameraFollowsPlayer(true),
   mCameraShakeIntensity(1.0)
{
   // load animations
   MotionMixer::addAnimation("player-idle.hjb");
   MotionMixer::addAnimation("player-walk.hjb");
   MotionMixer::addAnimation("player-run.hjb");
   MotionMixer::addAnimation("player-die1.hjb");
   MotionMixer::addAnimation("player-win.hjb");

   connect(
      GameStateMachine::getInstance(),
      SIGNAL(stateChanged()),
      this,
      SLOT(gameStateChanged())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
GameDrawable::~GameDrawable()
{
   MotionMixer::cleanup();

   deleteLevelData();
   delete mDetonations;
   delete mPlayerDeathEffect;
   delete mPlayerNameDisplay;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::deleteLevelData()
{
   clear();
   mDestructions.clear();

   if (mLevel)
   {
      delete mLevel;
      mLevel= nullptr;
   }

   mPlayfield= nullptr;
   mLevelSceneGraph= nullptr;
   mPlayers= nullptr;

   for (int i=0;i<mDestructAnim.size(); i++)
      delete mDestructAnim[i];
}


//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void GameDrawable::setVisible(bool visible)
{
   if (visible)
   {
      // reload camera settings from gameplay settings
      GameSettings::GameplaySettings* settings =
         GameSettings::getInstance()->getGameplaySettings();

      mCameraFollowsPlayer = settings->isCameraFollowingPlayer();
      mCameraShakeIntensity = settings->getCameraShakeIntensity();

      // reset
      mTimeReset = true;
      mCameraAnim= 0.0f;

      resetPlayers();

      if (mLevel)
         mLevel->reset();
   }
   else
   {
      mWinAnimationStarted = false;
   }

   Drawable::setVisible(visible);
}


//-----------------------------------------------------------------------------
/*!
   \param event key press event
*/
void GameDrawable::keyPressEvent(QKeyEvent* event)
{
   if (event->key() == Qt::Key_Tab)
   {
      displayPlayerNames();
   }

   emit keyPressed(event);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::displayPlayerNames()
{
   mPlayerNameDisplay->start();
}


//-----------------------------------------------------------------------------
/*!
   \param event key release event
*/
void GameDrawable::keyReleaseEvent(QKeyEvent* event)
{
   emit keyReleased(event);
}


//-----------------------------------------------------------------------------
/*!
*/
const QString &GameDrawable::getLevelPath() const
{
   return mLevelPath;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::setPlayerId(int id)
{
   mPlayerId= id;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::clear()
{
   foreach (MapItem *item, mMapItems)
      removeMapItem(item);

   mDetonations->clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::deleteMesh(Mesh *mesh)
{
   if (mPlayfield)
   {
      int count= mPlayfield->getMaterialCount();
      for (int i=0; i<count; i++)
      {
         Material* mat= mPlayfield->getMaterial(i);
         if (mat)
            mat->removeMesh(mesh);
      }
   }
   delete mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::setPlayfieldScale(float scaleX, float scaleY)
{
   mPlayfieldScaleX = 1.0f / scaleX;
   mPlayfieldScaleY = 1.0f / scaleY;

   if (mPlayfield)
   {
      mPlayfield->setGlobalTransform(
         Matrix::scale(
            mPlayfieldScaleX,
            mPlayfieldScaleY,
            mPlayfieldScaleX
         )
      );
   }

   if (mPlayers)
   {
      mPlayers->setGlobalTransform(
         Matrix::scale(
            mPlayfieldScaleX,
            mPlayfieldScaleY,
            mPlayfieldScaleX
         )
      );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::setPlayfieldSize(int width, int height)
{
   mMap.init( width, height );
}


//-----------------------------------------------------------------------------
/*!
   Real LevelLoadingThread (QThread + background GL resource creation) deliberately not ported -
   see project memory, Phase 5: GL resource creation off the main thread is a real correctness
   risk this port has no infrastructure for. Loads synchronously instead, same as the already-
   proven harness castle-level demo (main_harness.cpp).
*/
void GameDrawable::loadLevel(const QString& levelPath)
{
   qDebug("GameDrawable::loadLevel: loading %s", qPrintable(levelPath));

   emit levelLoadingStarted(levelPath);

   mMap.clear();

   resetPlayers();

   deleteLevelData();

   mLevelPath = QString("data/%1").arg(levelPath);

   Level* level = LevelFactory::getLevelInstance(mLevelPath);
   level->load();

   mLevel = level;

   mLevelSceneGraph= mLevel->getLevel();
   mPlayfield= mLevel->getScene();
   mPlayers= mLevel->getPlayers();

   mPlayfield->setGlobalTransform(
      Matrix::scale(
         mPlayfieldScaleX,
         mPlayfieldScaleY,
         mPlayfieldScaleX
      )
   );

   mPlayers->setGlobalTransform(
      Matrix::scale(
         mPlayfieldScaleX,
         mPlayfieldScaleY,
         mPlayfieldScaleX
      )
   );

   mExtraFlame   = mLevel->getFlameExtra();
   mExtraBomb    = mLevel->getBombExtra();
   mExtraSpeedup = mLevel->getSpeedupExtra();
   mExtraKick    = mLevel->getKickExtra();
   mExtraSkull   = mLevel->getSkullExtra();

   mExtraMaterials.insert(Constants::ExtraFlame,   mExtraFlame);
   mExtraMaterials.insert(Constants::ExtraBomb,    mExtraBomb);
   mExtraMaterials.insert(Constants::ExtraSpeedup, mExtraSpeedup);
   mExtraMaterials.insert(Constants::ExtraKick,    mExtraKick);
   mExtraMaterials.insert(Constants::ExtraSkull,   mExtraSkull);

   mDestructAnim= mLevel->getDestructions();

   mShadowBillboards= mLevel->getShadowBillboard();
   mShadowBlocks= mLevel->getShadowBlockBillboard();
   mBombs= mLevel->getBombMaterial();
   mStones= mLevel->getStoneMaterial();
   mBlocks= mLevel->getBlockMaterial();
   mSkulls= mLevel->getSkullMaterial();

   mDestruction= mLevel->getDestructionMaterial();
   mDestruction->clear();

   // we're done
   emit levelLoaded(mLevelPath);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::initializeGL()
{
   mDetonations= new DetonationManager();
   mDetonations->init();

   mPlayerDeathEffect = new PlayerDeathEffect();

   mPlayerNameDisplay = new GamePlayerNameDisplay(this);
   mPlayerNameDisplay->initialize();
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::getMesh(MapItem* item) const
{
   QMap<MapItem*,Mesh*>::ConstIterator it= mMeshes.constFind(item);
   if (it != mMeshes.constEnd())
      return it.value();
   else
      return nullptr;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::getSkullMesh(MapItem* item) const
{
   QMap<MapItem*,Skull*>::ConstIterator it= mSkullMap.constFind(item);
   if (it != mSkullMap.constEnd())
      return it.value();
   else
      return nullptr;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::updateBlock(int itemX, int itemY)
{
   int flags= 0;
   int bitpos= 1;

   MapItem* item= mMap.get(itemX, itemY);
   Mesh* mesh= getMesh( item );
   if (mesh)
   {
      for (int y=itemY-1; y<=itemY+1; y++)
      {
         for (int x=itemX-1; x<=itemX+1; x++)
         {
            MapItem* item= mMap.get(x,y);
            if ( getMesh(item) )
               flags |= bitpos;
            bitpos<<=1;
         }
      }
      mesh->setRenderFlags(static_cast<uint32_t>(flags));
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::updateNeighbouringBlocks(int itemX, int itemY)
{
   for (int y=itemY-1; y<=itemY+1; y++)
      for (int x=itemX-1; x<=itemX+1; x++)
         updateBlock(x, y);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::addBlock(MapItem *item)
{
   mMap.set(item->getX(), item->getY(), item);
   updateNeighbouringBlocks(item->getX(), item->getY());
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::removeBlock(MapItem *item)
{
   mMap.set(item->getX(), item->getY(), nullptr);
   updateNeighbouringBlocks(item->getX(), item->getY());
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::createBlock(SceneGraph* scene, Material* mat, float x, float y, float size)
{
   Matrix pos,scale;
   pos.identity();
   pos.translate( Vector(x+0.5f, -y-0.5f) );
   scale= Matrix::scale(size, size, size);

   Mesh *ref= dynamic_cast<Mesh*>(scene->getNode("Block"));
   Mesh *mesh= new Mesh(*ref);
   mesh->setTransform(scale * pos);
   mPlayfield->addNode(mesh);
   mat->addMesh(mesh);
   mShadowBlocks->addMesh(mesh);

   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::createExtra(ExtraMapItem *extra)
{
   Mesh* mesh = nullptr;
   Mesh *extraMesh= dynamic_cast<Mesh*>(mPlayfield->getNode("Extra"));
   mesh= new Extra(extra->getExtraType(), extraMesh->getPart(0), extra->getX(), extra->getY());
   mExtraMaterials[extra->getExtraType()]->addMesh(mesh);
   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::createBomb(MapItem *item)
{
   Mesh* mesh = nullptr;

   Mesh *obj= dynamic_cast<Mesh*>(mPlayfield->getNode("lunte"));

   if (obj)
   {
      mesh= new Mesh(*obj);
      mesh->setAnimationFrame(mTime);

      Matrix translationMatrix;
      Vector itemPosition =
         Vector(item->getX()+0.4f, -item->getY()-0.5f, 0.0f);
      translationMatrix.identity();
      translationMatrix.translate( itemPosition );

      mesh->setTransform(translationMatrix);

      mPlayfield->addNode(mesh);
      mBombs->addMesh(mesh);
      mShadowBillboards->addMesh(mesh);
   }

   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh *GameDrawable::createSkull(MapItem *item)
{
   Mesh* skullMesh = dynamic_cast<Mesh*>(mPlayfield->getNode("skull"));

   Skull* mesh = new Skull(skullMesh, item->getX(), item->getY());

   mesh->setTransform(skullMesh->getTransform() * mesh->getTransform());

   mPlayfield->addNode(mesh);
   mSkulls->addMesh(mesh);
   mSkullMap.insert(item, mesh);

   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::animateSkulls(float time)
{
   // annoying 62.5 multiplier
   time *= 0.016f;

   foreach(Skull* skull, mSkullMap)
   {
      // frame time in seconds, relative to start frame, tick-scaled - see original comment in
      // client/src/game/gamedrawable.cpp for the derivation of the 3200.0f/19200.0 constants.
      float frame = time;
      frame -= skull->getStartTime();
      frame *= 3200.0f;

      float f = static_cast<float>(fmod(static_cast<double>(frame), 19200.0));

      Mesh* ref = skull->getReference();
      ref->transform(f);

      Matrix transform = ref->getTransform();
      Matrix translate = skull->getTranslation();

      skull->setTransform(transform * translate);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::shakeBlock(MapItem* item)
{
   QMap<MapItem*,Mesh*>::ConstIterator it= mMeshes.constFind( item );
   if (it != mMeshes.constEnd())
   {
      mShakingBoxes.insert( item, 1.0f );
   }
}


//-----------------------------------------------------------------------------
/*!
   Real per-skull-type visual flourishes (mushroom screen filter, invincible-ribbon spawn,
   invisibility) are deferred (see header comment) - the actual infection *state* still comes
   through fully via BombermanClient's packet handling regardless of what renders here.
*/
void GameDrawable::playerInfected(
   int /*id*/,
   Constants::SkullType /*skullType*/,
   int /*infectorId*/,
   int /*extraX*/,
   int /*extraY*/
)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::createMapItem(MapItem *item)
{
   if (!mPlayfield)
      return;

   if (!mMapItems.contains(item))
   {
      Mesh *mesh= nullptr;

      switch (item->getType())
      {
         case MapItem::Stone:
         {
            mesh= createBlock(mPlayfield, mStones, item->getX(), item->getY(), 0.8f);
            mStoneList.append(item);
            break;
         }

         case MapItem::Block:
         {
            mesh= createBlock(mPlayfield, mBlocks, item->getX(), item->getY(), 0.9f);
            break;
         }

         case MapItem::Bomb:
         {
            mesh = createBomb(item);
            break;
         }

         case MapItem::Extra:
         {
            ExtraMapItem *extra= dynamic_cast<ExtraMapItem*>(item);

            if (extra->getExtraType() == Constants::ExtraSkull)
            {
               mesh = createSkull(item);
               mesh = nullptr; // skulls are tracked in mSkullMap, not mMeshes
            }
            else
               mesh = createExtra(extra);

            break;
         }

         default:
            item= nullptr;
            break;
      }

      if (item)
      {
         mMapItems.insert(item);

         if (mesh)
         {
            mMeshes.insert(item, mesh);

            if (
                  item->getType() == MapItem::Stone
               || item->getType() == MapItem::Block
            )
            {
               addBlock(item);
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::removeMapItem(MapItem *item)
{
   std::unordered_set<MapItem*>::iterator it= mMapItems.find(item);

   if (it != mMapItems.end())
   {
      mStoneList.removeAll(item);

      mShakingBoxes.remove(item);

      // get associated mesh
      QMap<MapItem*,Mesh*>::Iterator m= mMeshes.find(item);

      if (m != mMeshes.end())
      {
         if (
               item->getType() == MapItem::Stone
            || item->getType() == MapItem::Block
         )
         {
            removeBlock(item);
         }

         Mesh *mesh= m.value();

         mStones->removeMesh(mesh);
         mShadowBlocks->removeMesh(mesh);
         mBlocks->removeMesh(mesh);
         mExtraBomb->removeMesh(mesh);
         mExtraFlame->removeMesh(mesh);
         mExtraSpeedup->removeMesh(mesh);
         mExtraKick->removeMesh(mesh);
         mBombs->removeMesh(mesh);
         mShadowBillboards->removeMesh(mesh);
         deleteMesh(mesh);
         mMeshes.erase(m);
      }
      else
      {
         QMap<MapItem*,Skull*>::Iterator si= mSkullMap.find(item);

         if (si != mSkullMap.end())
         {
            Mesh *skullMesh= si.value();

            mSkulls->removeMesh(skullMesh);
            deleteMesh(skullMesh);
            mSkullMap.erase(si);
         }
      }

      mMapItems.erase(it);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
Node* GameDrawable::createDestruction(SceneGraph *scene, float x, float y, Constants::Direction direction, float flameCount)
{
   Dummy *dummy= nullptr;

   // create destruction animation
   Node *root= nullptr;
   if (flameCount < 3)
      root= mDestructAnim[0];
   else if (flameCount < 5)
      root= mDestructAnim[1];
   else if (flameCount < 8)
      root= mDestructAnim[2];
   else
      root= mDestructAnim[3];

   if (root)
   {
      int rot= 0;
      switch (direction)
      {
         case Constants::DirectionUp:    rot= 1; break;
         case Constants::DirectionDown:  rot= 3; break;
         case Constants::DirectionLeft:  rot= 2; break;
         case Constants::DirectionRight: rot= 0; break;
         default: break;
      }
      Node* destruct= root->getChild(rot);

      dummy= new Dummy(scene);
      dummy->setUserTransformable(true);
      Matrix pos;
      pos= Matrix::rotateZ( rot * static_cast<float>(M_PI) * 0.5f );
      pos.translate( Vector(x+0.5f, -y-0.5f) );
      dummy->setUserTransformable(true);
      Matrix scale= Matrix::scale(0.8f, 0.8f, 0.8f);
      dummy->setTransform(scale * pos);

      for (int i=0; i<destruct->getChildCount(); i++)
      {
         Node* child= destruct->getChild(i);
         if (child->id() == Node::idMesh)
         {
            Mesh *ref= dynamic_cast<Mesh*>(child);
            Mesh *mesh= new Mesh(*ref, dummy);
            mesh->setUserTransformable(false);
            mDestruction->addMesh(mesh);
            mesh->setFrame(0.0f);
         }
      }
   }

   return dummy;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::destroyMapItem(MapItem *item, float flameCount)
{
   std::unordered_set<MapItem*>::iterator it= mMapItems.find(item);
   if (it!=mMapItems.end())
   {
      if (item->getType() == MapItem::Stone)
      {
         Node* dummy=
            createDestruction(
                mPlayfield,
                item->getX(),
                item->getY(),
                item->getDestroyDirection(),
                flameCount
            );

         if (dummy)
            mDestructions.append(dummy);
      }
   }

   removeMapItem(item);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::addDetonation(int x, int y, int up, int down, int left, int right, float intense)
{
   if (mDetonations)
   {
      intense= sqrt(intense)*0.15f;
      if (intense>0.5f)
         intense= 0.5f;
      if (intense > mBounce)
         mBounce= intense;
      mDetonations->addDetonation(x,y,up,down,left,right);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param item item to update
   \param x x position
   \param y y position
   \param z z position
*/
void GameDrawable::setMapItemPosition(
   MapItem * item,
   float x,
   float y,
   float z
)
{
   QMap<MapItem*,Mesh*>::const_iterator iter = mMeshes.constFind(item);

   if (iter != mMeshes.constEnd())
   {
      float width = 0.0f;
      float height = 0.0f;

      getDimensions(width, height);

      if (x < -ITEM_INTERPOLATION_EPS)
         x = -ITEM_INTERPOLATION_EPS;
      if (x > width + ITEM_INTERPOLATION_EPS)
         x = width + ITEM_INTERPOLATION_EPS;

      if (y < -ITEM_INTERPOLATION_EPS)
         y = -ITEM_INTERPOLATION_EPS;
      if (y > height + ITEM_INTERPOLATION_EPS)
         y = height + ITEM_INTERPOLATION_EPS;

      Mesh* mesh = iter.value();

      Matrix pos;
      pos.identity();

      pos.translate(
         Vector(
            x + 0.5f,
           -y - 0.5f,
            z
         )
      );

      mesh->setTransform(pos);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param x x position where extra has been removed
   \param y y position where extra has been removed
   \param destroyed \c true if extra was destroyed
   \param playerId if of player who picked the extra up

   Real "extra destroyed" burst / "extra not collected in time" star-taler animations
   (ExtraAnimation / StarTalersFactory) are deferred - see header comment.
*/
void GameDrawable::extraRemoved(
   int /*x*/,
   int /*y*/,
   bool /*destroyed*/,
   Constants::ExtraType /*extra*/,
   int playerId
)
{
   if (playerId != -1)
   {
      PlayerItem* player = getPlayer(playerId);

      if (player)
         player->setFlash(1.0f);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::setPlayerPosition(int id, float x, float y, float angle)
{
   float width = 1.0f;
   float height = 1.0f;

   getDimensions(width, height);

   PlayerItem* player= getPlayer(id);

   if (player)
   {
      player->setRotation(angle + static_cast<float>(M_PI) * 0.5f);
      player->setPosition(x, y);
   }
}


//-----------------------------------------------------------------------------
/*!
   set zoom factor
   \param zoom zoom factor
*/
void GameDrawable::setCameraZoom(float zoom)
{
   mCameraZoom= zoom;
}


//-----------------------------------------------------------------------------
/*!
   \param width dimension width
   \param height dimension height
   \return dimensions enum
*/
Constants::Dimension GameDrawable::getDimensions(
   float& width,
   float& height
) const
{
   GameInformation* info =
      BombermanClient::getInstance()->getCurrentGameInformation();

   if (!info)
   {
      width= 0.0f;
      height= 0.0f;
      return Constants::DimensionInvalid;
   }

   Constants::Dimension dimensions = info->getMapDimensions();

   switch (dimensions)
   {
      case Constants::Dimension13x11:
         width = 13.0f;
         height = 11.0f;
         break;

      case Constants::Dimension19x17:
         width = 19.0f;
         height = 17.0f;
         break;

      case Constants::Dimension25x21:
         width = 25.0f;
         height = 21.0f;
         break;

      default:
         break;
   }

   return dimensions;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::setPlayerSpeed(int id, float dx, float dy, float /*da*/)
{
   PlayerItem *player= getPlayer(id);
   if (player)
   {
      player->setSpeed( sqrt(dx*dx+dy*dy) );
   }
}


//-----------------------------------------------------------------------------
/*!
*/
PlayerItem* GameDrawable::getPlayer(int id) const
{
   QMap<int,PlayerItem*>::ConstIterator it= mPlayerList.constFind(id);
   if (it != mPlayerList.constEnd())
      return it.value();
   else
      return nullptr;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::addPlayer(int id, const QString& nick, Constants::Color color)
{
   PlayerItem *player= getPlayer(id);
   if (player)
   {
      qWarning("GameDrawable::addPlayer: player already exists");
      player->setKilled(false);
      return;
   }

   player= new PlayerItem(id, nick, color);
   mPlayerList.insert(id, player);

   Mesh *mesh= MotionMixer::getMesh("bomberman");
   if (!mesh)
   {
      qWarning("GameDrawable::addPlayer: mesh not found");
   }

   Mesh *p= new Mesh(mPlayers);
   p->copy(*mesh);
   MotionMixer *mixer= new MotionMixer();
   p->setMotionMixer(mixer);
   p->setVisible(true);

   player->setMesh(p);
   Material* playerMaterial= mPlayers->getMaterial(static_cast<int32_t>(color-1));
   playerMaterial->addMesh(p);
   player->setMaterial( playerMaterial );

   mShadowBillboards->addMesh(p);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::removePlayer(int id)
{
   PlayerItem *player= getPlayer(id);

   if (player)
   {
      player->kill();
   }

   // check for survivors
   if (mPlayerList.size() > 1)
   {
      int alive= 0;
      foreach (PlayerItem* p, mPlayerList)
      {
         if (!p->isKilled())
            alive++;
      }

      if (alive == 1)
      {
         foreach (PlayerItem* p, mPlayerList)
         {
            if (!p->isKilled())
            {
               if (!mWinAnimationStarted)
               {
                  mWinAnimationStarted = true;
                  TimerHandler::singleShot(2000, this, SLOT(playWinAnimation()));
               }
            }
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::playWinAnimation()
{
   foreach (PlayerItem* player, mPlayerList)
   {
      if (!player->isKilled())
      {
         player->win();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::gameStateChanged()
{
   switch (GameStateMachine::getInstance()->getState())
   {
      case Constants::GameStopped:
         mWinAnimationStarted = false;
         break;

      default:
         break;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::animate(float time)
{
   if (mDetonations)
      mDetonations->update(time / 62.5f);

   if (mTimeReset)
   {
      mTime = time;
      mTimeReset = false;
   }

   float delta= time - mTime;
   mTime= time;

   if (mPlayerDeathEffect)
      mPlayerDeathEffect->animate(delta);

   mCameraAnim+=delta*60.0f;

   if (mBounce > delta*0.01f)
      mBounce-=delta*0.01f;
   else
      mBounce= 0.0f;

   // rotational rotation is rotating:
   for (QMap<MapItem*,Mesh*>::ConstIterator it= mMeshes.constBegin(); it != mMeshes.constEnd(); it++)
   {
      MapItem *item= it.key();
      switch (item->getType())
      {
         case MapItem::Extra:
         {
            Extra *extra= dynamic_cast<Extra*>(it.value());
            extra->animate(time);
         }
         break;

         case MapItem::Bomb:
         {
            Mesh *mesh= it.value();
            float t= time * 0.1f + mesh->getAnimationFrame();

            Vector pos= mesh->getTransform().translation();

            float sx= 1.0f + sin(t)*0.2f;
            float sy= 1.0f - sin(t)*0.3f;

            Matrix mat= Matrix::scale(sx,sx,sy);
            mat.translate(pos);
            mesh->setTransform(mat);
         }
         break;

         default:
            break;
      }
   }

   foreach (PlayerItem *player, mPlayerList)
   {
      player->animate(time, delta);
   }

   // update destructions and remove if end of animation was reached
   for (QList<Node*>::Iterator it= mDestructions.begin(); it!=mDestructions.end(); )
   {
      Node* destr= *it;
      bool remove= false;
      for (int i=0; i<destr->getChildCount(); i++)
      {
         Mesh *mesh= dynamic_cast<Mesh*>(destr->getChild(i));
         float frame= mesh->getFrame() + delta * 30.0f;
         if (frame > 4000)
            remove= true;
         mesh->setFrame(frame);
      }

      if (remove)
      {
         it= mDestructions.erase(it);
         for (int i=0; i<destr->getChildCount(); i++)
         {
            Mesh *mesh= dynamic_cast<Mesh*>(destr->getChild(i));
            mDestruction->removeMesh(mesh);
            deleteMesh(mesh);
         }
         delete destr;
      }
      else
         it++;
   }

   if (mLevel)
      mLevel->animate(delta);

   animateSkulls(time);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::shakeBoxes(float delta)
{
   QMap<MapItem*,float>::Iterator it;
   for (it= mShakingBoxes.begin(); it!=mShakingBoxes.end();)
   {
      float time= it.value();
      time-=delta;
      if (time<0.0f) time= 0.0f;
      float intense= time*time;
      MapItem* item= it.key();
      Mesh* mesh= getMesh(item);
      if (mesh)
      {
         const Matrix& cur= mesh->getTransform();

         Matrix scale;
         float x= 0.8f + sin((1.0f-time) * 14.0f) * 0.1f * intense;
         float y= 0.8f - sin((1.0f-time) * 12.0f) * 0.1f * intense;
         float z= 0.6f + cos((1.0f-time) * 16.0f) * 0.2f * intense + 0.2f *(1.0f - intense);

         scale= Matrix::scale(x, y, z);
         scale.translate( cur.translation() );
         mesh->setTransform(scale);
      }

      if (!mesh || time <= 0.0f)
         it= mShakingBoxes.erase(it);
      else
      {
         *it= time;
         it++;
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   Renders straight to the default framebuffer - the real GameDrawable renders to its own
   offscreen FrameBuffer via MainDrawable::getInstance()->getRenderBuffer() and blits it back with
   BlendQuad, but that indirection only existed to let the mushroom-effect screen filter and the
   invisibility material read the frame back as a texture, both deferred (see header comment). No
   remaining reason to render offscreen first.
*/
void GameDrawable::paintGL()
{
   float time= GlobalTime::Instance()->getTime();

   float dt = mTime - mTimePrev;

   shakeBoxes(dt*0.015f);

   float width, height;
   Constants::Dimension dimensions;
   dimensions = getDimensions(width, height);

   float bounceX, bounceY;

   bounceX= mCameraShakeIntensity * 0.5f * sin(time * 71.0f) * mBounce;
   bounceY= mCameraShakeIntensity * 0.5f * sin(time * 113.0f) * mBounce;

   if (dimensions == Constants::Dimension19x17)
   {
       bounceX *= 0.66f;
       bounceY *= 0.66f;
   }

   Matrix view;

   if (mLevel)
   {
      mLevel->startPositionUpdate(width, height, dt);

      if (mCameraFollowsPlayer)
      {
         // real HeadlessIntegration (bot/multi-instance camera) branch deferred - always follows
         // this client's own player, matching the single-player-on-this-pc path.
         PlayerInfo* player= BombermanClient::getInstance()->getCurrentPlayerInfo();
         if (player)
            mLevel->addPlayerPosition( player );

         // if no players have been added to the camera interpolation; then add all players
         if (mLevel->isPlayerMapEmpty())
         {
            const QMap<int, PlayerInfo*>* players =
               BombermanClient::getInstance()->getPlayerInfoMap();

            foreach (PlayerInfo* p, *players)
               mLevel->addPlayerPosition( p );
         }
      }

      mLevel->endPlayerPositionUpdate();

      view= mLevel->getCameraMatrix(mCameraAnim, mCameraZoom);
   }

   Matrix shake= Matrix::position(bounceX, bounceY, 0.0f) * view;

   // render scene
   if (mLevelSceneGraph)
   {
      mLevelSceneGraph->render(mCameraAnim, shake);
   }

   if (mPlayfield)
   {
      mPlayfield->render(0.0, shake);
   }

   if (mPlayers)
   {
      mPlayers->render(0.0, shake);
   }

   mDetonations->render();

   // start flow fields when a player got killed (and the kill anim is over) - matches the
   // original's own "playerMesh->getFrame() > 10000.0f" convention: PlayerItem::animate() only
   // wraps the frame counter back to 0 while alive, so it climbs unbounded once mKilled is set,
   // naturally crossing 10000 once the one-shot death animation has long finished playing.
   foreach (PlayerItem* player, mPlayerList)
   {
      if (player->isKilled())
      {
         Mesh* playerMesh = player->getMesh();

         if (playerMesh->getFrame() > 10000.0f)
         {
            Geometry* playerGeometry = playerMesh->getPart(0);

            if (playerGeometry->isVisible())
            {
               Material* material = mPlayers->getMaterial(static_cast<int32_t>(player->getColor()) - 1);
               mPlayerDeathEffect->add(material);
               playerGeometry->setVisible(false);
            }
         }
      }
   }

   mPlayerDeathEffect->render();

   // draw player names
   if (mPlayerNameDisplay->isActive())
   {
      mPlayerNameDisplay->setPlayerData(mPlayerList);
      mPlayerNameDisplay->draw();
   }

   // draw level specific stuff
   if (mLevel)
      mLevel->draw();

   mTimePrev = mTime;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::resetPlayers()
{
   if (mLevel)
      mLevel->resetPlayerPositions();

   QMap<int,PlayerItem*>::Iterator it= mPlayerList.begin();
   while (it != mPlayerList.end())
   {
      PlayerItem* player= *it;
      it= mPlayerList.erase(it);

      int color= static_cast<int32_t>(player->getColor());
      Mesh *mesh= player->getMesh();
      Material* playerMaterial= mPlayers->getMaterial(static_cast<int32_t>(color-1));
      playerMaterial->removeMesh(mesh);
      mShadowBillboards->removeMesh(mesh);

      delete player;
   }
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if camera follows player
*/
bool GameDrawable::isCameraFollowingPlayer() const
{
   return mCameraFollowsPlayer;
}


//-----------------------------------------------------------------------------
/*!
   \param value camera follows player flag
*/
void GameDrawable::setCameraFollowingPlayer(bool value)
{
   mCameraFollowsPlayer = value;
}
