// GLES3 port of client/src/game/gamewindrawable.cpp.

#include "gamewindrawable.h"

#include "gldevice.h"
#include "framework/framebuffer.h"
#include "postproduction/blurfilter.h"

#include "tools/filestream.h"

#include "menus/fontpool.h"
#include "menus/defaultshader.h"
#include "bombermanclient.h"
#include "gamesettings.h"
#include "playeritem.h"
#include "soundmanager.h"

#include "engine/animation/motionmixer.h"
#include "materials/material.h"
#include "materials/playermaterial.h"
#include "materials/materialfactory.h"
#include "materials/environmentambientmaterial.h"
#include "materials/environmentambientdiffusematerial.h"
#include "nodes/camera.h"
#include "nodes/mesh.h"
#include "nodes/scenegraph.h"
#include "render/texturepool.h"

#include <QTimer>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <math.h>

#define OFFSET_X_POINTS -43
#define OFFSET_Y_POINTS -2
#define OFFSET_Y_NAME -2
#define OFFSET_Y_ICON -20
#define CENTER_WIDTH_SCORE 130
#define RADIUS_MAX 200.0f

namespace
{
class CupMaterialFactory : public MaterialFactory
{
public:
   Material* createMaterial(SceneGraph* scene, int id) const override
   {
      Material* mat = nullptr;
      switch (id)
      {
         case (MAP_AMBIENT | MAP_REFLECT):
            mat = new EnvironmentAmbientMaterial(scene);
            break;
         case (MAP_AMBIENT | MAP_DIFFUSE | MAP_REFLECT):
            mat = new EnvironmentAmbientDiffuseMaterial(scene);
            break;
         default:
            break;
      }
      return mat;
   }
};
}  // namespace

GameWinDrawable::GameWinDrawable(RenderDevice* dev, bool visible)
    : QObject(),
      Drawable(dev, visible),
      mColorEnum(Constants::ColorWhite),
      mLargeFont(nullptr),
      mDefaultFont(nullptr),
      mScene(nullptr),
      mRenderTime(0.0f),
      mTime(0.0f),
      mDeltaTime(0.0f),
      mStartTime(0.0f),
      mDrawGame(false),
      mBlur(nullptr),
      mBackdropFb(nullptr),
      mSceneFb(nullptr),
      mSnapshotTexture(0),
      mGameInformation(nullptr),
      mPlayerItem(nullptr),
      mMotionMixer(nullptr),
      mPlayerMesh(nullptr),
      mPlayerMaterial(nullptr)
{
   mFilename = "data/game/results.psd";

   connect(GameStateMachine::getInstance(), SIGNAL(stateChanged()), this, SLOT(stateChanged()));
}

GameWinDrawable::~GameWinDrawable()
{
   delete mPlayerItem;
   delete mMotionMixer;
   delete mPlayerMaterial;
   delete mBlur;
   delete mBackdropFb;
   delete mSceneFb;

   // mPlayerMesh is not deleted here - it's owned by mScene (part of the player item)

   qDeleteAll(mPsdLayers);
   mPsdLayers.clear();
}

void GameWinDrawable::initializeGL()
{
   FileStream::addPath("data/cup");

   mLargeFont = FontPool::Instance()->get("large");
   mDefaultFont = FontPool::Instance()->get("large-outlined");

   mScene = new SceneGraph();
   CupMaterialFactory factory;
   mScene->load("cup.hjb", &factory);

   Node* node = mScene->getNode("Cup");
   node->setUserTransformable(true);

   mScene->getCamera()->setUserTransformable(true);

   mScene->render();

   FileStream::removePath("data/cup");

   mBlur = new BlurFilter();
   mBlur->init();

   glGenTextures(1, &mSnapshotTexture);

   initializeLayers();

   initializePlayerMaterial();
   initializeWinnerScene();
}

float GameWinDrawable::getContentsAlpha() const
{
   float alpha = 0.0f;
   float elapsed = mTime;

   if (elapsed < SHOW_WINNER_FADE_IN_TIME)
   {
      alpha = elapsed / static_cast<float>(SHOW_WINNER_FADE_IN_TIME);
   }
   else if (elapsed > (SHOW_WINNER_FADE_IN_TIME + SHOW_WINNER_DISPLAY_TIME + SHOW_WINNER_ADDITIONAL_TIME))
   {
      alpha = qMax(
         1.0f - ((elapsed - SHOW_WINNER_FADE_IN_TIME - SHOW_WINNER_DISPLAY_TIME - SHOW_WINNER_ADDITIONAL_TIME) /
                 static_cast<float>(SHOW_WINNER_FADE_OUT_TIME)),
         0.0f
      );
   }
   else
   {
      alpha = 1.0f;
   }

   return alpha;
}

