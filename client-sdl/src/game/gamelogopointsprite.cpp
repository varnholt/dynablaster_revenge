// header
#include "gamelogopointsprite.h"

// framework
#include "gldevice.h"
#include "render/texturepool.h"
#include "math/matrix.h"
#include "math/vector.h"

#include <vector>

// static
Texture GameLogoPointSprite::sTexture;
unsigned int GameLogoPointSprite::sShader = 0;
int GameLogoPointSprite::sTextureParam = -1;
unsigned int GameLogoPointSprite::sVertexBuffer = 0;
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
   TexturePool* pool = TexturePool::Instance();
   sTexture = pool->getTexture("pointsprite");

   sShader = activeDevice->loadShader(
      "pointsprite-vert.glsl",
      "pointsprite-frag.glsl"
   );
   sTextureParam = activeDevice->getParameterIndex("texturemap");

   glGenBuffers(1, &sVertexBuffer);
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
   if (mPositions.size() == 0)
      return;

   // bind point sprite texture and shader
   activeDevice->setShader(sShader);
   activeDevice->bindSampler(sTextureParam, 0);
   glBindTexture(GL_TEXTURE_2D, sTexture.getTexture());

   // positions are already absolute world-space (see GameLogoDrawable::drawSparks()) - no
   // per-object transform beyond whatever perspective projection is already active
   // (initPointSpriteGlParameters()).
   activeDevice->push(Matrix());

   // 6 verts (2 tris) per sprite quad, 5 floats/vert (position.xyz, uv.xy) - matches the legacy
   // glBegin(GL_QUADS) vertex order (BL, BR, TR, TL) so GL_TRIANGLES(0,1,2)+(0,2,3) reproduces the
   // same quad.
   std::vector<float> verts;
   verts.reserve(mPositions.size() * 6 * 5);

   for (int i = 0; i < mPositions.size(); i++)
   {
      const float pointSize = mGlowValues[i];
      const Vector& p = mPositions[i];

      const float bl[5] = { p.x - pointSize, p.y - pointSize, p.z, 0.0f, 0.0f };
      const float br[5] = { p.x + pointSize, p.y - pointSize, p.z, 1.0f, 0.0f };
      const float tr[5] = { p.x + pointSize, p.y + pointSize, p.z, 1.0f, 1.0f };
      const float tl[5] = { p.x - pointSize, p.y + pointSize, p.z, 0.0f, 1.0f };

      const float* quad[6] = { bl, br, tr, bl, tr, tl };
      for (int v = 0; v < 6; v++)
      {
         verts.insert(verts.end(), quad[v], quad[v] + 5);
      }
   }

   glBindBuffer(GL_ARRAY_BUFFER, sVertexBuffer);
   glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

   glDrawArrays(GL_TRIANGLES, 0, mPositions.size() * 6);

   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);

   activeDevice->pop();
   activeDevice->setShader(0);
}
