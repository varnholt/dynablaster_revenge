// header
#include "lensflare.h"

// engine
#include "gldevice.h"
#include "image/image.h"
#include "render/texturepool.h"
#include "tools/filestream.h"

// framework
#include "globaltime.h"

// Qt
#include <QGLContext>
#include <QImage>

// cmath
#include <math.h>

// static
unsigned int LensFlare::sShader = 0;

// defines
#define MATRIX_WIDTH 4
#define MATRIX_HEIGHT 4


//-----------------------------------------------------------------------------
/*!
*/
LensFlare::LensFlare()
 : mBufferVertices(0),
   mBufferIndices(0),
   mInitialized(false),
   mIndexCount(0),
   mMaxLength(0.0f),
   mLowerLimit(0.0f),
   mAngle(0.0f),
   mInverted(false),
   mInvertOffset(0.0f),
   m2d(false),
   mTimeParam(0),
   mSunParam(0),
   mNormalizedLength(0)
{
}


//-----------------------------------------------------------------------------
/*!
*/
LensFlare::~LensFlare()
{
   activeDevice->deleteBuffer(mBufferVertices);
   activeDevice->deleteBuffer(mBufferIndices);
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::initializeStatic()
{
   // init shader
   sShader = activeDevice->loadShader(
      "data/effects/lensflare/shaders/lensflare-vert.glsl",
      "data/effects/lensflare/shaders/lensflare-frag.glsl"
   );
}


//-----------------------------------------------------------------------------
/*!
   \param initialized set ray to initialized
*/
void LensFlare::setInitialized(bool initialized)
{
   mInitialized = initialized;
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::initializeInstance()
{
   FileStream::addPath("data/effects/lensflare/flares");
   TexturePool* pool= TexturePool::Instance();
   mTexture = pool->getTexture(qPrintable(getTextureFileName()));

   // read ghosts from harddisk
   initializeGhosts();

   // init shader parameters
   mTimeParam = activeDevice->getParameterIndex("time");
   mSunParam = activeDevice->getParameterIndex("sun");
   mNormalizedLength = activeDevice->getParameterIndex("normalizedlength");

   createBuffers();
   setInitialized(true);
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::bindTexture()
{
   glBindTexture(GL_TEXTURE_2D, mTexture.getTexture());
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if animation is already initialized
*/
bool LensFlare::isInitialized() const
{
   return mInitialized;
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::pre()
{
   glDisable(GL_CULL_FACE);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(false);

   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE);

   glAlphaFunc(GL_GREATER,0.0f);
   glEnable(GL_ALPHA_TEST);
   glEnable(GL_TEXTURE_2D);

   activeDevice->setShader(sShader);
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::post()
{
   activeDevice->setShader(0);

   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(true);
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_ALPHA_TEST);
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void LensFlare::draw()
{
   bindTexture();

   float length = sqrtf(mSun2d.x * mSun2d.x + mSun2d.y * mSun2d.y);

   /*
   qDebug(
      "length: %f, "
      "length - lower limit: %f, "
      "length - lower limit, normalized: %f",
      length,
      length - getLowerLimit(),
      (length - getLowerLimit()) / getMaxLength()
   );
   */

   if (isInverted())
      length = getInvertOffset() - length;

   length -= getLowerLimit();
   length /= getMaxLength();

   activeDevice->setParameter(mTimeParam, GlobalTime::Instance()->getTime());
   activeDevice->setParameter(mSunParam, mSun2d);
   activeDevice->setParameter(mNormalizedLength, length);

   drawBuffers();
}


//-----------------------------------------------------------------------------
/*!
   \param sun sun position
*/
void LensFlare::setSun2d(const Vector2 &sun)
{
   mSun2d = sun;
}


//-----------------------------------------------------------------------------
/*!
  \return sun position
*/
Vector2 LensFlare::getSun2d() const
{
   return mSun2d;
}


//-----------------------------------------------------------------------------
/*!
   \param tileCol tile column
   \param tileRow tile row
   \param tileWidth tile width
   \param tileHeight tile height
   \return texture 2d offset vector
*/
Vector2 LensFlare::computeTextureOffset(
   int tileCol,
   int tileRow,
   float& tileWidth,
   float& tileHeight
) const
{
   Vector2 uvOffset;

   int tileMatrixWidth  = MATRIX_WIDTH;
   int tileMatrixHeight = MATRIX_HEIGHT;
   tileWidth =  1.0f / tileMatrixWidth;
   tileHeight = 1.0f / tileMatrixHeight;

   uvOffset.x = tileCol * tileWidth;
   uvOffset.y = tileRow * tileHeight;

   return uvOffset;
}


//-----------------------------------------------------------------------------
/*!
   \param p point to rotate
   \param angle rotation angle
*/
void LensFlare::rotate(Vector2& p, float angle)
{
   float x = p.x * cos(angle) - p.y * sin(angle);
   float y = p.y * cos(angle) + p.x * sin(angle);
   p.x = x;
   p.y = y;
}


//-----------------------------------------------------------------------------
/*!
   \param scale star scale
*/
void LensFlare::createBuffers()
{
   /*

   height

     ^
     | vec2 sun position
     |
     |     (*)
     |     ---___                   |
     |            ^^^---___         |
     |                     ^^^---___| vec2 center
     |------------------------------+----------------------------------
     |                              |^^^---___
     |                              |         ^^^---___
     |                              |                  ^^^---
     +-----------------------------------------------------------------> width

   */



   int ghostCount = mGhosts.size();
   int vertexCount = ghostCount * 4;
   mIndexCount = ghostCount * 6;

   // create vertex and index buffer
   mBufferVertices = activeDevice->createVertexBuffer(
         vertexCount * sizeof(LensFlareVertex)
      );

   mBufferIndices = activeDevice->createIndexBuffer(
         mIndexCount * sizeof(unsigned short)
      );

   // fill vertex buffer
   LensFlareVertex* vtx= (LensFlareVertex*)activeDevice->lockVertexBuffer( mBufferVertices );

   float tileWidth = 0.0f;
   float tileHeight = 0.0f;
   int index = 0;
   for (int i=0; i < vertexCount; i+=4)
   {
      const LensFlareGhost& ghost = mGhosts.at(index);

      // read from config
      float size    = ghost.getSize();
      float scalar  = ghost.getRayPosition();
      float offsetY = ghost.getOffsetY();
      int tileRow   = ghost.getTileRow();
      int tileCol   = ghost.getTileColumn();
      Vector color  = ghost.getColor();
      int width     = ghost.getWidth();
      int height    = ghost.getHeight();
      float angle   = ghost.getAngle();
      float speed   = ghost.getSpeed();

      // if no angle is given, use the global angle
      if (angle == 0.0f)
          angle = getAngle();

      // compute uv offset
      Vector2 uvOffset =
         computeTextureOffset(
            tileCol,
            tileRow,
            tileWidth,
            tileHeight
         );

      tileWidth *= width;
      tileHeight *= height;

      Vector2 p1( 0.5f * size,  0.5f * size);
      Vector2 p2(-0.5f * size,  0.5f * size);
      Vector2 p3(-0.5f * size, -0.5f * size);
      Vector2 p4( 0.5f * size, -0.5f * size);

      rotate(p1, angle);
      rotate(p2, angle);
      rotate(p3, angle);
      rotate(p4, angle);

      // set positions
      vtx[i  ].mPosition = p1;
      vtx[i+1].mPosition = p2;
      vtx[i+2].mPosition = p3;
      vtx[i+3].mPosition = p4;

      // set scalar
      vtx[i  ].mRayPosition = scalar;
      vtx[i+1].mRayPosition = scalar;
      vtx[i+2].mRayPosition = scalar;
      vtx[i+3].mRayPosition = scalar;

      vtx[i  ].mRotationSpeed = speed;
      vtx[i+1].mRotationSpeed = speed;
      vtx[i+2].mRotationSpeed = speed;
      vtx[i+3].mRotationSpeed = speed;

      // set offset
      vtx[i  ].mOffset.x = 0.0f; // offsetX; unused
      vtx[i  ].mOffset.y = offsetY;
      vtx[i+1].mOffset.x = 0.0f; // offsetX; unused
      vtx[i+1].mOffset.y = offsetY;
      vtx[i+2].mOffset.x = 0.0f; // offsetX; unused
      vtx[i+2].mOffset.y = offsetY;
      vtx[i+3].mOffset.x = 0.0f; // offsetX; unused
      vtx[i+3].mOffset.y = offsetY;

      // set UVs
      vtx[i  ].mUv.x = uvOffset.x + tileWidth;  // top, right
      vtx[i  ].mUv.y = uvOffset.y + tileHeight;
      vtx[i+1].mUv.x = uvOffset.x + 0.0f;       // top, left
      vtx[i+1].mUv.y = uvOffset.y + tileHeight;
      vtx[i+2].mUv.x = uvOffset.x + 0.0f;       // bottom, left
      vtx[i+2].mUv.y = uvOffset.y + 0.0f;
      vtx[i+3].mUv.x = uvOffset.x + tileWidth;  // bottom, right
      vtx[i+3].mUv.y = uvOffset.y + 0.0f;

      // set color
      vtx[i  ].mColor = color;
      vtx[i+1].mColor = color;
      vtx[i+2].mColor = color;
      vtx[i+3].mColor = color;

      index++;
   }

   activeDevice->unlockVertexBuffer(mBufferVertices);

   // fill vertex buffer
   unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer(mBufferIndices);

   index = 0;
   for (int i=0; i < mIndexCount; i+= 6)
   {
      // make indices for 2 triangles
      idx[i  ] = 0 + index;
      idx[i+1] = 1 + index;
      idx[i+2] = 3 + index;

      idx[i+3] = 3 + index;
      idx[i+4] = 1 + index;
      idx[i+5] = 2 + index;

      // next quad
      index += 4;
   }

   activeDevice->unlockIndexBuffer(mBufferIndices);
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::drawBuffers()
{
   drawVbos(&mBufferVertices, &mBufferIndices, mIndexCount);
}


//-----------------------------------------------------------------------------
/*!
*/
void LensFlare::initializeGhosts()
{
   mGhosts = LensFlareGhost::readGhostStrip(
      QString("data/effects/lensflare/flares/%1").arg(getGhostsFileName())
   );
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if sun is 2d
*/
bool LensFlare::is2d() const
{
   return m2d;
}


//-----------------------------------------------------------------------------
/*!
   \param value sun is 2d
*/
void LensFlare::set2d(bool value)
{
   m2d = value;
}



//-----------------------------------------------------------------------------
/*!
   \return invert offset
*/
float LensFlare::getInvertOffset() const
{
   return mInvertOffset;
}


//-----------------------------------------------------------------------------
/*!
   \param value invert offset
*/
void LensFlare::setInvertOffset(float value)
{
   mInvertOffset = value;
}


//-----------------------------------------------------------------------------
/*!
   return inverted flag
*/
bool LensFlare::isInverted() const
{
   return mInverted;
}


//-----------------------------------------------------------------------------
/*!
   \param value inverted flag
*/
void LensFlare::setInverted(bool value)
{
   mInverted = value;
}



//-----------------------------------------------------------------------------
/*!
   \return angle flare angle
*/
float LensFlare::getAngle() const
{
   return mAngle;
}


//-----------------------------------------------------------------------------
/*!
   \param value flare angle
*/
void LensFlare::setAngle(float value)
{
   mAngle = value;
}


//-----------------------------------------------------------------------------
/*!
   \return lower limit
*/
float LensFlare::getLowerLimit() const
{
   return mLowerLimit;
}


//-----------------------------------------------------------------------------
/*!
   \param limit lower limit
*/
void LensFlare::setLowerLimit(float limit)
{
   mLowerLimit = limit;
}


//-----------------------------------------------------------------------------
/*!
   \return max length
*/
float LensFlare::getMaxLength() const
{
   return mMaxLength;
}


//-----------------------------------------------------------------------------
/*!
   \param length max length
*/
void LensFlare::setMaxLength(float length)
{
   mMaxLength = length;
}


//-----------------------------------------------------------------------------
/*!
   \return ghost filename
*/
const QString& LensFlare::getGhostsFileName() const
{
   return mGhostsFileName;
}


//-----------------------------------------------------------------------------
/*!
   \param fileName ghost filename
*/
void LensFlare::setGhostsFileName(const QString &fileName)
{
   mGhostsFileName = fileName;
}


//-----------------------------------------------------------------------------
/*!
   \return texture filename
*/
const QString &LensFlare::getTextureFileName() const
{
   return mTextureFileName;
}


//-----------------------------------------------------------------------------
/*!
   \param fileName texture filename
*/
void LensFlare::setTextureFileName(const QString &fileName)
{
   mTextureFileName = fileName;
}



//-----------------------------------------------------------------------------
/*!
   \return sun 3d vector
*/
const Vector& LensFlare::getSun3d() const
{
   return mSun3d;
}


//-----------------------------------------------------------------------------
/*!
   param sun 3d vector
*/
void LensFlare::setSun3d(const Vector& sun)
{
   mSun3d = sun;
}



//-----------------------------------------------------------------------------
/*!
  \param vertexBuffer pointer to vertex buffer
  \param indexBuffer pointer to index buffer
  \param indices indices
*/
void LensFlare::drawVbos(
   unsigned int* vertexBuffer,
   unsigned int* indexBuffer,
   int mIndexCount
)
{
   // vertex structure:
   //
   // - Vector2 mPosition;
   // - float mScalar;
   // - Vector2 mOffset;
   // - Vector2 mUv;
   // - Vector mColor;

   char* ptr = 0;

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   // vertex
   glBindBuffer( GL_ARRAY_BUFFER_ARB, *vertexBuffer );

      // position and scalar go in here
      glVertexPointer(4, GL_FLOAT, sizeof(LensFlareVertex), (GLvoid*)ptr);
      ptr += sizeof(Vector4);

      // offset and uv go into color
      glColorPointer(4,  GL_FLOAT, sizeof(LensFlareVertex), (GLvoid*)ptr);
      ptr += sizeof(Vector4);

      // color goes into normal
      glNormalPointer(GL_FLOAT, sizeof(LensFlareVertex), (GLvoid*)ptr );
      ptr += sizeof(Vector);


   // indices
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, *indexBuffer );

      // render
      glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0 );

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
}



