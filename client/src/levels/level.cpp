// header
#include "level.h"
#include "framework/gldevice.h"
#include "tools/filestream.h"
#include "nodes/scenegraph.h"

// defines
#define LEVEL_CASTLE  "level-castle"
#define LEVEL_MANSION "level-mansion"
#define LEVEL_SPACE   "level-space"
#define LEVEL_DUMMY   "level-dummy"

// Qt
#include <QCoreApplication>


//-----------------------------------------------------------------------------
/*!
*/
Level::Level(LevelType levelType)
  : mLevelType(levelType),
    mAborted(false),
    mScene(0),
    mLevel(0),
    mPlayers(0),
    mCamInterp(0),
    mStones(0),
    mBlocks(0),
    mSkulls(0),
    mDestruction(0),
    mExtraFlame(0),
    mExtraBomb(0),
    mExtraSpeedup(0),
    mExtraKick(0),
    mExtraSkull(0),
    mOutlines(0),
    mBombs(0),
    mShadowBillboards(0),
    mShadowBlocks(0)
{
}


//-----------------------------------------------------------------------------
/*!
*/
Level::~Level()
{
   delete mLevel;
   delete mScene;
   delete mPlayers;
}

//-----------------------------------------------------------------------------
/*!
*/
void Level::abort()
{
   QWriteLocker locker(&mLock);
   mAborted = true;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if thread is aborted
*/
bool Level::isAborted() const
{
   QReadLocker locker(&mLock);
   return mAborted;
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::load()
{
   QString levelPath= "data/"+path();

   FileStream::addPath( qPrintable(levelPath) );

   loadData();

   FileStream::removePath( qPrintable(levelPath) );
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::loadData()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::initialize()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::draw()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::drawBackground()
{
   activeDevice->clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::animate(float /*dt*/)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::reset()
{
}


//-----------------------------------------------------------------------------
/*!
*/
QString Level::path() const
{
   return getLevelDirectoryName( mLevelType );
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::resetPlayerPositions()
{
   if (mCamInterp)
      mCamInterp->resetPlayerPositions();
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::startPositionUpdate(float width, float height, float dt)
{
   if (mCamInterp)
      mCamInterp->startPositionUpdate(width, height, dt);
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::addPlayerPosition( PlayerInfo* player )
{
   if (mCamInterp)
      mCamInterp->addPlayerPosition( player );
}


//-----------------------------------------------------------------------------
/*!
*/
void Level::endPlayerPositionUpdate()
{
   if (mCamInterp)
      mCamInterp->endPlayerPositionUpdate();
}


//-----------------------------------------------------------------------------
/*!
*/
bool Level::isPlayerMapEmpty() const
{
   bool empty = false;

   if (mCamInterp)
      empty = mCamInterp->isPlayerMapEmpty();

   return empty;
}


//-----------------------------------------------------------------------------
/*!
*/
Matrix Level::getCameraMatrix(float time, float scale)
{
   if (mCamInterp)
      return mCamInterp->getCameraMatrix(time, scale);
   else
      return Matrix();
}

/*
void Level::setPlayerPosition()
{

}
*/


//-----------------------------------------------------------------------------
/*!
*/
QString Level::getLevelDirectoryName(Level::LevelType levelType)
{
   QString dir;

   switch (levelType)
   {
      case LevelMansion:
         dir = LEVEL_MANSION;
         break;
      case LevelSpace:
         dir = LEVEL_SPACE;
         break;
      case LevelDummy:
         dir = LEVEL_DUMMY;
         break;

      default:
         dir = LEVEL_CASTLE;
         break;
   }

   return dir;
}


//-----------------------------------------------------------------------------
/*!
*/
QString Level::getLevelName(Level::LevelType levelType)
{
   QString name;

   switch (levelType)
   {
      case LevelMansion:
         name = qApp->tr("the mansion");
         break;
      case LevelSpace:
         name = qApp->tr("space");
         break;
      case LevelDummy:
         name = qApp->tr("dummy");
         break;

      default:
         name =  qApp->tr("the castle");
         break;
   }

   return name;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getFlameExtra() const
{
   return mExtraFlame;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getBombExtra() const
{
   return mExtraBomb;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getSpeedupExtra() const
{
   return mExtraSpeedup;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getKickExtra() const
{
   return mExtraKick;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getSkullExtra() const
{
   return mExtraSkull;
}


//-----------------------------------------------------------------------------
/*!
*/
SceneGraph* Level::getLevel() const
{
   return mLevel;
}


//-----------------------------------------------------------------------------
/*!
*/
SceneGraph* Level::getScene() const
{
   return mScene;
}


//-----------------------------------------------------------------------------
/*!
*/
SceneGraph* Level::getPlayers() const
{
   return mPlayers;
}


//-----------------------------------------------------------------------------
/*!
*/
const Array<Node*>& Level::getDestructions() const
{
   return mDestructAnim;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getShadowBillboard() const
{
   return mShadowBillboards;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getShadowBlockBillboard() const
{
   return mShadowBlocks;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getBombMaterial() const
{
   return mBombs;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getSkullMaterial() const
{
   return mSkulls;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getStoneMaterial() const
{
   return mStones;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getBlockMaterial() const
{
   return mBlocks;
}


//-----------------------------------------------------------------------------
/*!
*/
Material* Level::getDestructionMaterial() const
{
   return mDestruction;
}


