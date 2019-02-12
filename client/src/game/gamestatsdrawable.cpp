#include "gamestatsdrawable.h"

// game
#include "fontpool.h"
#include "bombermanclient.h"
#include "gamesettings.h"
#include "gamestatemachine.h"
#include "soundmanager.h"

// shared
#include "playerinfo.h"

// menus
#include "psdlayer.h"

// framework
#include "gldevice.h"
#include "globaltime.h"

// Qt
#include <QColor>
#include <QGLContext>
#include <QImage>

#include <cstdint>
#include <math.h>

// various defines
#define IMAGE_ORB           "data/game/game_orb.png"

// layers
#define LAYER_PLAYER_ICON           "player_icon_"
#define LAYER_PLAYER_NAME           "player_name"
#define LAYER_PLAYER_SCORE          "player_score"
#define LAYER_GAME_TIME_PANEL_ORB   "time-panel-orb"
#define LAYER_GAME_TIME_ORB         "time-orb"
#define LAYER_GAME_TIME             "time"
#define LAYER_MUTE_MUSIC            "music-mute-icon"
#define LAYER_MUTE_SFX              "sfx-mute-icon"
#define LAYER_SKULL_BLUE            "blue-skull"
#define LAYER_SKULL_RED             "red-skull"
#define LAYER_SKULL_PURPLE          "purple-skull"
#define LAYER_SKULL_GREEN           "green-skull"
#define LAYER_SKULL_GOLD            "gold-skull"
#define LAYER_SKULL_ALL             "all-skull"
#define LAYER_SKULL_PROGRESS_START  "progressbar-start"
#define LAYER_SKULL_PROGRESS_MIDDLE "progressbar-middle"

// offsets and scales
#define PLAYER_ICON_OFFSET     10
#define OFFSET_Y_PLAYER_NICK   15
#define OFFSET_X_PLAYER_WINS   4
#define OFFSET_Y_PLAYER_WINS   10
#define OFFSET_X_PLAYER_KILLS  40
#define OFFSET_Y_PLAYER_KILLS  35
#define OFFSET_X_PLAYER_DEATHS -40
#define OFFSET_Y_PLAYER_DEATHS 35
#define FONT_SCALE_NICK        0.085f
#define FONT_SCALE_WINS        0.15f
#define FONT_SCALE_KILLS       0.085f
#define FONT_SCALE_DEATHS      0.085f


//-----------------------------------------------------------------------------
/*!
   \param dev render device
*/
GameStatsDrawable::GameStatsDrawable(RenderDevice* dev)
 : QObject(),
   Drawable(dev),
   mTimeFont(nullptr),
   mOutline(nullptr),
   mScaleFactor(0.0f),
   mLayerOrb(nullptr),
   mLayerPanelOrb(nullptr),
   mLayerMuteMusic(nullptr),
   mLayerMuteSfx(nullptr),
   mLayerSkullProgressStart(nullptr),
   mLayerSkullProgressMiddle(nullptr),
   mPlayerScore(0),
   mTimeLeft(0),
   mDuration(0),
   mPlayerNameOffsetX(0.0f),
   mPlayerNameOffsetY(0.0f),
   mPlayerScoreOffsetX(0.0f),
   mPlayerScoreOffsetY(0.0f),
   mGameTimeOffsetX(0.0f),
   mGameTimeOffsetY(0.0f),
   mGameTimeHeight(0.0f),
   mGrayscaleShader(0),
   mParamGrayscale(-1),
   mLastGlobalTime(0.0f),
   mSkullType(Constants::SkullReset),
   mSkullDuration(0.0f),
   mSkullAnimationStartTime(0.0f),
   mSkullAlpha(0.0f),
   mSkullAborted(true),
   mProgressShader(0),
   mParamProgress(0),
   mProgressBarWidth(0)
{
   memset(mGrayscales, 0.0f, 11 * sizeof(float));

   for (int i=0; i < 10; i++)
      mLayersPlayer[i]=nullptr;

   for (int i=0; i < Constants::SkullReset + 1; i++)
      mLayersSkulls[i]=nullptr;

   mFilename = "data/game/gameui.psd";
}