float GameWinDrawable::getDrawableAlpha() const
{
   float alpha = 1.0f;
   float elapsed = mTime;

   float duration = SHOW_WINNER_FADE_IN_TIME + SHOW_WINNER_DISPLAY_TIME + SHOW_WINNER_ADDITIONAL_TIME + SHOW_WINNER_FADE_OUT_TIME;

   if (elapsed > duration)
   {
      alpha = qMax(1.0f - ((elapsed - duration) / static_cast<float>(SHOW_WINNER_SHOW_MENU_TIME)), 0.0f);
   }

   return alpha;
}

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

void GameWinDrawable::drawWinnerText()
{
   float alpha = getContentsAlpha();
   float yOffset = sin(mRenderTime * 0.05f) * 1080.0f * 0.015f;

   const float fontSize = 0.7f;

   mLargeFont->setColor(1.0f, 1.0f, 1.0f, alpha);

   if (isDrawGame())
   {
      mLargeFont->buildVertices(fontSize, qPrintable(tr("draw game")), 0.0f, 0.15f * 1080.0f + yOffset, 1920.0f);
      mLargeFont->draw();
   }
   else
   {
      QString winText = tr("%1 wins!").arg(getWinnerName());
      mLargeFont->buildVertices(fontSize, qPrintable(winText), 0.0f, 0.15f * 1080.0f + yOffset, 1920.0f);
      mLargeFont->draw();
   }
}

void GameWinDrawable::initGameData()
{
   mGameInformation = BombermanClient::getInstance()->getCurrentGameInformation();

   mPlayerScores.clear();
   QList<PlayerInfo*> infoList = BombermanClient::getInstance()->getPlayerInfoList();

   foreach (PlayerInfo* info, infoList)
   {
      Weighted<PlayerInfo*, int> w(info, computeScore(info));
      mPlayerScores.push_back(w);
   }

   std::sort(mPlayerScores.begin(), mPlayerScores.end());

   hideLayers();

   int row = 0;
   for (const Weighted<PlayerInfo*, int>& w : mPlayerScores)
   {
      PlayerInfo* info = w.getObject();
      Constants::Color color = info->getColor();
      int colorIndex = color - 1;

      qDebug("GameWinDrawable::initGameData(): #%d: %s", w.getWeight(), qPrintable(info->getNick()));

      PSDLayer* rankLayer = mRanks[row];
      PSDLayer* nameLayer = mNames[row];
      PSDLayer* barLayer = mBars[row];
      PSDLayer* iconLayer = mIcons[colorIndex];

      iconLayer->getLayer()->setY(nameLayer->getTop() + OFFSET_Y_ICON);

      rankLayer->getLayer()->setVisible(true);
      barLayer->getLayer()->setVisible(true);
      iconLayer->getLayer()->setVisible(true);

      row++;
   }

   memset(mPlayerScoresAnimated, 0, 10 * sizeof(float));
}

int GameWinDrawable::computeScore(PlayerInfo* info) const
{
   PlayerStats ps = info->getRoundStats();

   float score = 500.0f * ps.getWins() + 10.0f * ps.getKills() + 5.0f * ps.getExtrasCollected() + 0.25f * ps.getSurvivalTime();

   score *= 0.1f;

   return static_cast<int32_t>(score);
}

void GameWinDrawable::drawGameData()
{
   int row = 0;
   for (const Weighted<PlayerInfo*, int>& w : mPlayerScores)
   {
      PlayerInfo* info = w.getObject();

      PSDLayer* nameLayer = mNames[row];
      PSDLayer* pointsLayer = mPoints[row];

      mDefaultFont->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      mDefaultFont->buildVertices(0.27f, qPrintable(info->getNick()), nameLayer->getLeft(), nameLayer->getBottom() + OFFSET_Y_NAME);
      mDefaultFont->draw();

      const int colorIndex = static_cast<int32_t>(info->getColor()) - 1;
      mPlayerScoresAnimated[colorIndex] += mDeltaTime;
      const int score = static_cast<int32_t>(qMin(mPlayerScoresAnimated[colorIndex], static_cast<float>(computeScore(info))));

      mDefaultFont->buildVertices(
         0.27f, qPrintable(QString("%1").arg(score)), pointsLayer->getLeft() + OFFSET_X_POINTS, pointsLayer->getBottom() + OFFSET_Y_POINTS,
         CENTER_WIDTH_SCORE
      );
      mDefaultFont->draw();

      row++;
   }
}

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

