// header
#include "gamewindrawable.h"
#include "nodes/scenegraph.h"
#include "framework/framebuffer.h"
#include "postproduction/blurfilter.h"
#include "maindrawable.h"

#include "tools/filestream.h"

// game
#include "fontpool.h"
#include "bombermanclient.h"
#include "gamesettings.h"
#include "levels/levelloadingthread.h"
#include "playeritem.h"
#include "soundmanager.h"

// engine
#include "gldevice.h"
#include "engine/animation/motionmixer.h"
#include "image/tga.h"
#include "materials/material.h"
#include "materials/playermaterial.h"
#include "nodes/camera.h"
#include "nodes/mesh.h"
#include "render/texturepool.h"
#include "materials/materialfactory.h"
#include "materials/texturematerial.h"
#include "materials/environmentambientmaterial.h"
#include "materials/environmentambientdiffusematerial.h"

// cmath
#include <math.h>

// defines
#define OFFSET_X_POINTS -43
#define OFFSET_Y_POINTS -2
#define OFFSET_Y_NAME -2
#define OFFSET_Y_ICON -20
#define CENTER_WIDTH_SCORE 130
#define RADIUS_MAX 200.0f

class CupMaterialFactory : public MaterialFactory
{
public:
   Material* createMaterial(SceneGraph* scene, int id) const
   {
      Material* mat= 0;
      switch(id)
      {
         case (MAP_AMBIENT | MAP_REFLECT):
            mat= new EnvironmentAmbientMaterial(scene); //
            break;
         case (MAP_AMBIENT | MAP_DIFFUSE | MAP_REFLECT):
            mat= new EnvironmentAmbientDiffuseMaterial(scene); //
            break;
         default:
            break;
      }
      return mat;
   }
};

//-----------------------------------------------------------------------------
/*!
   \param dev gl device
   \param visible visible flag
*/
GameWinDrawable::GameWinDrawable(RenderDevice *dev, bool visible)
 : QObject(),
   Drawable(dev, visible),
   mColorEnum(Constants::ColorWhite),
   mLargeFont(0),
   mScene(0),
   mRenderTime(0.0f),
   mTime(0.0f),
   mDeltaTime(0.0f),
   mStartTime(0.0f),
   mBlur(0),
   mGameInformation(0),
   mPlayerItem(0),
   mMotionMixer(0),
   mPlayerMesh(0),
   mPlayerMaterial(0)
{
   mFilename = "data/game/results.psd";

   connect(
      GameStateMachine::getInstance(),
      SIGNAL(stateChanged()),
      this,
      SLOT(stateChanged())
   );
}


