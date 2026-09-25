// GLES3 port of client/src/game/gameplayernamedisplay.cpp.

#include "gameplayernamedisplay.h"

#include "gldevice.h"

#include "bombermanclient.h"
#include "menus/fontpool.h"
#include "playeritem.h"

#include "framework/globaltime.h"

#include "math/matrix.h"

#include "nodes/mesh.h"

#include "render/texturepool.h"

#include <math.h>

#define FONT_DISPLAY_DURATION 8000
#define FONT_DISPLAY_FADE_DURATION 3000
#define FONT_Y_OFFSET 30
#define ARROW_DISPLAY_DURATION 3000
#define ARROW_DISPLAY_FADE_DURATION 2000

namespace
{
struct ArrowVertex
{
   float x, y, z;
   float u, v;
};
}  // namespace

GamePlayerNameDisplay::GamePlayerNameDisplay(QObject* parent)
    : QObject(parent),
      mFont(nullptr),
      mShowArrow(false),
      mArrowShader(0),
      mArrowVertexBuffer(0),
      mArrowIndexBuffer(0),
      mArrowParamTexture(-1),
      mArrowParamAlpha(-1)
{
}

bool GamePlayerNameDisplay::isActive() const
{
   return (mActiveTime.elapsed() < (FONT_DISPLAY_DURATION + FONT_DISPLAY_FADE_DURATION));
}

void GamePlayerNameDisplay::setPlayerData(QMap<int, PlayerItem*>& players)
{
   Matrix projMat = static_cast<GLDevice*>(activeDevice)->getProjectionMatrix();

   const int width = 1920;
   const int height = 1080;

   mPositions.clear();
   mNames.clear();

   foreach (PlayerItem* player, players)
   {
      if (!player->isKilled())
      {
         Matrix mat = player->getMesh()->getTransform() * projMat;
         mat = mat.xyw();

         Vector v = mat * Vector(0.0f, 0.0f, 0.0f);

         // 2d transform
         float t = 1.0f / v.z;

         v.x = (v.x * t + 1.0f) * 0.5f * width;
         v.y = (-v.y * t + 1.0f) * 0.5f * height;
         v.z = 0.0f;

         mPositions << v;
         mNames << player->getNick();

         if (player->getID() == BombermanClient::getInstance()->getPlayerId())
         {
            mArrowPosition = player->getPosition();
         }
      }
   }
}

void GamePlayerNameDisplay::start()
{
   mActiveTime.restart();

   // show arrow only in large maps
   mShowArrow = false;
   GameInformation* info = BombermanClient::getInstance()->getCurrentGameInformation();

   switch (info->getMapDimensions())
   {
      case Constants::Dimension19x17:
      case Constants::Dimension25x21:
         mShowArrow = true;
         break;

      default:
         break;
   }
}

void GamePlayerNameDisplay::initialize()
{
   mFont = FontPool::Instance()->get("outlined");

   TexturePool* pool = TexturePool::Instance();
   mArrowTexture = pool->getTexture("data/game/arrow");

   mArrowShader = activeDevice->loadShader("texalpha-vert.glsl", "texalpha-frag.glsl");
   mArrowParamTexture = activeDevice->getParameterIndex("tex");
   mArrowParamAlpha = activeDevice->getParameterIndex("alpha");

   // local quad in the arrow's own X/Z plane (Y fixed at 0) - positioned and animated per frame
   // via activeDevice->push(Matrix::position(...)) instead of rewriting vertex data every draw.
   const float width = 0.75f;
   const float height = 0.75f;

   mArrowVertexBuffer = activeDevice->createVertexBuffer(4 * sizeof(ArrowVertex));
   ArrowVertex* vtx = (ArrowVertex*)activeDevice->lockVertexBuffer(mArrowVertexBuffer);
   vtx[0] = {width, 0.0f, -height, 1.0f, 1.0f};
   vtx[1] = {width, 0.0f, height, 1.0f, 0.0f};
   vtx[2] = {-width, 0.0f, height, 0.0f, 0.0f};
   vtx[3] = {-width, 0.0f, -height, 0.0f, 1.0f};
   activeDevice->unlockVertexBuffer(mArrowVertexBuffer);

   mArrowIndexBuffer = activeDevice->createIndexBuffer(6 * sizeof(unsigned short));
   unsigned short* idx = (unsigned short*)activeDevice->lockIndexBuffer(mArrowIndexBuffer);
   idx[0] = 0;
   idx[1] = 1;
   idx[2] = 2;
   idx[3] = 0;
   idx[4] = 2;
   idx[5] = 3;
   activeDevice->unlockIndexBuffer(mArrowIndexBuffer);
}