float GameWinDrawable::getRadius() const
{
   float radius = mRenderTime * 0.5f;

   if (radius > RADIUS_MAX)
      radius = RADIUS_MAX;

   float fadeOutStart = static_cast<float>(SHOW_WINNER_FADE_IN_TIME + SHOW_WINNER_DISPLAY_TIME + SHOW_WINNER_ADDITIONAL_TIME + SHOW_WINNER_FADE_OUT_TIME);

   if (mTime > fadeOutStart)
   {
      radius = qMax(1.0f - ((mTime - fadeOutStart) / static_cast<float>(SHOW_WINNER_SHOW_MENU_TIME)), 0.0f);
      radius *= RADIUS_MAX;
   }

   return radius;
}

void GameWinDrawable::drawBackBuffer(float alpha)
{
   const int width = activeDevice->getWidth();
   const int height = activeDevice->getHeight();

   // snapshot the just-rendered frame into a plain (non-FBO-attached) texture - BlurFilter reads
   // from this while writing its result into mBackdropFb below; sampling and writing the same
   // live FBO attachment at once would be an undefined feedback loop.
   glBindTexture(GL_TEXTURE_2D, mSnapshotTexture);
   glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   if (!mBackdropFb)
      mBackdropFb = new FrameBuffer(width, height, 0, FrameBuffer::NoDepthBuffer);
   else
      mBackdropFb->setResolution(width, height);

   const float radius = getRadius();

   mBackdropFb->bind();
   mBlur->setAlpha(1.0f);
   mBlur->setRadius(radius);
   mBlur->process(mSnapshotTexture, 1, 1);
   mBackdropFb->unbind();

   // composite the blurred backdrop onto the real screen - whole-rect fade, not per-pixel alpha
   // (matches getFramebufferBlitShader()'s documented "replace alpha" use case).
   activeDevice->setShader(getFramebufferBlitShader());
   static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(Matrix());
   activeDevice->push(Matrix());
   activeDevice->setParameter(getFramebufferBlitShaderAlphaParam(), alpha);
   mBackdropFb->draw(alpha);
   activeDevice->pop();
   activeDevice->setShader(0);
}

void GameWinDrawable::drawScene()
{
   if (!isDrawGame())
   {
      // every material which has not been used yet restores its own textures the first time it's
      // used - so after setting a different color map earlier, it reverts to the one it was
      // created with. resetting the color map after we know the material has been used is the
      // simplest workaround.
      mPlayerMaterial->setColorMap(mPlayerTextures[getColorEnum() - 1]);

      // after the fov was fixed to match the 3dsmax settings, it's too narrow in this scene -
      // compensate manually.
      Matrix scale = Matrix::scale(0.75f, 0.75f, 1.0f);
      mScene->render(mRenderTime, scale);
   }
}

void GameWinDrawable::drawSceneToFramebuffer(float alpha)
{
   const int width = activeDevice->getWidth();
   const int height = activeDevice->getHeight();

   if (!mSceneFb)
      mSceneFb = new FrameBuffer(width, height, 0, 0);
   else
      mSceneFb->setResolution(width, height);

   mSceneFb->bind();
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   drawScene();
   mSceneFb->unbind();

   // composite on top of the already-drawn blurred backdrop - real per-pixel alpha this time
   // (transparent gaps around the cup/player let the backdrop underneath show through).
   activeDevice->setShader(getDefaultMenuShader());
   static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(Matrix());
   activeDevice->push(Matrix());
   activeDevice->setParameter(getDefaultMenuShaderAlphaParam(), alpha);
   mSceneFb->draw(alpha);
   activeDevice->pop();
   activeDevice->setShader(0);
}

