// header
#include "starfieldbackground.h"

// render
#include "render/texturepool.h"

// framework
#include "framework/globaltime.h"
#include "gldevice.h"


StarFieldBackground::StarFieldBackground()
 : mTexture(0)
{
   // init texture
   TexturePool* pool= TexturePool::Instance();
   mTexture = pool->getTexture("starfield");
}


void StarFieldBackground::begin()
{
}


void StarFieldBackground::end()
{

}


void StarFieldBackground::draw()
{
   // bind point sprite texture
   glBindTexture(GL_TEXTURE_2D, mTexture);
   glBegin(GL_QUADS);
   glColor4f(1,1,1,1);
   glEnable(GL_ALPHA_TEST);

   float scale = 1.0f;
   float offset = GlobalTime::Instance()->getTime() * 0.002f;

   glTexCoord2f(offset + 0.0f, 0.0f);
   glVertex2f(-scale, -scale);

   glTexCoord2f(offset + 2.0f, 0.0f);
   glVertex2f(scale, -scale);

   glTexCoord2f(offset + 2.0f, 1.0f);
   glVertex2f(scale, scale);

   glTexCoord2f(offset + 0.0f, 1.0f);
   glVertex2f(-scale, scale);

   glEnd();

   glDisable(GL_ALPHA_TEST);

   glBindTexture(GL_TEXTURE_2D, 0);
}