void GamePlayerNameDisplay::draw() const
{
   initGlParameters();
   drawPlayTexts();

   if (mShowArrow)
      drawArrow();

   cleanupGlParameters();
}

void GamePlayerNameDisplay::initGlParameters() const
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
}

void GamePlayerNameDisplay::cleanupGlParameters() const
{
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}

float GamePlayerNameDisplay::computeFontAlpha() const
{
   float alpha = 1.0f;

   if (mActiveTime.elapsed() > FONT_DISPLAY_DURATION)
   {
      alpha = 1.0f - ((mActiveTime.elapsed() - FONT_DISPLAY_DURATION) / (float)FONT_DISPLAY_FADE_DURATION);
   }

   return alpha;
}

float GamePlayerNameDisplay::computeArrowAlpha() const
{
   float alpha = 1.0f;

   if (mActiveTime.elapsed() > ARROW_DISPLAY_DURATION)
   {
      alpha = 1.0f - ((mActiveTime.elapsed() - ARROW_DISPLAY_DURATION) / (float)ARROW_DISPLAY_FADE_DURATION);
   }

   return alpha;
}

void GamePlayerNameDisplay::drawPlayTexts() const
{
   // ortho over the whole frame, restored for drawArrow() below - mirrors the original's
   // glMatrixMode(GL_PROJECTION)/glPushMatrix()/glLoadMatrixf(ortho)/.../glPopMatrix() bracket.
   GLDevice* device = static_cast<GLDevice*>(activeDevice);
   device->pushProjection();
   device->setProjectionMatrix(Matrix::ortho(0, 1920, 1080, 0, -1.0f, 1.0f));

   for (int i = 0; i < mPositions.size(); i++)
   {
      const Vector& pos = mPositions[i];
      const QString& name = mNames[i];

      mFont->setColor(1.0f, 1.0f, 1.0f, computeFontAlpha());
      mFont->buildVertices(0.1f, qPrintable(name), pos.x, pos.y + FONT_Y_OFFSET, 0.0f);
      mFont->draw();
   }

   device->popProjection();
}

void GamePlayerNameDisplay::drawArrow() const
{
   if (!BombermanClient::getInstance()->getCurrentPlayerInfo()->isKilled())
   {
      const float offsetZ = 4.0f + sin(GlobalTime::Instance()->getTime() * 4.5f) * 0.5f;

      activeDevice->setShader(mArrowShader);

      glBindTexture(GL_TEXTURE_2D, mArrowTexture.getTexture());
      activeDevice->bindSampler(mArrowParamTexture, 0);
      activeDevice->setParameter(mArrowParamAlpha, computeArrowAlpha() * 0.5f);

      activeDevice->push(Matrix::position(mArrowPosition.x, mArrowPosition.y, offsetZ));

      glBindBuffer(GL_ARRAY_BUFFER, mArrowVertexBuffer);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ArrowVertex), (GLvoid*)0);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ArrowVertex), (GLvoid*)(3 * sizeof(float)));

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mArrowIndexBuffer);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);

      activeDevice->pop();
      activeDevice->setShader(0);
   }
}