void GameWinDrawable::drawPsdContents(float alpha)
{
   initGlParameters();
   drawWinnerText();
   drawLayers(alpha);
   drawGameData();
   cleanupGlParameters();
}

void GameWinDrawable::paintGL()
{
   float alpha = getDrawableAlpha();

   drawBackBuffer(alpha);
   drawSceneToFramebuffer(alpha);
   drawPsdContents(alpha);

   if (alpha == 0.0f)
      Drawable::setVisible(false);
}

void GameWinDrawable::setVisible(bool visible)
{
   if (visible)
      mStartTime = 0.0f;

   Drawable::setVisible(visible);
}

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

   mPlayerItem->animate(mRenderTime, mDeltaTime);

   Matrix cupTransform = mScene->getNode("Cup")->getTransform();

   // move player elsewhere
   cupTransform.xw = 200.0f;
   cupTransform.yw = 0.0f;
   cupTransform.zw = 50.0f;
   cupTransform.ww = 1.0f;
   Matrix rotzc = Matrix::rotateZ(0.005f * sin(0.01f * time));
   Matrix rotyc = Matrix::rotateY(0.001f * sin(0.01f * time));
   mScene->getNode("Cup")->setTransform(rotyc * rotzc * cupTransform);

   float scale = 365;
   Matrix playerMatrix = Matrix::scale(scale, scale, scale);
   playerMatrix.xw = 345.0f;
   playerMatrix.yw = 0.0f;
   playerMatrix.zw = -40.0f;
   playerMatrix.ww = 1.0f;

   Matrix rotz = Matrix::rotateZ(static_cast<float>(M_PI));
   Matrix rotx = Matrix::rotateY(0.11f);
   playerMatrix = playerMatrix * rotz * rotx;

   mPlayerMesh->setUserTransformable(true);
   mPlayerMesh->setTransform(playerMatrix);
}

void GameWinDrawable::setColor(const QColor& color)
{
   mColor = color;
}

const QColor& GameWinDrawable::getColor()
{
   return mColor;
}

void GameWinDrawable::setColorEnum(Constants::Color color)
{
   mColorEnum = color;
}

Constants::Color GameWinDrawable::getColorEnum() const
{
   return mColorEnum;
}

void GameWinDrawable::setWinnerName(const QString& name)
{
   mWinnerName = name;
}

const QString& GameWinDrawable::getWinnerName()
{
   return mWinnerName;
}

void GameWinDrawable::setDrawGame(bool draw)
{
   mDrawGame = draw;
}

bool GameWinDrawable::isDrawGame() const
{
   return mDrawGame;
}

void GameWinDrawable::playSound()
{
   if (isDrawGame())
      QTimer::singleShot(500, SoundManager::getInstance(), SLOT(playSoundGameDraw()));
   else
      QTimer::singleShot(500, SoundManager::getInstance(), SLOT(playSoundGameWin()));
}

void GameWinDrawable::stateChanged()
{
   if (GameStateMachine::getInstance()->getState() == Constants::GameStopped)
   {
      // if the player pressed ESC, there's no valid game id anymore - and no results screen to show.
      if (BombermanClient::getInstance()->isGameIdValid())
      {
         SoundManager::getInstance()->fadeOut(1000);

         QList<PlayerInfo*> playerAlive;
         QList<PlayerInfo*> playerList = BombermanClient::getInstance()->getPlayerInfoList();

         foreach (PlayerInfo* player, playerList)
         {
            if (!player->isKilled())
               playerAlive << player;
         }

         if (playerAlive.size() == 1)
         {
            setWinnerName(playerAlive[0]->getNick());

            Constants::Color color = playerAlive[0]->getColor();
            setColorEnum(color);
            setColor(GameSettings::getInstance()->getStyleSettings()->getColor(color));
         }

         setDrawGame(playerAlive.size() != 1);

         initGameData();

         QTimer::singleShot(500, this, SLOT(startWinAnimation()));

         playSound();

         setVisible(true);
      }
   }
}

void GameWinDrawable::initGlParameters()
{
   Matrix ortho = Matrix::ortho(0, 1920, 1080, 0, -1.0f, 1.0f);
   static_cast<GLDevice*>(activeDevice)->setProjectionMatrix(ortho);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   activeDevice->setShader(0);
}

