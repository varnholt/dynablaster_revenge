// header
#include "gamedrawable.h"

// effects
#include "effects/lensflare/lensflarefactory.h"
#include "effects/ribbons/ribbonanimationfactory.h"
#include "effects/spherefragments/blendquad.h"
#include "effects/startalers/startalersfactory.h"
#include "mushroomanimation.h"

// framework
#include "gldevice.h"
#include "framework/framebuffer.h"
#include "framework/timerhandler.h"
#include "nodes/camera.h"
#include "nodes/dummy.h"
#include "nodes/mesh.h"
#include "nodes/scenegraph.h"
#include "materials/texturematerial.h"
#include "materials/blockmaterial.h"
#include "materials/environmenttexturematerial.h"
#include "materials/outlining.h"
#include "materials/bombexplosionshader.h"
#include "materials/extramapping.h"
#include "materials/shadowbillboard.h"
#include "materials/invisibilitymaterial.h"
#include "tools/chunk.h"
#include "tools/filestream.h"
#include "tools/profiling.h"

// game
#include "animation/motionmixer.h"
#include "bombermanclient.h"
#include "detonationmanager.h"
#include "extra.h"
#include "extraanimation.h"
#include "extramapitem.h"
#include "extrarevealanimation.h"
#include "gameplaybackdisplay.h"
#include "gameplayernamedisplay.h"
#include "gamesettings.h"
#include "gamestatemachine.h"
#include "headlessintegration.h"
#include "invisibleplayereffect.h"
#include "levels/levelfactory.h"
#include "levels/levelloadingthread.h"
#include "maindrawable.h"
#include "mapitem.h"
#include "playeritem.h"
#include "skull.h"
#include "videodebugging.h"

// particle
#include "fuseparticle.h"
#include "fuseparticleemitter.h"

// postproduction
#include "postproduction/shroomfilter.h"

// flowfield animation
#include "playerdeatheffect.h"
#include "playerinfectedeffect.h"
#include "playerinvincibleeffect.h"

// std
#include <math.h>

// qt
#include <QApplication>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>


//-----------------------------------------------------------------------------
/*!
*/
GameDrawable::GameDrawable(RenderDevice* device)
 : QObject(),
   Drawable(device),
   mLevel(0),
   mPlayfield(0),
   mLevelSceneGraph(0),
   mPlayers(0),
   mDestructAnim(),
   mDetonations(0),
   mTime(0.0f),
   mTimePrev(0.0f),
   mStones(0),
   mBlocks(0),
   mSkulls(0),
   mDestruction(0),
   mExtraFlame(0),
   mExtraBomb(0),
   mExtraSpeedup(0),
   mExtraKick(0),
   mOutlines(0),
   mBombs(0),
   mShadowBillboards(0),
   mShadowBlocks(0),
   mPlayerId(-1),
   mBounce(0.0),
   mPlayfieldScaleX(1.0),
   mPlayfieldScaleY(1.0),
   mCameraAnim(0.0f),
   mTimeReset(true),
   mFlowFieldAnimDebug(false),
   mWinAnimationStarted(false),
   mPlayerNameDisplay(0),
   mGamePlaybackDisplay(0),
   mCameraFollowsPlayer(false),
   mCameraShakeIntensity(false),
   mShroomFilter(0),
   mBlendQuad(0),
   mMushroomAnimation(0),
   mInvisiblePlayers(0),
   mPlayerDeathEffect(0),
   mPlayerInfectedEffect(0),
   mPlayerInvincibleEffect(0),
   mRibbonAnimationFactory(0),
   mStarTalersFactory(0),
   mExportScene(false)
{
#ifdef INSPECT_SCENE
   mMouseButtons[0]=false;
   mMouseButtons[1]=false;
   mMouseX=0;
   mMouseY=0;
   mZoom=0.0f;
   mRotX=0.0f;
   mRotY=0.0f;
#endif

   mIdentityMatrix.identity();

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
   delete mMushroomAnimation;
   delete mInvisiblePlayers;
   delete mBlendQuad;
   delete mShroomFilter;
   delete mRibbonAnimationFactory;
   delete mStarTalersFactory;
   delete mDetonations;
   delete mPlayerDeathEffect;
   delete mPlayerInfectedEffect;
   delete mPlayerInvincibleEffect;

}


void GameDrawable::setExportScene(bool exp)
{
   mExportScene= exp;
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
      mLevel= 0;
   }

   mPlayfield= 0;
   mLevelSceneGraph= 0;
   mPlayers= 0;

   for (int i=0;i<mDestructAnim.size(); i++)
      delete mDestructAnim[i];
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
   \param visible visible flag