//-----------------------------------------------------------------------------
/*!
*/
GameWinDrawable::~GameWinDrawable()
{
   delete mPlayerItem;
   delete mMotionMixer;
   delete mPlayerMaterial;
   delete mBlur;

   // we do not delete mPlayerMesh as it is part of the player item

   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::initializeGL()
{
   FileStream::addPath("data/cup");

   // init font
   mLargeFont = FontPool::Instance()->get("large");
   mDefaultFont = FontPool::Instance()->get("large-outlined");

   mScene= new SceneGraph();
   CupMaterialFactory factory;
   mScene->load("cup.hjb", &factory);

   Node* node= mScene->getNode("Cup");
   // mTransform= node->getTransform();

   node->setUserTransformable(true);

   mScene->getCamera()->setUserTransformable(true);

   // finish init
   mScene->render();

   FileStream::removePath("data/cup");

   mBlur= new BlurFilter();
   mBlur->init();

   // init layers
   initializeLayers();

   // win animation
   initializePlayerMaterial();
   initializeWinnerScene();
}


//-----------------------------------------------------------------------------
/*!
   \return alpha value
*/
float GameWinDrawable::getContentsAlpha() const
{
   float alpha = 0.0f;

   float elapsed = mTime;

   if (elapsed < SHOW_WINNER_FADE_IN_TIME)
   {
      alpha = elapsed / (float)SHOW_WINNER_FADE_IN_TIME;
   }
   else if (elapsed > (SHOW_WINNER_FADE_IN_TIME + SHOW_WINNER_DISPLAY_TIME + SHOW_WINNER_ADDITIONAL_TIME) )
   {
      alpha =
         qMax(
            1.0f - (
                 (elapsed - SHOW_WINNER_FADE_IN_TIME - SHOW_WINNER_DISPLAY_TIME - SHOW_WINNER_ADDITIONAL_TIME)
               / (float)SHOW_WINNER_FADE_OUT_TIME
            ),
            0.0f
         );
   }
   else
   {
      alpha = 1.0f;
   }

   return alpha;
}


//-----------------------------------------------------------------------------
/*!
   \return drawable alpha
*/
float GameWinDrawable::getDrawableAlpha() const
{
   float alpha = 1.0f;

   float elapsed = mTime;

   float duration =
        SHOW_WINNER_FADE_IN_TIME
      + SHOW_WINNER_DISPLAY_TIME
      + SHOW_WINNER_ADDITIONAL_TIME
      + SHOW_WINNER_FADE_OUT_TIME;

   if (elapsed > duration)
   {
      alpha =
         qMax(
            1.0f - ( (elapsed - duration) / (float)SHOW_WINNER_SHOW_MENU_TIME),
            0.0f
         );
   }

   return alpha;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::hideLayers()
{
   for (int i = 0; i < 10; i++)
   {
      mRanks[i]->getLayer()->setVisible(false);
      mIcons[i]->getLayer()->setVisible(false);
      mNames[i]->getLayer()->setVisible(false);
      mPoints[i]->getLayer()->setVisible(false);
      mBars[i]->getLayer()->setVisible(false);
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::drawWinnerText()
{
   float alpha = getContentsAlpha();
   float yOffset = sin(mRenderTime * 0.05f) * 1080.0 * 0.015f;

   const float fontSize = 0.7f;

   if (isDrawGame())
   {
      glColor4f(1.0f, 1.0f, 1.0f, alpha);

      glPushMatrix();
         glTranslatef(0.0f, 0.15f * 1080.0 + yOffset, 0.0f);
         mLargeFont->buildVertices(fontSize, qPrintable(tr("draw game")), 0.0f, 0.0f, 1920.0f);
         mLargeFont->draw(mLargeFont->getVertices());
      glPopMatrix();
   }
   else
   {
      QRgb rgb = getColor().rgb();
      glColor4ub(qRed(rgb), qGreen(rgb), qBlue(rgb), alpha * 255);

      QString winText = tr("%1 wins!").arg(getWinnerName());

      glColor4f(1.0f, 1.0f, 1.0f, alpha);
      glPushMatrix();
         glTranslatef(0.0f, 0.15f * 1080.0 + yOffset, 0.0f);
         mLargeFont->buildVertices(fontSize, qPrintable(winText), 0.0f, 0.0f, 1920.0f);
         mLargeFont->draw(mLargeFont->getVertices());
      glPopMatrix();
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::initGameData()
{
   // process game progress information
   mGameInformation =
      BombermanClient::getInstance()->getCurrentGameInformation();

   // process game stats information
   mPlayerScores.clear();
   QList<PlayerInfo*> infoList = BombermanClient::getInstance()->getPlayerInfoList();

   foreach(PlayerInfo* info, infoList)
   {
      Weighted<PlayerInfo*, int> w(info, computeScore(info));
      mPlayerScores.push_back(w);
   }

   qSort(mPlayerScores);

   // initially hide all layers
   hideLayers();

   // then just make those visible that are actually used
   int row = 0;
   PlayerInfo* info = 0;
   Weighted<PlayerInfo*, int> w;
   foreach (w, mPlayerScores)
   {
      info = w.getObject();
      Constants::Color color = info->getColor();
      int colorIndex = color - 1;

      qDebug(
         "GameWinDrawable::initGameData(): #%d: %s",
         w.getWeight(),
         qPrintable(info->getNick())
      );

      PSDLayer* rankLayer = mRanks[row];
      PSDLayer* nameLayer = mNames[row];
      PSDLayer* barLayer = mBars[row];
      PSDLayer* iconLayer = mIcons[colorIndex];

      // align icon layer to name layer
      iconLayer->getLayer()->setY(nameLayer->getTop() + OFFSET_Y_ICON);

      rankLayer->getLayer()->setVisible(true);
      barLayer->getLayer()->setVisible(true);
      iconLayer->getLayer()->setVisible(true);

      row++;
   }

   // reset score anim
   memset(mPlayerScoresAnimated, 0, 10 * sizeof(float));
}


//-----------------------------------------------------------------------------
/*!
   \param info player info object
   \return score
*/
int GameWinDrawable::computeScore(PlayerInfo* info) const
{
   PlayerStats ps = info->getRoundStats();

   float score =
           500.0f  * ps.getWins()
         +  10.0f  * ps.getKills()
         +   5.0f  * ps.getExtrasCollected()
         +   0.25f * ps.getSurvivalTime();

   score *= 0.1f;

   return (int)score;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::drawGameData()
{
   if (mGameInformation)
   {
      int gamesPlayed = mGameInformation->getGamesPlayed();
      int round       = mGameInformation->getCurrentRound();
      int roundCount  = mGameInformation->getRoundCount();

      Q_UNUSED(gamesPlayed);
      Q_UNUSED(round);
      Q_UNUSED(roundCount);
   }

   int score = 0;
   int row = 0;
   PlayerInfo* info = 0;
   Weighted<PlayerInfo*, int> w;
   foreach (w, mPlayerScores)
   {
      info = w.getObject();

      PSDLayer* nameLayer = mNames[row];
      PSDLayer* pointsLayer = mPoints[row];

      // draw nick
      mDefaultFont->buildVertices(
         0.27f,
         qPrintable(info->getNick()),
         nameLayer->getLeft(),
         nameLayer->getBottom() + OFFSET_Y_NAME
      );

      mDefaultFont->draw(mDefaultFont->getVertices());

      // draw score      
      mPlayerScoresAnimated[info->getColor()-1] += mDeltaTime;
      score = qMin(mPlayerScoresAnimated[info->getColor()-1], (float)computeScore(info));

      mDefaultFont->buildVertices(
         0.27f,
         qPrintable(QString("%1").arg(score)),
         pointsLayer->getLeft() + OFFSET_X_POINTS,
         pointsLayer->getBottom() + OFFSET_Y_POINTS,
         CENTER_WIDTH_SCORE
      );

      mDefaultFont->draw(mDefaultFont->getVertices());

      row++;
   }
}


//-----------------------------------------------------------------------------
/*!
   \param alpha layer alpha
*/
void GameWinDrawable::drawLayers(float alpha)
{
   for (int layerIndex = 0; layerIndex < mPsd.getLayerCount(); layerIndex++)
   {
      PSD::Layer* psdLayer = mPsd.getLayer(layerIndex);

      if (psdLayer->isVisible())
      {
         mPsdLayers[layerIndex]->render(0.0f, 0.0f, alpha);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \return radius
*/
float GameWinDrawable::getRadius() const
{
   // increase blur radius
   float radius = mRenderTime * 0.5;

   if (radius > RADIUS_MAX)
      radius = RADIUS_MAX;

   float fadeOutStart = (float)(
         SHOW_WINNER_FADE_IN_TIME
       + SHOW_WINNER_DISPLAY_TIME
       + SHOW_WINNER_ADDITIONAL_TIME
       + SHOW_WINNER_FADE_OUT_TIME
      );

   if (mTime > fadeOutStart)
   {
      // 1..0
      radius =
         qMax(
            1.0f - ((mTime - fadeOutStart) / (float)SHOW_WINNER_SHOW_MENU_TIME ),
            0.0f
         );

      radius *= RADIUS_MAX;
   }

   return radius;
}


//-----------------------------------------------------------------------------
/*!
   \param alpha computed alpha
*/
void GameWinDrawable::drawBackBuffer(float alpha)
{
   FrameBuffer* fb= FrameBuffer::Instance();

   unsigned int texture= fb->copyTexture();
   fb->bind();

   float radius = getRadius();

   mBlur->setAlpha(alpha);
   mBlur->setRadius( radius );
   mBlur->process( texture, 1,1 );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::drawScene()
{
   if (!isDrawGame())
   {
      // this is a bit tricky:
      // every material which has not been used yet, restores its textures
      // when it's used the first time. so when we set a different color map
      // earlier, it will be restored to the color map the material was created
      // with. the most simple workaround is to reset the color map after we
      // know the material has been used.
      mPlayerMaterial->setColorMap(mPlayerTextures[getColorEnum()-1]);

      // render scene
      // after the fov was fixed to match the 3dsmax settings,
      // it's too narrow in this scene. compensate manually.
      Matrix scale= Matrix::scale(0.75f, 0.75f, 1.0f);
      mScene->render(mRenderTime, scale);
   }
}


//-----------------------------------------------------------------------------
/*!
   \param alpha computed alpha
*/
void GameWinDrawable::drawPsdContents(float alpha)
{
   initGlParameters();
   drawWinnerText();
   drawLayers(alpha);
   drawGameData();
   cleanupGlParameters();
}



//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::paintGL()
{
   FrameBuffer::push();

   float alpha = getDrawableAlpha();

   // blur backbuffer
   drawBackBuffer(alpha);

   FrameBuffer *fb= MainDrawable::getInstance()->getRenderBuffer();
   fb->bind();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // draw the meshes
   drawScene();

   fb->unbind();
   FrameBuffer::pop();

   glEnable(GL_BLEND);
   fb->draw(alpha);
   glDisable(GL_BLEND);

   // draw game information
   drawPsdContents(alpha);

   if (alpha == 0.0f)
      Drawable::setVisible(false);
}


//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void GameWinDrawable::setVisible(bool visible)
{
   if (visible)
   {
      mStartTime = 0.0f;
   }
   Drawable::setVisible(visible);
}


//-----------------------------------------------------------------------------
/*!
   \param time animate time
*/
void GameWinDrawable::animate(float time)
{
   if (mStartTime == 0.0f)
      mStartTime = time;

   float timeOffset = time - mStartTime;

   mDeltaTime = timeOffset - mRenderTime;
   mRenderTime = timeOffset;
   mTime = mRenderTime * 16.0f;

   if (mDeltaTime < 0.0f)
      mDeltaTime = 0.0f;

   // player anim
   mPlayerItem->animate(mRenderTime, mDeltaTime);

   Matrix cameraTransform;

   cameraTransform = mScene->getCamera()->getTransform();

   Matrix cupTransform = mScene->getNode("Cup")->getTransform();

   /*
      scene nodes:

         Camera.target
         Camera
         Cup
            cup
            handle left
            Dynablaster
            Tex002
            Revenge
            handle right
            base

   */

   // move player elsewhere
   cupTransform.xw = 200.0f;
   cupTransform.yw = 0.0f;
   cupTransform.zw = 50.0f;
   cupTransform.ww = 1.0f;
   Matrix rotzc = Matrix::rotateZ(0.005f * sin(0.01f*time));
   Matrix rotyc = Matrix::rotateY(0.001f * sin(0.01f*time));
   mScene->getNode("Cup")->setTransform(rotyc*rotzc*cupTransform);

   float scale = 365;
   Matrix playerMatrix = Matrix::scale(scale,scale,scale);
   playerMatrix.xw = 345.0f; //baseTransform.xw; 77.246277, 47.503918, 80.641464, 1.000000
   playerMatrix.yw = 00.0f; //baseTransform.yw;
   playerMatrix.zw = -40.0f; //baseTransform.zw;
   playerMatrix.ww = 1.0f;   //baseTransform.ww;

   Matrix rotz = Matrix::rotateZ((float)M_PI);
   Matrix rotx = Matrix::rotateY(0.11f);
   playerMatrix = playerMatrix*rotz*rotx;

   mPlayerMesh->setUserTransformable(true);
   mPlayerMesh->setTransform(playerMatrix);
}


//-----------------------------------------------------------------------------
/*!
   \param m matrix to debug
   \param name matrix name
*/
void GameWinDrawable::debugMatrix(const Matrix& m, const char* name)
{
   qDebug("%s = {", name);
   qDebug("   %f, %f, %f, %f", m.xx, m.yx, m.zx, m.wx);
   qDebug("   %f, %f, %f, %f", m.xy, m.yy, m.zy, m.wy);
   qDebug("   %f, %f, %f, %f", m.xz, m.yz, m.zz, m.wz);
   qDebug("   %f, %f, %f, %f", m.xw, m.yw, m.zw, m.ww);
   qDebug("}");
}


//-----------------------------------------------------------------------------
/*!
   \param color color to use
*/
void GameWinDrawable::setColor(const QColor &color)
{
   mColor = color;
}


//-----------------------------------------------------------------------------
/*!
   \return color to use
*/
const QColor &GameWinDrawable::getColor()
{
   return mColor;
}


//-----------------------------------------------------------------------------
/*!
   \param color color to use
*/
void GameWinDrawable::setColorEnum(Constants::Color color)
{
   mColorEnum = color;
}


//-----------------------------------------------------------------------------
/*!
   \return color to use
*/
Constants::Color GameWinDrawable::getColorEnum() const
{
   return mColorEnum;
}


//-----------------------------------------------------------------------------
/*!
   \param name winner name
*/
void GameWinDrawable::setWinnerName(const QString &name)
{
   mWinnerName = name;
}


//-----------------------------------------------------------------------------
/*!
   \return winner name
*/
const QString &GameWinDrawable::getWinnerName()
{
   return mWinnerName;
}


//-----------------------------------------------------------------------------
/*!
   \param draw \c true if no winner
*/
void GameWinDrawable::setDrawGame(bool draw)
{
   mDrawGame = draw;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if no winner
*/
bool GameWinDrawable::isDrawGame() const
{
   return mDrawGame;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::playSound()
{
   if (isDrawGame())
      QTimer::singleShot(500, SoundManager::getInstance(), SLOT(playSoundGameDraw()));
   else
      QTimer::singleShot(500, SoundManager::getInstance(), SLOT(playSoundGameWin()));
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::stateChanged()
{
   if (
          GameStateMachine::getInstance()->getState()
       == Constants::GameStopped
   )
   {
      // if player pressed ESC, we don't have a valid game id anymore
      // also we don't want to see the bloody results screen.
      if (BombermanClient::getInstance()->isGameIdValid())
      {
         // fade out music
         SoundManager::getInstance()->fadeOut(1000);

         QList<PlayerInfo*> playerAlive;
         QList<PlayerInfo*> playerList =
            BombermanClient::getInstance()->getPlayerInfoList();

         // evaluate winner
         foreach (PlayerInfo* player, playerList)
         {
            if (!player->isKilled())
               playerAlive << player;
         }

         if (playerAlive.size() == 1)
         {
            setWinnerName(playerAlive[0]->getNick());

            // store winner color
            Constants::Color color = playerAlive[0]->getColor();

            setColorEnum(color);
            setColor(GameSettings::getInstance()->getStyleSettings()->getColor(color));

            // update player material with winner color
            // mPlayerMaterial->setColorMap(mPlayerTextures[color-1]);
         }

         // check if we have a draw game
         setDrawGame(playerAlive.size() != 1);

         // update game information ptr
         initGameData();

         // update player material depending on winner color
         // initializeWinnerScene();
         // initializePlayerMaterial();
         QTimer::singleShot(500, this, SLOT(startWinAnimation()));
         // startWinAnimation();

         playSound();

         // show winner screen
         setVisible(true);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(0, 1920, 1080, 0, -1.0f, 1.0f);
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::cleanupGlParameters()
{
   // enable blending
   glColor4f(1,1,1,1);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   QMap<QString, int> ranks;
   ranks.insert("p1-rank",  0);
   ranks.insert("p2-rank",  1);
   ranks.insert("p3-rank",  2);
   ranks.insert("p4-rank",  3);
   ranks.insert("p5-rank",  4);
   ranks.insert("p6-rank",  5);
   ranks.insert("p7-rank",  6);
   ranks.insert("p8-rank",  7);
   ranks.insert("p9-rank",  8);
   ranks.insert("p10-rank", 9);

   QMap<QString, Constants::Color> icons;
   icons.insert("white-icon",  Constants::ColorWhite );
   icons.insert("black-icon",  Constants::ColorBlack );
   icons.insert("red-icon",    Constants::ColorRed   );
   icons.insert("green-icon",  Constants::ColorGreen );
   icons.insert("blue-icon",   Constants::ColorBlue  );
   icons.insert("silver-icon", Constants::ColorGrey  );
   icons.insert("gold-icon",   Constants::ColorYellow);
   icons.insert("purple-icon", Constants::ColorPurple);
   icons.insert("cyan-icon",   Constants::ColorCyan  );
   icons.insert("orange-icon", Constants::ColorOrange);

   QMap<QString, int> names;
   names.insert("p1-name",  0);
   names.insert("p2-name",  1);
   names.insert("p3-name",  2);
   names.insert("p4-name",  3);
   names.insert("p5-name",  4);
   names.insert("p6-name",  5);
   names.insert("p7-name",  6);
   names.insert("p8-name",  7);
   names.insert("p9-name",  8);
   names.insert("p10-name", 9);

   QMap<QString, int> points;
   points.insert("p1-points",  0);
   points.insert("p2-points",  1);
   points.insert("p3-points",  2);
   points.insert("p4-points",  3);
   points.insert("p5-points",  4);
   points.insert("p6-points",  5);
   points.insert("p7-points",  6);
   points.insert("p8-points",  7);
   points.insert("p9-points",  8);
   points.insert("p10-points", 9);

   QMap<QString, int> bars;
   bars.insert("bar-bg-1",  0);
   bars.insert("bar-bg-2",  1);
   bars.insert("bar-bg-3",  2);
   bars.insert("bar-bg-4",  3);
   bars.insert("bar-bg-5",  4);
   bars.insert("bar-bg-6",  5);
   bars.insert("bar-bg-7",  6);
   bars.insert("bar-bg-8",  7);
   bars.insert("bar-bg-9",  8);
   bars.insert("bar-bg-10", 9);

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSDLayer* layer = new PSDLayer(mPsd.getLayer(l));;
      QString layerName = layer->getLayer()->getName();

      mPsdLayers << layer;

      // ranks
      QMap<QString, int>::const_iterator rankIterator = ranks.find(layerName);

      if (rankIterator != ranks.end())
         mRanks[rankIterator.value()] = layer;

      // icons
      QMap<QString, Constants::Color>::const_iterator iconsIterator = icons.find(layerName);

      if (iconsIterator != icons.end())
         mIcons[iconsIterator.value()-1] = layer;

      // names
      QMap<QString, int>::const_iterator namesIterator = names.find(layerName);

      if (namesIterator != names.end())
         mNames[namesIterator.value()] = layer;

      // points
      QMap<QString, int>::const_iterator pointsIterator = points.find(layerName);

      if (pointsIterator != points.end())
         mPoints[pointsIterator.value()] = layer;

      // backgrounds
      QMap<QString, int>::const_iterator barsIterator = bars.find(layerName);

      if (barsIterator != bars.end())
         mBars[barsIterator.value()] = layer;

      /*
         ranks:
            p1-rank
            ...

         icons:
            white-icon
            black-icon
            red-icon
            blue-icon
            green-icon
            silver-icon
            gold-icon
            purple-icon
            orange-icon
            cycan-icon

         names:
            p1-name
            ...

         points:
            p1-points
            ...

         backgrounds:
            bar-bg-1
            ...
      */
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::initializeWinnerScene()
{
   Mesh *mesh= MotionMixer::getMesh("bomberman");

   mesh->setUserTransformable(true);

   mPlayerMesh = new Mesh(mScene);
   mPlayerMesh->copy(*mesh);

   mMotionMixer = new MotionMixer();
   mPlayerMesh->setMotionMixer(mMotionMixer);
   mPlayerMesh->setVisible(true);
   mPlayerMesh->setUserTransformable(true);

   mPlayerMaterial->addMesh(mPlayerMesh);

   mPlayerItem = new PlayerItem(0, "winner", Constants::ColorCyan);
   mPlayerItem->setMesh(mPlayerMesh);
   mPlayerItem->setPosition(0.0f, 0.0f);
   mPlayerItem->setKilled(false);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::initializePlayerMaterial()
{
   delete mPlayerMaterial;
   mPlayerMaterial = 0;

   // todo: use current level path
   FileStream::addPath( "data/winner" );

   mPlayerMaterial = new PlayerMaterial(
      mScene,
      qPrintable(QString("player_%1").arg(Constants::ColorCyan)),
      "diffuse_level",
      "specular_level",
      "player-ao"
   );

   // load all textures
   TexturePool* pool= TexturePool::Instance();
   for (int i = 0; i < 10; i++)
   {
      mPlayerTextures[i] = pool->getTexture(
         qPrintable(QString("player_%1").arg(i+1))
      );
   }

   FileStream::removePath( "data/winner" );
}


//-----------------------------------------------------------------------------
/*!
   \param time current time
   \param dt delta time
*/
void GameWinDrawable::updateWinAnimation(float time, float dt)
{
   mPlayerItem->animate(time, dt);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameWinDrawable::startWinAnimation()
{
   mPlayerItem->win();
}