void GameWinDrawable::cleanupGlParameters()
{
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

void GameWinDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   QMap<QString, int> ranks;
   ranks.insert("p1-rank", 0);
   ranks.insert("p2-rank", 1);
   ranks.insert("p3-rank", 2);
   ranks.insert("p4-rank", 3);
   ranks.insert("p5-rank", 4);
   ranks.insert("p6-rank", 5);
   ranks.insert("p7-rank", 6);
   ranks.insert("p8-rank", 7);
   ranks.insert("p9-rank", 8);
   ranks.insert("p10-rank", 9);

   QMap<QString, Constants::Color> icons;
   icons.insert("white-icon", Constants::ColorWhite);
   icons.insert("black-icon", Constants::ColorBlack);
   icons.insert("red-icon", Constants::ColorRed);
   icons.insert("green-icon", Constants::ColorGreen);
   icons.insert("blue-icon", Constants::ColorBlue);
   icons.insert("silver-icon", Constants::ColorGrey);
   icons.insert("gold-icon", Constants::ColorYellow);
   icons.insert("purple-icon", Constants::ColorPurple);
   icons.insert("cyan-icon", Constants::ColorCyan);
   icons.insert("orange-icon", Constants::ColorOrange);

   QMap<QString, int> names;
   names.insert("p1-name", 0);
   names.insert("p2-name", 1);
   names.insert("p3-name", 2);
   names.insert("p4-name", 3);
   names.insert("p5-name", 4);
   names.insert("p6-name", 5);
   names.insert("p7-name", 6);
   names.insert("p8-name", 7);
   names.insert("p9-name", 8);
   names.insert("p10-name", 9);

   QMap<QString, int> points;
   points.insert("p1-points", 0);
   points.insert("p2-points", 1);
   points.insert("p3-points", 2);
   points.insert("p4-points", 3);
   points.insert("p5-points", 4);
   points.insert("p6-points", 5);
   points.insert("p7-points", 6);
   points.insert("p8-points", 7);
   points.insert("p9-points", 8);
   points.insert("p10-points", 9);

   QMap<QString, int> bars;
   bars.insert("bar-bg-1", 0);
   bars.insert("bar-bg-2", 1);
   bars.insert("bar-bg-3", 2);
   bars.insert("bar-bg-4", 3);
   bars.insert("bar-bg-5", 4);
   bars.insert("bar-bg-6", 5);
   bars.insert("bar-bg-7", 6);
   bars.insert("bar-bg-8", 7);
   bars.insert("bar-bg-9", 8);
   bars.insert("bar-bg-10", 9);

   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSDLayer* layer = new PSDLayer(mPsd.getLayer(l));
      QString layerName = layer->getLayer()->getName();

      mPsdLayers << layer;

      auto rankIterator = ranks.find(layerName);
      if (rankIterator != ranks.end())
         mRanks[rankIterator.value()] = layer;

      auto iconsIterator = icons.find(layerName);
      if (iconsIterator != icons.end())
         mIcons[iconsIterator.value() - 1] = layer;

      auto namesIterator = names.find(layerName);
      if (namesIterator != names.end())
         mNames[namesIterator.value()] = layer;

      auto pointsIterator = points.find(layerName);
      if (pointsIterator != points.end())
         mPoints[pointsIterator.value()] = layer;

      auto barsIterator = bars.find(layerName);
      if (barsIterator != bars.end())
         mBars[barsIterator.value()] = layer;
   }
}

void GameWinDrawable::initializeWinnerScene()
{
   Mesh* mesh = MotionMixer::getMesh("bomberman");
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

void GameWinDrawable::initializePlayerMaterial()
{
   delete mPlayerMaterial;
   mPlayerMaterial = nullptr;

   FileStream::addPath("data/winner");

   mPlayerMaterial = new PlayerMaterial(mScene, qPrintable(QString("player_%1").arg(Constants::ColorCyan)), "diffuse_level", "specular_level", "player-ao");

   TexturePool* pool = TexturePool::Instance();
   for (int i = 0; i < 10; i++)
   {
      mPlayerTextures[i] = pool->getTexture(qPrintable(QString("player_%1").arg(i + 1)));
   }

   FileStream::removePath("data/winner");
}

void GameWinDrawable::updateWinAnimation(float time, float dt)
{
   mPlayerItem->animate(time, dt);
}

void GameWinDrawable::startWinAnimation()
{
   mPlayerItem->win();
}