//-----------------------------------------------------------------------------
/*!
*/
GameStatsDrawable::~GameStatsDrawable()
{
   qDeleteAll(mRenderLayers);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::initializeGL()
{
   connect(
      GameStateMachine::getInstance(),
      SIGNAL(stateChanged()),
      this,
      SLOT(resetGrayscales())
   );

   // init font
   mTimeFont = FontPool::Instance()->get("time");
   mOutline = FontPool::Instance()->get("outlined");

   // init layers
   initializeLayers();
   initializeOrb();

   // init grayscale shader
   GLDevice* dev = dynamic_cast<GLDevice*>(activeDevice);

   // grayscale shader
   mGrayscaleShader = dev->loadShader("grayscale-vert.glsl", "grayscale-frag.glsl");
   mParamGrayscale = activeDevice->getParameterIndex("intensity");

   // progress shader
   mProgressShader = dev->loadShader("progressbar-vert.glsl", "progressbar-frag.glsl");
   mParamProgress = activeDevice->getParameterIndex("progress");

}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* layer = mPsd.getLayer(l);

      PSDLayer* renderLayer= new PSDLayer(layer);

      mRenderLayers << renderLayer;

      // qDebug("layername: %s", layer->getName());

      // player icons

      if (QString(layer->getName()).startsWith(LAYER_PLAYER_ICON))
      {
         int number =
            (QString(layer->getName()).replace(LAYER_PLAYER_ICON, "").toInt() - 1);

         mLayersPlayer[number] = renderLayer;
      }

      // time and orb

      else if (QString::compare(layer->getName(), LAYER_GAME_TIME) == 0)
      {
         mGameTimeOffsetX = layer->getLeft();
         mGameTimeOffsetY = layer->getTop() + layer->getHeight();
         mGameTimeHeight = layer->getHeight();
      }

      else if (QString::compare(layer->getName(), LAYER_GAME_TIME_ORB) == 0)
      {
         mLayerOrb = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_GAME_TIME_PANEL_ORB) == 0)
      {
         mLayerPanelOrb = renderLayer;
      }

      // mute buttons

      else if (QString::compare(layer->getName(), LAYER_MUTE_MUSIC) == 0)
      {
         mLayerMuteMusic = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_MUTE_SFX) == 0)
      {
         mLayerMuteSfx = renderLayer;
      }

      // skulls

      else if (QString::compare(layer->getName(), LAYER_SKULL_RED) == 0)
      {
         mLayersSkulls[Constants::SkullAutofire] = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_SKULL_BLUE) == 0)
      {
         mLayersSkulls[Constants::SkullMinimumBomb] = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_SKULL_PURPLE) == 0)
      {
         mLayersSkulls[Constants::SkullKeyboardInvert] = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_SKULL_ALL) == 0)
      {
         mLayersSkulls[Constants::SkullMushroom] = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_SKULL_GREEN) == 0)
      {
         mLayersSkulls[Constants::SkullInvisible] = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_SKULL_GOLD) == 0)
      {
         mLayersSkulls[Constants::SkullInvincible] = renderLayer;
      }

      // skull progressbar

      else if (QString::compare(layer->getName(), LAYER_SKULL_PROGRESS_START) == 0)
      {
         mLayerSkullProgressStart = renderLayer;
      }

      else if (QString::compare(layer->getName(), LAYER_SKULL_PROGRESS_MIDDLE) == 0)
      {
         mLayerSkullProgressMiddle = renderLayer;
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::initializeOrb()
{
   mOrbPalette.load(IMAGE_ORB);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::paintGL()
{
   initGlParameters();

   drawOverlay();
   drawPlayerIcons();
   drawPlayerInfo();
   drawGameTime();
   drawSkullProgress();
   drawOrb();
   drawSkull();
   drawMuteIcons();

   cleanupGlParameters();
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void GameStatsDrawable::animateDeathGrayScales(float dt)
{
   QList<PlayerInfo*> playerInfoList =
      BombermanClient::getInstance()->getPlayerInfoList();

   foreach(PlayerInfo* info, playerInfoList)
   {
      if (info->isKilled())
      {
         // ensure the next grayscale factor does not exceed 1
         float val = mGrayscales[info->getColor()];
         float nextVal = qMin(1.0f, val + 0.01f * dt);

         mGrayscales[info->getColor()] = nextVal;
      }
   }
}


//-----------------------------------------------------------------------------
/*!
   \param globalTime global time
*/
void GameStatsDrawable::animate(float globalTime)
{
   if (fabs(mLastGlobalTime - globalTime) > 0.0001f)
   {
      if (mLastGlobalTime != 0.0f)
      {
         float dt = globalTime - mLastGlobalTime;

         animateDeathGrayScales(dt);
         animateSkulls(dt);
      }

      mLastGlobalTime = globalTime;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawOverlay()
{
   for (int layerIndex = 0; layerIndex < mPsd.getLayerCount(); layerIndex++)
   {
      PSDLayer* psdLayer = mRenderLayers[layerIndex];

      if (psdLayer->getLayer()->isVisible())
      {
         psdLayer->render();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawPlayerIcons()
{
   GLDevice* dev= dynamic_cast<GLDevice*>(activeDevice);

   glPushMatrix();

   QList<PlayerInfo*> playerInfoList =
      BombermanClient::getInstance()->getPlayerInfoList();

   foreach(PlayerInfo* info, playerInfoList)
   {
      int color = static_cast<int32_t>(info->getColor()) - 1;
      if (color > -1 && color < 10)
      {
         if (info->isKilled())
         {
            dev->setShader(mGrayscaleShader);
            activeDevice->setParameter(
               mParamGrayscale,
               mGrayscales[info->getColor()]
            );
         }

         PSDLayer* layer = mLayersPlayer[color];
         layer->render();

         if (info->isKilled())
         {
            dev->setShader(0);
         }

         glTranslatef(-(layer->getWidth() + PLAYER_ICON_OFFSET), 0.0f, 0.0f);
      }
   }

   glPopMatrix();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawPlayerInfo()
{
   glPushMatrix();

   QList<PlayerInfo*> playerInfoList =
      BombermanClient::getInstance()->getPlayerInfoList();

   int i = 0;
   foreach(PlayerInfo* info, playerInfoList)
   {
      PSDLayer* layer = mLayersPlayer[i];

      // get grey value for dead players
      float grayScale = 0.0f;
      float color = 1.0f;
      if (info->isKilled())
      {
         grayScale = qMin(1.0f, mGrayscales[info->getColor()]);
         color = color - grayScale;
      }

      // nick
      mOutline->buildVertices(
         FONT_SCALE_NICK,
         qPrintable( info->getNick() ),
         layer->getLeft(),
         layer->getTop() + OFFSET_Y_PLAYER_NICK,
         layer->getWidth()
      );

      mOutline->draw();

      PlayerStats stats = info->getOverallStats();

      // wins
      QString statsWins = QString("%1").arg(stats.getWins());

      mOutline->buildVertices(
         FONT_SCALE_WINS,
         qPrintable(statsWins),
         layer->getLeft(),
         layer->getTop() + layer->getHeight() + OFFSET_Y_PLAYER_WINS,
         layer->getWidth()
      );

      mOutline->draw();

      // deaths
      QString statsDeaths = QString("%1").arg(stats.getDeaths());

      mOutline->buildVertices(
         FONT_SCALE_DEATHS,
         qPrintable(statsDeaths),
         layer->getLeft() + OFFSET_X_PLAYER_DEATHS,
         layer->getTop() + OFFSET_Y_PLAYER_DEATHS,
         layer->getWidth()
      );

      glColor3f(
         color * 0.9f + grayScale * 0.5f,
         color * 0.1f + grayScale * 0.5f,
         color * 0.0f + grayScale * 0.5f
      );

      mOutline->draw();

      // kills
      QString statsKills = QString("%1").arg(stats.getKills());

      mOutline->buildVertices(
         FONT_SCALE_DEATHS,
         qPrintable(statsKills),
         layer->getLeft() + OFFSET_X_PLAYER_KILLS,
         layer->getTop() + OFFSET_Y_PLAYER_KILLS,
         layer->getWidth()
      );

      glColor3f(
         color * 0.9f + grayScale * 0.5f,
         color * 0.9f + grayScale * 0.5f,
         color * 0.0f + grayScale * 0.5f
      );

      mOutline->draw();

      // reset color
      glColor3f(1.0f, 1.0f, 1.0f);

      i++;

      glTranslatef(-(layer->getWidth() + PLAYER_ICON_OFFSET), 0.0f, 0.0f);
   }

   glPopMatrix();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawGameTime()
{
   int mins = mTimeLeft / 60;

   QString timeString;
   timeString.sprintf(
      "%02d:%02d",
      mins,
      mTimeLeft - (mins * 60)
   );

   mTimeFont->buildVertices(
      0.30f,
      qPrintable(timeString),
      mGameTimeOffsetX,
      mGameTimeOffsetY,
      -1,
      mGameTimeHeight
   );

   mTimeFont->draw();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawOrb()
{
   mLayerPanelOrb->render();

   float duration = qMax(static_cast<float>(mDuration), 1.0f);

   QRgb pixel =
      mOrbPalette.pixel(
         qMin(
            mOrbPalette.width() - 1,
            static_cast<int32_t>(((mDuration - mTimeLeft) / duration) * 255.0f)
         ),
         0
      );

   mLayerOrb->setColor(
      qRed(pixel)   / 255.0f,
      qGreen(pixel) / 255.0f,
      qBlue(pixel)  / 255.0f
   );

   mLayerOrb->render();
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawMuteIcons()
{
   mLayerMuteMusic->getLayer()->setVisible(
      SoundManager::getInstance()->isMusicMuted()
   );

   mLayerMuteSfx->getLayer()->setVisible(
      SoundManager::getInstance()->isSfxMuted()
   );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::initGlParameters()
{
   Matrix ortho= Matrix::ortho(
      0.0f,
      mPsd.getWidth(),
      mPsd.getHeight(),
      0.0f,
      -1.0f,
      1.0f
   );
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::setPlayerScore(int score)
{
   mPlayerScore = score;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::setGameTimeLeft(int time, int duration)
{
   mTimeLeft = time;
   mDuration = duration;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::resetGrayscales()
{
   if (
          GameStateMachine::getInstance()->getState()
       == Constants::GamePreparing
   )
   {
      memset(mGrayscales, 0, 11 * sizeof(float));
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::unitTest1()
{
   setPlayerScore(1000);
   setGameTimeLeft(240, 240);
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void GameStatsDrawable::animateSkulls(float dt)
{
   if (mSkullAborted)
      mSkullAlpha -= dt * 0.01f;
}


//-----------------------------------------------------------------------------
/*!
   \return skull progress
*/
float GameStatsDrawable::getSkullProgress() const
{
   float progress = 0.0f;

   if (mSkullAborted)
   {
      progress = 1.0f;
   }
   else
   {
      float diff = (GlobalTime::Instance()->getTime() - mSkullAnimationStartTime);
      progress = diff / mSkullDuration;
   }

   return progress;
}


//-----------------------------------------------------------------------------
/*!
   \param skullType player's skull type
*/
void GameStatsDrawable::setSkullType(Constants::SkullType skullType)
{
   mSkullType = skullType;
}


//-----------------------------------------------------------------------------
/*!
   \return player's skull type
*/
Constants::SkullType GameStatsDrawable::getSkullType() const
{
   return mSkullType;
}


//-----------------------------------------------------------------------------
/*!
   \param visible visible flag
*/
void GameStatsDrawable::setVisible(bool visible)
{
   Drawable::setVisible(visible);

   if (visible)
   {
      // initialize clock with a proper value
      GameInformation* info =
         BombermanClient::getInstance()->getCurrentGameInformation();

      if (info)
      {
         int duration = info->getDuration();
         setGameTimeLeft(duration, duration);
      }
   }
   else
   {
      mSkullAlpha = 0.0f;
      mSkullAborted = true;
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawSkull()
{
   if (mSkullAlpha > 0)
   {
      PSDLayer* skull = mLayersSkulls[mSkullType];

      if (skull)
      {
         skull->render(0.0f, 0.0f, mSkullAlpha);
      }
   }
}


//-----------------------------------------------------------------------------
/*!
  \param id infected player id
  \param skullType skull type
  \param infectorId id of infector
  \param extraX skull position x
  \param extraY skull position y
*/
void GameStatsDrawable::playerInfected(
   int id,
   Constants::SkullType skullType,
   int infectorId,
   int extraX,
   int extraY
)
{
   Q_UNUSED(infectorId);
   Q_UNUSED(extraX);
   Q_UNUSED(extraY);

   if (id == BombermanClient::getInstance()->getPlayerId())
   {
      bool aborted = (skullType == Constants::SkullReset);

      mSkullAborted = aborted;

      if (!aborted)
      {
         mSkullAlpha = 1.0f;
         mSkullType = skullType;
         mSkullDuration = 0.001f * SERVER_SKULL_DURATION;
         mSkullAnimationStartTime = GlobalTime::Instance()->getTime();
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GameStatsDrawable::drawSkullProgress()
{
   float progress = getSkullProgress();

   activeDevice->setShader(mProgressShader);
   activeDevice->setParameter(
      mParamProgress,
      progress
   );

   mLayerSkullProgressMiddle->render();

   activeDevice->setShader(0);

   float offset = -(1.0f - progress) * mLayerSkullProgressMiddle->getWidth();
   mLayerSkullProgressStart->render(offset);
}


