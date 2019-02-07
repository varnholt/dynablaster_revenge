// header
#include "gameplayernamedisplay.h"

// Qt
#include "gldevice.h"

// game
#include "bombermanclient.h"
#include "fontpool.h"
#include "gamesettings.h"
#include "playeritem.h"

// framework
#include "globaltime.h"

// materials
#include "materials/material.h"

// math
#include "math/matrix.h"

// nodes
#include "nodes/mesh.h"

// image
#include "image/image.h"

// render
#include "render/texturepool.h"

// cmath
#include <math.h>

// defines
#define FONT_DISPLAY_DURATION 8000
#define FONT_DISPLAY_FADE_DURATION 3000
#define FONT_Y_OFFSET 30
#define ARROW_DISPLAY_DURATION 3000
#define ARROW_DISPLAY_FADE_DURATION 2000


//-----------------------------------------------------------------------------
/*!
   \param parent parent object
*/
GamePlayerNameDisplay::GamePlayerNameDisplay(QObject *parent)
 : QObject(parent),
   mFont(0),
   mShowArrow(false)
{
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if active
*/
bool GamePlayerNameDisplay::isActive() const
{
   return (mActiveTime.elapsed() < (FONT_DISPLAY_DURATION + FONT_DISPLAY_FADE_DURATION) );
}


//-----------------------------------------------------------------------------
/*!
   \param players
   \param projMat
*/
void GamePlayerNameDisplay::setPlayerData(
   QMap<int, PlayerItem*>& players
)
{
   Matrix projMat;
   glGetFloatv(GL_PROJECTION_MATRIX, projMat.data());

   int width = 1920; // activeDevice->getWidth();
   int height = 1080; //activeDevice->getHeight();

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

         v.x =  (v.x * t + 1.0f) * 0.5f * width;
         v.y = (-v.y * t + 1.0f) * 0.5f * height;
         v.z = 0.0f;

         mPositions << v;
         mNames << player->getNick();

         // init
         if (player->getID() == BombermanClient::getInstance()->getPlayerId())
         {
            mArrowPosition = player->getPosition();
         }
      }
   }
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::start()
{
   mActiveTime.restart();

   // show arrow only in large maps
   mShowArrow = false;
   GameInformation* info= BombermanClient::getInstance()->getCurrentGameInformation();

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


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::initialize()
{
   // init font
   mFont = FontPool::Instance()->get("outlined");

   // load arrow texture
   TexturePool* pool= TexturePool::Instance();
   mArrowTexture = pool->getTexture("data/game/arrow");
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::draw() const
{
   initGlParameters();
   drawPlayTexts();

   if (mShowArrow)
      drawArrow();

   cleanupGlParameters();
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::drawBoundingBox(const Vector& min, const Vector& max) const
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
void GamePlayerNameDisplay::initGlParameters() const
{
   // set gl settings
   glEnable(GL_BLEND);
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::cleanupGlParameters() const
{
   // enable blending
   glColor4f(1,1,1,1);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
}


//-----------------------------------------------------------------------------
/*!
   \return alpha
*/
float GamePlayerNameDisplay::computeFontAlpha() const
{
   float alpha = 1.0f;

   if (mActiveTime.elapsed() > FONT_DISPLAY_DURATION)
   {
      alpha =
           1.0f
         - ((mActiveTime.elapsed() - FONT_DISPLAY_DURATION) / (float)FONT_DISPLAY_FADE_DURATION);
   }

   return alpha;
}


//-----------------------------------------------------------------------------
/*!
   \return alpha
*/
float GamePlayerNameDisplay::computeArrowAlpha() const
{
   float alpha = 1.0f;

   if (mActiveTime.elapsed() > ARROW_DISPLAY_DURATION)
   {
      alpha =
           1.0f
         - ((mActiveTime.elapsed() - ARROW_DISPLAY_DURATION) / (float)ARROW_DISPLAY_FADE_DURATION);
   }

   return alpha;
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::drawPlayTexts() const
{
   // setup an ortho matrix but keep the projection and
   // model view matrices intact
   Matrix ortho= Matrix::ortho(0, 1920, 1080, 0, -1.0f, 1.0f);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadMatrixf(ortho);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   float alpha = 0.0f;

   for (int i=0; i< mPositions.size(); i++)
   {
      const Vector& pos= mPositions[i];
      const QString& name= mNames[i];

      alpha = computeFontAlpha();

      glColor4f(1.0f, 1.0f, 1.0f, alpha);

      mFont->buildVertices(
         0.1f,
         qPrintable(name),
         pos.x,
         pos.y + FONT_Y_OFFSET,
         0.0f
      );

      mFont->draw(mFont->getVertices());
   }

   // pop ortho from stack
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
}


//-----------------------------------------------------------------------------
/*!
*/
void GamePlayerNameDisplay::drawArrow() const
{
   if (!BombermanClient::getInstance()->getCurrentPlayerInfo()->isKilled())
   {
      // set opacity
      glColor4f(
         1.0,
         1.0,
         1.0,
         computeArrowAlpha() * 0.5f
      );

      glEnable(GL_TEXTURE_2D);

      glBindTexture(GL_TEXTURE_2D, mArrowTexture.getTexture());

      glBegin(GL_QUADS);

      float width = 0.75f;
      float height = 0.75f;

      float offsetZ = 4.0f + sin(GlobalTime::Instance()->getTime() * 4.5f) * 0.5f;

      glTexCoord2f(1.0f, 1.0f);
      glVertex3f( mArrowPosition.x + width, mArrowPosition.y, - height + offsetZ);

      glTexCoord2f(1.0f, 0.0f);
      glVertex3f( mArrowPosition.x + width, mArrowPosition.y, + height + offsetZ);

      glTexCoord2f(0.0f, 0.0f);
      glVertex3f( mArrowPosition.x - width, mArrowPosition.y, + height + offsetZ);

      glTexCoord2f(0.0f, 1.0f);
      glVertex3f( mArrowPosition.x - width, mArrowPosition.y, - height + offsetZ);

      glEnd();
   }
}


