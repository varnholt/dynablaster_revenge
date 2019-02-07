/***************************************************************************
                                 pointsprite
 ---------------------------------------------------------------------------
    project              : fireflies
    begin                : june 2011
    copyright            : (C) 2011 by mueslee
    author               : mueslee
    email                : nope
***************************************************************************/

// header
#include "gamelogopointsprite.h"

// framework
#include "gldevice.h"
#include <math.h>

// static
int GameLogoPointSprite::sTexture = 0;
Array<Vector> GameLogoPointSprite::mPositions;
Array<float> GameLogoPointSprite::mGlowValues;


//-----------------------------------------------------------------------------
/*!
*/
GameLogoPointSprite::GameLogoPointSprite()
{
}


//-----------------------------------------------------------------------------
/*!
*/
void GameLogoPointSprite::initialize()
{
   QImage image = QImage("data/logo/pointsprite.png");

   // load layers to texture
   sTexture =
      ((QGLContext*)QGLContext::currentContext())->bindTexture(
         image,
         GL_TEXTURE_2D,
         GL_RGBA,
         QGLContext::LinearFilteringBindOption
      );
}


//-----------------------------------------------------------------------------
/*!
*/
void GameLogoPointSprite::setPointSprites(
   const Array<Vector>& v,
   const Array<float>& glowValues
)
{
   mPositions = v;
   mGlowValues = glowValues;
}


//-----------------------------------------------------------------------------
/*!
*/
void GameLogoPointSprite::draw()
{
   // bind point sprite texture
   glBindTexture(GL_TEXTURE_2D, sTexture);

   float pointSize = 0.0f;

   glBegin(GL_QUADS);

   for (int i = 0; i < mPositions.size(); i++)
   {
      // new code
      pointSize = mGlowValues[i];

      glColor4f(1,1,1,1);

      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(
         mPositions[i].x - pointSize,
         mPositions[i].y - pointSize,
         mPositions[i].z
      );

      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(
         mPositions[i].x + pointSize,
         mPositions[i].y - pointSize,
         mPositions[i].z
      );

      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(
         mPositions[i].x + pointSize,
         mPositions[i].y + pointSize,
         mPositions[i].z
      );

      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(
         mPositions[i].x - pointSize,
         mPositions[i].y + pointSize,
         mPositions[i].z
      );
   }

   glEnd();
}