*/
void GameDrawable::setVisible(bool visible)
{
   // TODO:
   // some of this could should better be done using the
   // dynablaster state machine GameStateMachine

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

      displayPlayerNames();
   }
   else
   {
      mPlayerDeathEffect->clear();
      mPlayerInfectedEffect->clear();
      mPlayerInvincibleEffect->clear();

      clearFuseParticleEmitters();

      mWinAnimationStarted = false;
   }

   Drawable::setVisible(visible);
}


//-----------------------------------------------------------------------------
/*!
   \param obj sender
   \param event event
*/
bool GameDrawable::eventFilter(QObject* obj, QEvent* event)
{
   if (event->type() == QEvent::ActivationChange)
   {
       emit releaseAllKeys();
   }

   return QObject::eventFilter(obj, event);
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
   \param event key release event
*/
void GameDrawable::keyReleaseEvent(QKeyEvent* event)
{
   emit keyReleased(event);
}


//-----------------------------------------------------------------------------
/*!
   \param x x coord
   \param y y coord
   \param button mouse button
*/
void GameDrawable::mousePressEvent(
   int x,
   int y,
   Qt::MouseButton button
)
{
#ifdef INSPECT_SCENE
   if (button == Qt::LeftButton)
      mMouseButtons[0] = true;

   if (button == Qt::RightButton)
      mMouseButtons[1] = true;

   mMouseX = x;
   mMouseY = y;
#else
   Q_UNUSED(x);
   Q_UNUSED(y);
   Q_UNUSED(button);
#endif
}


//-----------------------------------------------------------------------------
/*!
   \param x x coord
   \param y y coord
*/
void GameDrawable::mouseMoveEvent(int x, int y)
{
#ifdef INSPECT_SCENE
   if (mMouseButtons[0])
   {
      mRotY -= (mMouseX - x) * 0.001f;
      mRotX -= (mMouseY - y) * 0.001f;
   }

   mMouseX = x;
   mMouseY = y;
#else
   Q_UNUSED(x);
   Q_UNUSED(y);
#endif
}


//-----------------------------------------------------------------------------
/*!
   \param e mouse event
*/
void GameDrawable::mouseReleaseEvent(QMouseEvent* e)
{
#ifdef INSPECT_SCENE
   if (e->button() == Qt::LeftButton)
      mMouseButtons[0] = false;

   if (e->button() == Qt::RightButton)
      mMouseButtons[1] = false;
#else
   Q_UNUSED(e);
#endif
}


//-----------------------------------------------------------------------------
/*!
   \param e mouse event
*/
void GameDrawable::wheelEvent(QWheelEvent * e)
{
#ifdef INSPECT_SCENE
   mZoom += e->delta() * 0.0005f;
   if (mZoom < 0.0001f)
      mZoom= 0.0001f;
   if (mZoom > 100.0f)
      mZoom= 100.0f;
#else
   Q_UNUSED(e);
#endif
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
   foreach (MapItem *item, mMapItems.values())
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
*/
void GameDrawable::loadLevel(const QString& levelPath)
{
//   QString levelPath= "level-space";
   qDebug("GameDrawable::loadLevel: loading %s", qPrintable(levelPath));

   emit levelLoadingStarted(levelPath);

//   if (!mLevelPath.isEmpty())
//      FileStream::removePath( qPrintable(mLevelPath) );

   mMap.clear();

   resetPlayers();

   deleteLevelData();

   mLevelPath =
     QString("data/%1").arg(levelPath);

   // assign new level
   LevelLoadingThread *loader= new LevelLoadingThread(mLevelPath);

   connect(
      loader,
      SIGNAL(finished(const QString&)),
      this,
      SLOT(levelFinished(const QString&))
   );

   // delete the thread after all pointers have been transferred here
   // by calling the slot levelFinished (see above)
   connect(
      loader,
      SIGNAL(finished()),
      loader,
      SLOT(deleteLater())
   );

   loader->start();
}


//-----------------------------------------------------------------------------
/*!
   \param levelPath loaded level
*/
void GameDrawable::levelFinished(const QString&)
{
   LevelLoadingThread *loader= (LevelLoadingThread*)sender();


   if (mLevel)
   {
      clear();
      delete mLevel;
      mLevel= 0;
   }

   mLevel= loader->getLevel();
   if (!mLevel)
   {
      mLevelSceneGraph= 0;
      mPlayfield= 0;
      mPlayers= 0;
      mExtraFlame   = 0;
      mExtraBomb    = 0;
      mExtraSpeedup = 0;
      mExtraKick    = 0;
      mExtraSkull   = 0;
      mDestructAnim = 0;
      mShadowBillboards= 0;
      mShadowBlocks= 0;
      mBombs= 0;
      mStones= 0;
      mBlocks= 0;
      mSkulls= 0;
      mDestruction= 0;
      return;
   }

   mLevelSceneGraph= mLevel->getLevel();
   mPlayfield= mLevel->getScene();
   mPlayers= mLevel->getPlayers();

   mInvisiblePlayers->setPlayerScene( mPlayers );

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
   // recycling the blendquad shader
   FileStream::addPath("data/effects/spherefragments/shaders");

   // init extra animations
   ExtraAnimation::initializeTextures();
   ExtraRevealAnimation::initialize();

   // init particles
   FuseParticle::initialize();

   mDetonations= new DetonationManager();
   mDetonations->init();

   // player names
   mPlayerNameDisplay = new GamePlayerNameDisplay(this);
   mPlayerNameDisplay->initialize();

   // text display for game replay
   mGamePlaybackDisplay = new GamePlaybackDisplay(this);
   mGamePlaybackDisplay->initialize();

   // frame buffer blending
   mBlendQuad= new BlendQuad();

   // init skull filters
   mShroomFilter = new ShroomFilter();
   mShroomFilter->init();

   // init skull animations
   mMushroomAnimation = new MushroomAnimation();

   // init player invisibility
   mInvisiblePlayers = new InvisiblePlayerEffect();

   mPlayerDeathEffect= new PlayerDeathEffect();
   mPlayerInfectedEffect= new PlayerInfectedEffect();
   mPlayerInvincibleEffect= new PlayerInvincibleEffect();

   mRibbonAnimationFactory = new RibbonAnimationFactory();
   mRibbonAnimationFactory->initialize();

   mStarTalersFactory = new StarTalersFactory();
   mStarTalersFactory->initialize();

   LensFlareFactory::getInstance()->initialize();

//   // link skull animation to game state
//   connect(
//      BombermanClient::getInstance(),
//      SIGNAL(gameStopped()),
//      this,
//      SLOT(cleanupInfections())
//   );
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
      return 0;
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
      return 0;
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
      mesh->setRenderFlags(flags);
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
   mMap.set(item->getX(), item->getY(), 0);
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

   Mesh *ref= (Mesh*)scene->getNode("Block");
   Mesh *mesh= new Mesh(*ref);
   mesh->setTransform(scale * pos);
   mPlayfield->addNode(mesh);
   mat->addMesh(mesh);
   mShadowBlocks->addMesh(mesh);

//               mStones->addMesh(mesh);
//               mOutlines->addMesh(mesh);

   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::createExtra(ExtraMapItem *extra)
{
   Mesh* mesh = 0;
   Mesh *extraMesh= (Mesh*)mPlayfield->getNode("Extra");
   mesh= new Extra(extra->getExtraType(), extraMesh->getPart(0), extra->getX(), extra->getY());
   mExtraMaterials[extra->getExtraType()]->addMesh(mesh);
   ExtraRevealAnimation* anim = new ExtraRevealAnimation();
   anim->setPos(extra->getX(), extra->getY());
   mExtraRevealAnimations << anim;
   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh* GameDrawable::createBomb(MapItem *item)
{
   Mesh* mesh = 0;

   Mesh *obj= (Mesh*)mPlayfield->getNode("lunte");

   if (obj)
   {
      mesh= new Mesh(*obj);
      mesh->setAnimationFrame(mTime);

      Matrix translationMatrix;
      Vector itemPosition =
         Vector(item->getX()+0.4, -item->getY()-0.5, 0.0f);
      translationMatrix.identity();
      translationMatrix.translate( itemPosition );

      mesh->setTransform(translationMatrix);

      mPlayfield->addNode(mesh);
      mBombs->addMesh(mesh);
      mShadowBillboards->addMesh(mesh);

      // create new particle emitter
      FuseParticleEmitter* emitter = new FuseParticleEmitter(
         itemPosition + FuseParticle::getBombOffset()
      );

      mEmitters.insert(item, emitter);
   }

   return mesh;
}


//-----------------------------------------------------------------------------
/*!
*/
Mesh *GameDrawable::createSkull(MapItem *item)
{
   Mesh* skullMesh = ((Mesh*)mPlayfield->getNode("skull"));

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
      /*

      ^
      |
      |
      +-----+-----+-----+-----+-----+-----+----->
      0     1     2     3     4     5           t

      => that is 6 x 20 frames

         1 frame consists of 160 ticks

         => the animation needs to be restarted after 120 * 160 = 19200 ticks

            one complete cycle shall be restarted after 6 seconds

            => 19200 / 6 = 3200

               one cycle is achieved by time *= 3200.0f

      */

      // frame time in seconds
      float frame = time;

      // frame is relative to start frame
      frame -= skull->getStartTime();

      // apply tick multiplier
      frame *= 3200.0f;

      // restart animation after 1 loop of 19200 ticks
      float f = (float)fmod((double)frame, 19200.0);

      // animate reference
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
*/
void GameDrawable::playerInfected(
   int id,
   Constants::SkullType skullType,
   int infectorId,
   int extraX,
   int extraY
)
{
   PlayerItem* playerItem = getPlayer(id);

   if (playerItem)
   {
      bool aborted = (skullType == Constants::SkullReset);

      // infection started, create new infection animation
      if (!aborted)
      {
         // some skulls have their own animations
         // these are started here
         switch (skullType)
         {
            case Constants::SkullMushroom:
            {
               mPlayerInfectedEffect->add( playerItem->getMaterial() );
               // mushroom effect only works on self
               if (id == mPlayerId)
               {
                  mMushroomAnimation->start();
               }

               break;
            }

            case Constants::SkullInvincible:
            {
               if (infectorId == -1)
                  mRibbonAnimationFactory->add(extraX, extraY);
               mPlayerInvincibleEffect->add( playerItem->getMaterial() );
               break;
            }

            case Constants::SkullInvisible:
            {
               // all players can be invisible
               mInvisiblePlayers->addPlayer(playerItem);
               break;
            }

            default:
            {
               mPlayerInfectedEffect->add( playerItem->getMaterial() );
               break;
            }
         }
      }

      // this is the abort case => clean up running infections here
      else
      {
         // delete the animation
         mPlayerInfectedEffect->remove( playerItem->getMaterial() );
         mPlayerInvincibleEffect->remove( playerItem->getMaterial() );
         mInvisiblePlayers->removePlayer(playerItem);
         // in all other cases, cleanup
         if (id == mPlayerId)
         {
            mMushroomAnimation->abort();
         }
      }
   }
}


//void GameDrawable::cleanupInfections()
//{
//   // stop animation in any case on game stop event
//   foreach (InfectionAnimation* animation, mInfectionAnimations.values())
//   {
//      animation->deleteLater();
//   }

//   mInfectionAnimations.clear();

//   // abort mushroom animation
//   mMushroomAnimation->abort();

//   // remove invisibility
//   // mInvisiblePlayers->removeAllPlayers();
//   // not needed...
//}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::createMapItem(MapItem *item)
{
   if (!mMapItems.contains(item))
   {
      Mesh *mesh= 0;

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
            ExtraMapItem *extra= (ExtraMapItem*)item;

            if (extra->getExtraType() == Constants::ExtraSkull)
            {
               mesh = createSkull(item);
               mesh = 0; // TODO: nicer.
            }
            else
               mesh = createExtra(extra);

            break;
         }

         default:
            item= 0;
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
   QSet<MapItem*>::Iterator it= mMapItems.find(item);

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
//         mOutlines->removeMesh(mesh);
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

   // remove particle emitter
   if (item->getType() == MapItem::Bomb)
   {
      FuseParticleEmitter* emitter = mEmitters.take(item);
      delete emitter;
   }
   else if (item->getType() == MapItem::Extra)
   {
      //
   }
}


//-----------------------------------------------------------------------------
/*!
*/
Node* GameDrawable::createDestruction(SceneGraph *scene, float x, float y, Constants::Direction direction, float flameCount)
{
   Dummy *dummy= 0;

   // create destruction animation
   Node *root= 0;
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
      pos= Matrix::rotateZ( rot * M_PI * 0.5f );
      pos.translate( Vector(x+0.5f, -y-0.5f) );
      dummy->setUserTransformable(true);
      Matrix scale= Matrix::scale(0.8f, 0.8f, 0.8f);
      dummy->setTransform(scale * pos);

      for (int i=0; i<destruct->getChildCount(); i++)
      {
         Node* child= destruct->getChild(i);
         if (child->id() == Node::idMesh)
         {
            Mesh *ref= (Mesh*)child;
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
   QSet<MapItem*>::Iterator it= mMapItems.find(item);
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
*/
void drawCube(float x, float y, float s, Vector c)
{
   glColor3f(c.x, c.y, c.z);

   glBegin(GL_LINES);

   glVertex3f(x,   -y, s);
   glVertex3f(x+s, -y, s);

   glVertex3f(x,   -y-s, s);
   glVertex3f(x+s, -y-s, s);

   glVertex3f(x, -y, s);
   glVertex3f(x, -y-s, s);

   glVertex3f(x+s, -y, s);
   glVertex3f(x+s, -y-s, s);

   glVertex3f(x,   -y, 0);
   glVertex3f(x+s, -y, 0);

   glVertex3f(x,   -y-s, 0);
   glVertex3f(x+s, -y-s, 0);

   glVertex3f(x, -y, 0);
   glVertex3f(x, -y-s, 0);

   glVertex3f(x+s, -y, 0);
   glVertex3f(x+s, -y-s, 0);

   glVertex3f(x, -y, 0.0f);
   glVertex3f(x, -y, s);

   glVertex3f(x, -y-s, 0.0f);
   glVertex3f(x, -y-s, s);

   glVertex3f(x+s, -y, 0.0f);
   glVertex3f(x+s, -y, s);

   glVertex3f(x+s, -y-s, 0.0f);
   glVertex3f(x+s, -y-s, s);

   glEnd();
}


//-----------------------------------------------------------------------------
/*!
*/
void drawFloorTile(float x, float y)
{
   glColor4f(1.0f,0.0f,0.0f,1.0f);

   glBegin(GL_QUADS);

   glVertex3f(x,        -y,        0.01f);
   glVertex3f(x + 1.0f, -y,        0.01f);
   glVertex3f(x + 1.0f, -y - 1.0f, 0.01f);
   glVertex3f(x,        -y - 1.0f, 0.01f);

   glEnd();
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
            x + 0.5,
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
*/
void GameDrawable::extraRemoved(
   int x,
   int y,
   bool destroyed,
   Constants::ExtraType extra,
   int playerId
)
{
   if (destroyed)
   {
      // init extra destroyed animation
      ExtraAnimation* animation = new ExtraAnimation();
      animation->setOrigin(
         Vector(
            x + 0.5f,
           -y - 0.5f,
            0.0f
         )
      );

      animation->setExtraDestroyed(destroyed);
      animation->initialize();

      mExtraAnimations << animation;
   }
   else
   {
      mStarTalersFactory->add(x, y, extra);
   }

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
      player->setRotation(angle + (float)M_PI * 0.5f);
      player->setPosition(x, y);
   }
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
      return 0;
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
   Material* playerMaterial= mPlayers->getMaterial((int)color-1);
   playerMaterial->addMesh(p);
   player->setMaterial( playerMaterial );

   // test invincible effect: add to every player
//   mPlayerInvincibleEffect->add( playerMaterial );


   // infection test: add to every player
//   mPlayerInfectedEffect->add( playerMaterial );

//   mInvisiblePlayers->addPlayer(player);

   mShadowBillboards->addMesh(p);

   //   mPlayerMaterials->addMesh(p);
   //   mOutlines->addMesh(p);
   //   outlines->addMesh(p);
   //   player->setMaterial(tex);
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
      mPlayerInfectedEffect->remove( player->getMaterial() );
      mPlayerInvincibleEffect->remove( player->getMaterial() );
      mInvisiblePlayers->removePlayer( player );

/*
      int mid= (int)player->getColor();
      mPlayerMaterials[mid]->removeMesh(player->getMesh());
      mShadowBillboards->removeMesh(player->getMesh());
      mPlayerList.remove(id);
      delete player;
*/

      // abort mushroom animation if we were killed
      if (id == mPlayerId)
      {
         if (mMushroomAnimation->isActive())
         {
            mMushroomAnimation->abort();
         }
      }
   }

   // check for survivors
   if (mPlayerList.size() > 1)
   {
      int alive= 0;
      foreach (PlayerItem* player, mPlayerList)
      {
         if (!player->isKilled())
            alive++;
      }

      if (alive == 1)
      {
         foreach (PlayerItem* player, mPlayerList)
         {
            if (!player->isKilled())
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
         mPlayerDeathEffect->clear();
         mPlayerInfectedEffect->clear();
         mPlayerInvincibleEffect->clear();
         clearFuseParticleEmitters();
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
      mDetonations->update(time / 62.5);

   if (mTimeReset)
   {
      mTime = time;
      mTimeReset = false;
   }

   float delta= time - mTime;
   mTime= time;

   mCameraAnim+=delta*60.0;
//   mCameraAnim= 200*160;

   if (mBounce > delta*0.01f)
      mBounce-=delta*0.01f;
   else
      mBounce= 0.0f;

   // animate fuse sparkles
   FuseParticle::animate(delta);

   // rotational rotation is rotating:
   for (QMap<MapItem*,Mesh*>::ConstIterator it= mMeshes.constBegin(); it != mMeshes.constEnd(); it++)
   {
      MapItem *item= it.key();
      switch (item->getType())
      {
         case MapItem::Extra:
         {
            Extra *extra= (Extra*)it.value();
            extra->animate(time);
         }
         break;

         case MapItem::Bomb:
         {
            Mesh *mesh= it.value();
            float t= time * 0.1 + mesh->getAnimationFrame();

            Vector pos= mesh->getTransform().translation();

            float sx= 1.0 + sin(t)*0.2f;
            float sy= 1.0 - sin(t)*0.3f;

            Matrix mat= Matrix::scale(sx,sx,sy);
            mat.translate(pos);
            mesh->setTransform(mat);

            // update particle emitter position
            sx= 1.0 + sin(t+0.1f)*0.2f;
            sy= 1.0 - sin(t+0.1f)*0.3f;
            Matrix particleScaleMat= Matrix::scale(sx,sx,sy);
            particleScaleMat.translate(pos);
            Vector emitterPos = particleScaleMat * FuseParticle::getBombOffset();

            QMap<MapItem*, FuseParticleEmitter*>::const_iterator it;
            it = mEmitters.constFind(item);

            if (it != mEmitters.constEnd())
               it.value()->setPosition(emitterPos);
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
   // printf("destructions: %d \r", mDestructions.size());
   for (QList<Node*>::Iterator it= mDestructions.begin(); it!=mDestructions.end(); )
   {
      Node* destr= *it;
      bool remove= false;
      for (int i=0; i<destr->getChildCount(); i++)
      {
         Mesh *mesh= (Mesh*)destr->getChild(i);
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
            Mesh *mesh= (Mesh*)destr->getChild(i);
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

   mPlayerDeathEffect->animate( delta );
   mPlayerInfectedEffect->animate( delta );
   mPlayerInvincibleEffect->animate( delta );

   animateSkulls(time);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::drawTestQuad()
{
   float pointSize = 0.3f;

   float x = 6.5f;
   float y = -5.5f;
   float z = 1.0f;
   glBegin(GL_QUADS);

   glColor4f(1,1,1,1);

   glVertex3f(
      x - pointSize,
      y - pointSize,
      z
   );

   glVertex3f(
      x + pointSize,
      y - pointSize,
      z
   );

   glVertex3f(
      x + pointSize,
      y + pointSize,
      z
   );

   glVertex3f(
      x - pointSize,
      y + pointSize,
      z
   );

   glEnd();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::drawExtraAnimations(float dt)
{
   if (!mExtraAnimations.isEmpty())
   {
      QList<ExtraAnimation*>::Iterator it;
      for (it = mExtraAnimations.begin(); it != mExtraAnimations.end(); )
      {
         if ( (*it)->isElapsed() )
         {
            delete *it;
            it = mExtraAnimations.erase(it);
         }
         else
         {
            (*it)->draw(dt);
            it++;
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::drawExtraRevealAnimations(float dt)
{
   if (!mExtraRevealAnimations.isEmpty())
   {
      QList<ExtraRevealAnimation*>::Iterator it;
      for (it = mExtraRevealAnimations.begin(); it != mExtraRevealAnimations.end(); )
      {
         if ( (*it)->isElapsed() )
         {
            delete *it;
            it = mExtraRevealAnimations.erase(it);
         }
         else
         {
            (*it)->draw(dt);
            it++;
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::drawBoundingBox(Vector min, Vector max)
{
   glBegin(GL_LINES);
   glVertex3f( min.x, min.y, min.z );
   glVertex3f( max.x, min.y, min.z );

   glVertex3f( max.x, min.y, min.z );
   glVertex3f( max.x, max.y, min.z );

   glVertex3f( max.x, max.y, min.z );
   glVertex3f( min.x, max.y, min.z );

   glVertex3f( min.x, max.y, min.z );
   glVertex3f( min.x, min.y, min.z );
   glEnd();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::drawBoundingRect()
{
/*
    // get projection matrix (contains camera)
    Matrix projMat;
    glGetFloatv(GL_PROJECTION_MATRIX, projMat.data());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // clip space unit cube
    glOrtho(-1,1, 1,1, -1,+1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int i=0; i<MAX_PLAYERS; i++)
    {
       Vector min, max;
       Material* playermaterial= mPlayers->getMaterial(i);
       playerBoundingRect(playermaterial, min, max, projMat);

       glColor4f(1,1,1,1);
       // draw bounding box
       drawBoundingBox(min, max);
    }
*/
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
         float x= 0.8 + sin((1.0f-time) * 14.0) * 0.1f * intense;
         float y= 0.8 - sin((1.0f-time) * 12.0) * 0.1f * intense;
         float z= 0.6 + cos((1.0f-time) * 16.0) * 0.2f * intense + 0.2*(1.0f - intense);

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
*/
void GameDrawable::paintGL()
{
   float time= GlobalTime::Instance()->getTime();

   FrameBuffer* gameFb= MainDrawable::getInstance()->getRenderBuffer();

   FrameBuffer::push();

   mInvisiblePlayers->update( time );

/*
   int test= rand() % 100;
   if (test == 0)
   {
      PlayerItem* playerItem = getPlayer( mPlayerId );

      if (playerItem)
      {
         mPlayerInfectedEffect->remove( playerItem->getMaterial() );
         mPlayerInfectedEffect->add( playerItem->getMaterial() );
      }
   }
*/

   // smaller screen -> smaller blur radius
   mPlayerInvincibleEffect->setRadius( 30.0f * gameFb->width() / 1920.0 );

   gameFb->bind();


   if (mLevel)
      mLevel->drawBackground();


   float dt = mTime - mTimePrev;
   // double t1,t2;




/*
   if ((rand() % 200) == 50)
   {
      int x= rand() % 13;
      int y= rand() % 11;
      ExtraRevealAnimation* anim = new ExtraRevealAnimation();
      anim->setPos(x,y);
      mExtraRevealAnimations << anim;
   }
*/

//   if ((rand() % 500) == 50)
//   {
//      if (mStoneList.size() > 0)
//      {
//         int index= rand() % mStoneList.size();
//         shakeBlock( mStoneList[index] );
//      }
//   }

//   // randomly flash player 0
//   if ((rand() % 500) == 50)
//   {
//      PlayerItem *player= mPlayerList[0];
//      player->setFlash(1.0f);
//   }

   shakeBoxes(dt*0.015f);

   float width, height;
   Constants::Dimension dimensions;
   dimensions = getDimensions(width, height);

   float bounceX, bounceY;

   // use prime factors
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
          if (HeadlessIntegration::getInstanceCount() > 0)
          {
             // multiple players on one pc
             const QMap<int, PlayerInfo*>* players =
                BombermanClient::getInstance()->getPlayerInfoMap();

             int id = 0;
             foreach (PlayerInfo* player, *players)
             {
                id = player->getId();

                if (
                      HeadlessIntegration::isHeadlessPlayer(id)
                   || (BombermanClient::getInstance()->getPlayerId() == id)
                )
                {
                   if (mLevel)
                   mLevel->addPlayerPosition( player );
                }
             }
          }
          else
          {
             // single player
             PlayerInfo* player= BombermanClient::getInstance()->getCurrentPlayerInfo();
             if (player)
                mLevel->addPlayerPosition( player );
          }

          // if no players have been added to the camera interpolation; then
          // add all players
          if (mLevel->isPlayerMapEmpty())
          {
             // multiple players on one pc
             const QMap<int, PlayerInfo*>* players =
                BombermanClient::getInstance()->getPlayerInfoMap();

             foreach (PlayerInfo* player, *players)
                mLevel->addPlayerPosition( player );
          }
      }

      mLevel->endPlayerPositionUpdate();

      view= mLevel->getCameraMatrix(mCameraAnim);
   }


   Matrix shake= Matrix::position(bounceX, bounceY, 0.0f) * view;

#ifdef INSPECT_SCENE
   shake = Matrix::rotateX( mRotX ) * Matrix::rotateZ( mRotY );
#endif

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
      InvisibilityMaterial* invisible= (InvisibilityMaterial*)mPlayers->getMaterial(10);
      if (invisible->size() > 0)
      {
         // TODO: check if any player is invisible in the first place!
         // TODO: do not reallocate texture every frame!
         unsigned int background= gameFb->copyTexture();

         invisible->setTexture( background );
      }

      mPlayers->render(0.0, shake);
   }

   mDetonations->render();

//   drawBoundingRect();


/*
   // death effect test:
   if ((rand() % 100) == 1)
   {
      int id= rand() % mPlayerList.size();
      PlayerItem* player= mPlayerList[id];

      if (player)
      {
         Material* material = mPlayers->getMaterial( (int)player->getColor()-1 );
         mPlayerDeathEffect->add( material );
      }
   }
*/

   // start flow fields when player got killed (and kill anim is over)
   Mesh* playerMesh = 0;
   Geometry* playerGeometry = 0;
   foreach(PlayerItem* player, mPlayerList)
   {
      if (player->isKilled())
      {
         playerMesh = player->getMesh();

         if (playerMesh->getFrame() > 10000.0f || mFlowFieldAnimDebug)
         {
            playerGeometry = playerMesh->getPart(0);

            if (playerGeometry->isVisible())
            {
               Material* material = mPlayers->getMaterial( (int)player->getColor()-1 );
               mPlayerDeathEffect->add( material );
               // make player invisible
               playerGeometry->setVisible(false);
            }
         }
      }
   }


   // particle effects (don't need multisampling)
   if (gameFb->samples()>1)
      glDisable(GL_MULTISAMPLE);
   mPlayerDeathEffect->render();
   mPlayerInfectedEffect->render();
   mPlayerInvincibleEffect->render();

   if (gameFb->samples()>1)
      glEnable(GL_MULTISAMPLE);

   // draw extra animations
   drawExtraAnimations(dt);
   drawExtraRevealAnimations(dt);

   mRibbonAnimationFactory->update(dt);

   mStarTalersFactory->update(dt);
   // VideoDebugging::debugModelViewMatrix();
   // VideoDebugging::debugProjectionMatrix();

   // draw all fuse particles
//   glFinish();
   FuseParticle::draw();

   // draw player names
   if (mPlayerNameDisplay->isActive())
   {
      mPlayerNameDisplay->setPlayerData(mPlayerList);
      mPlayerNameDisplay->draw();
   }

   // draw level specific stuff
   if (mLevel)
      mLevel->draw();

   // use game framebuffer to generate shroom effect
   if (mMushroomAnimation->isActive())
   {
      int width = 0;
      int height = 0;
      activeDevice->getViewPort(0,0, &width, &height);
      int scaleX = (int)floor((float)width / gameFb->width() + 0.5);
      int scaleY = (int)floor((float)height / gameFb->height() + 0.5);
      float u = (float) width / (gameFb->width() * scaleX);
      float v = (float) height / (gameFb->height() * scaleY);

//      glClear(GL_DEPTH_BUFFER_BIT);
      mMushroomAnimation->update();
      mShroomFilter->setIntensity(mMushroomAnimation->getIntensity());
      mShroomFilter->setTime(GlobalTime::Instance()->getTime());
      mShroomFilter->process(gameFb->copyTexture(), u, v);
   }

   // re-set unscaled camera
   if (mLevelSceneGraph)
   {
      mLevelSceneGraph->setupCamera(shake);
   }

   gameFb->unbind();
   FrameBuffer::pop();

   // draw game framebuffer
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE); // _MINUS_SRC_ALPHA
   mBlendQuad->process(gameFb->texture());
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // draw lensflares
   LensFlareFactory::getInstance()->draw();

   // draw playback text
   mGamePlaybackDisplay->draw(time);

   //! export was pressed
   if (mExportScene)
   {
      FileStream stream;
      stream.open("scene.obj", true);
      int index= 1;
      if (mPlayfield)
         mPlayfield->exportOBJ(0.0, &stream, index);
      if (mLevelSceneGraph)
         mLevelSceneGraph->exportOBJ(0.0, &stream, index);
      if (mPlayers)
         mPlayers->exportOBJ(0.0, &stream, index);
      stream.close();
      mExportScene= false;
   }

   mTimePrev = mTime;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::resetPlayers()
{
   if (mLevel)
      mLevel->resetPlayerPositions();

   mInvisiblePlayers->removeAllPlayers();

   QMap<int,PlayerItem*>::Iterator it= mPlayerList.begin();
   while (it != mPlayerList.end())
   {
      PlayerItem* player= *it;
      it= mPlayerList.erase(it);

      int color= (int)player->getColor();
      Mesh *mesh= player->getMesh();
      Material* playerMaterial= mPlayers->getMaterial((int)color-1);
      playerMaterial->removeMesh(mesh);
      mShadowBillboards->removeMesh(mesh);

      delete player;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameDrawable::clearFuseParticleEmitters()
{
   qDeleteAll(mEmitters);
   mEmitters.clear();
   FuseParticle::clear();
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


