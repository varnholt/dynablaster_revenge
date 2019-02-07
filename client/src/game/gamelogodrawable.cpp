// header
#include "gamelogodrawable.h"
#include "gamelogopointsprite.h"
#include "gamemenudefines.h"
#include "psdlayer.h"

// Qt
#include <QGLBuffer>
#include <QGLShaderProgram>

// engine
#include "gldevice.h"
#include "nodes/mesh.h"
#include "renderdevice.h"
#include "shaderpool.h"
#include "render/texturepool.h"
#include "nodes/scenegraph.h"
#include "tools/filestream.h"

// math
#include "math/vector.h"

// cmath
#include <math.h>

#define SPARK_COUNT 300
#define SPARK_START_INTENSITY 0.06f

#define LAYER_HIGHLIGHT "highlight"
#define LAYER_EARTH "earth"
#define LAYER_DYNABLASTER "dynablaster"
#define LAYER_REVENGE "revenge"

#define FADE_IN_LENGTH 80.0f
#define FADE_OUT_LENGTH 80.0f


GameLogoDrawable::GameLogoDrawable(RenderDevice *dev, bool visible)
   : SphereFragmentsDrawable(dev, visible),
     mMainMenuVisible(true),
     mDeltaTime(0.0),
     mTime(0.0),
     mLayerEarth(0),
     mLayerDynablaster(0),
     mLayerRevenge(0),
     mSceneGraphEarth(0),
     mVertexBufferCube(0),
     mIndexBufferCube(0),
     mFadeInEnd(0.0f),
     mFadeOutEnd(0.0f),
     mSparkTimesInitialized(false),
     mTextureHighlight(0),
     mHighlightDuration(0.0f)
{
   mFilename = "data/logo/logo.psd";

   mSceneGraphEarth = new SceneGraph();
}


GameLogoDrawable::~GameLogoDrawable()
{
   delete mSceneGraphEarth;
   delete mVertexBufferCube;
   delete mIndexBufferCube;

   qDeleteAll(mLayers);
}


/*

     +------------------------+ ----------------------- 'pointsprite layer'
     |                        |
     |                        |--- + ------------------ 'dynablaster font layer'
     |                        |    |
     |                        |    |----+ ------------- 'front cube layer'
     |                        |    |    |
     |                        |    |    |----+ -------- 'earth layer'
     |                        |    |    |    |
     |                        |    |    |    |----+ --- 'back cube layer'
     |                        |    |    |    |    |
     |                        |    |    |    |    |
 -3  +------------------------+    |    |    |    |
           |                       |    |    |    |
       -2  +-----------------------+    |    |    |
                 |                      |    |    |
             -1  +----------------------+    |    |
                       |                     |    |
                    0  +---------------------+    |
                             |                    |
                          1  +--------------------+

*/


void GameLogoDrawable::initializeGL()
{
   ShaderPool::registerShaderFromFile(
      "data/shaders/simplelight-vert.glsl",
      "data/shaders/simplelight-frag.glsl"
   );

   initializeLayers();
   initializeCubes();
   initializeSparks();

   SphereFragmentsDrawable::initializeGL();
}


void GameLogoDrawable::paintGL()
{
   updateFadeAlpha();

   if (mAlpha > 0.0f)
   {
      /*
      initOrthoGlParameters();
      mLayerEarth->render(0,0, mAlpha);
      drawHighlight();
      cleanupGlParameters();
      */

      SphereFragmentsDrawable::paintGL();

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

      /*
      initFrustumGlParameters(false);
      ShaderPool::getProgram("data/shaders/simplelight-vert.glsl")->bind();
      drawCubes();
      ShaderPool::getProgram("data/shaders/simplelight-vert.glsl")->release();
      */

      /*
      initFrustumGlParameters(true);
      ShaderPool::getProgram("data/shaders/simplelight-vert.glsl")->bind();
      drawCubes();
      ShaderPool::getProgram("data/shaders/simplelight-vert.glsl")->release();
      */

      initOrthoGlParameters();

      mLayerDynablaster->render(
         20.0f * cos(mTime * 0.03f),
         30.0f + 15.0f * sin(mTime * 0.04f),
         mAlpha
      );

      mLayerRevenge->render(
         30.0f * cos(mTime * 0.03f),
         30.0f + 25.0f * sin(mTime * 0.04f),
         mAlpha
      );

      initPointSpriteGlParameters();
      drawSparks();

      cleanupGlParameters();
   }
}


void GameLogoDrawable::pageChanged(const QString &page)
{
   bool wasVisible = mMainMenuVisible;
   mMainMenuVisible = (page == MAINMENU);

   // fade out
   if (wasVisible && !mMainMenuVisible)
   {
      mFadeOutEnd = mTime + FADE_OUT_LENGTH;
   }

   // fade in
   else if (!wasVisible && mMainMenuVisible)
   {
      mFadeInEnd = mTime + FADE_IN_LENGTH;
   }
}


void GameLogoDrawable::animate(float time)
{
   mDeltaTime = time - mTime;
   mTime = time;
}


void GameLogoDrawable::setVisible(bool visible)
{
   Drawable::setVisible(visible);

   if (!visible)
      mSparkTimesInitialized = false;
}


void GameLogoDrawable::drawSparks()
{
   bool visible = true;

   Array<Vector> positions;
   Array<float> glow;

   if (!mSparkTimesInitialized)
   {
      for (int i = 0; i < mSparks.size(); i++)
         mSparks[i].mStartTime = mTime + (qrand() % 1000)*0.1f;// * (i % 6);

      mSparkTimesInitialized = true;
   }

   for (int i = 0; i < mSparks.size(); i++)
   {
      if (mSparks[i].mStartTime <= mTime)
      {
         // reset spark if intensity threshold exceeded
         if (mSparks[i].mIntensity < 0.01)
         {
            initSpark(mSparks[i]);
            mSparks[i].mStartTime = mTime + (qrand() % 1000)*0.1f;// * (i % 6);
         }

         mSparks[i].mDirection.y -= mDeltaTime * 0.02f;
         mSparks[i].mScalar += mDeltaTime * 0.02f;
         mSparks[i].mIntensity -= mDeltaTime * 0.0015f;

         Vector pos =
               mSparks[i].mOrigin
             +
               (
                  mSparks[i].mDirection
                * mSparks[i].mLength
                * mSparks[i].mScalar
            );

         // TODO: fix this
         pos.z = -12.0f;

         // spark fading
         float intensity = 1.0f;
         if (mFadeInEnd > mTime)
         {
            intensity =  1.0f - (mFadeInEnd - mTime) / FADE_IN_LENGTH;
         }
         else if (mFadeOutEnd > mTime)
         {
            intensity = (mFadeOutEnd - mTime) / FADE_OUT_LENGTH;
         }
         else if (!mMainMenuVisible)
         {
            visible = false;
         }

         if (visible)
         {
            positions.add(pos);
            glow.add(mSparks[i].mIntensity * intensity);
         }
      }
   }

   if (visible)
   {
      GameLogoPointSprite::setPointSprites(
         positions,
         glow
      );

      GameLogoPointSprite::draw();
   }
}


void GameLogoDrawable::drawCubes()
{
   bool visible = true;

   glColor4ub(5, 58, 91, 255);

   for (int i = 0; i < mCubes.size(); i++)
   {
      glBindTexture(GL_TEXTURE_2D, mCubeTexture[i]);

      glPushMatrix();

      float factor = 1.0f;
      float max = 6.0f;
      float oneToZero = 0.0f;
      float smoothMotion = 0.0f;

      if (mFadeInEnd > mTime)
      {
         oneToZero = (mFadeInEnd - mTime) / FADE_IN_LENGTH;
         smoothMotion = 1.0f - (0.5f * (1.0f + cos(M_PI * (oneToZero))));
         factor = 1.0f + max * smoothMotion;
      }
      else if (mFadeOutEnd > mTime)
      {
         oneToZero = (mFadeOutEnd - mTime) / FADE_OUT_LENGTH;
         smoothMotion = 0.5f * (1.0f + cos(M_PI * (oneToZero)));
         factor = 1.0f + max * smoothMotion;
      }
      else if (!mMainMenuVisible)
      {
         visible = false;
      }

      if (visible)
      {
         float x = sin(mCubes[i].mRotOffset + mTime * 0.01) * 5.0 * factor;
         float y = cos(mCubes[i].mRotOffset + mTime * 0.01) * 5.0 * factor;

         // rotate earth axis a little around z
         glRotatef(
            -15.0f,
            0.0f,
            0.0f,
            1.0f
         );

         // finally move to destination (which rotates around earth)
         glTranslatef(x, -1.8f, y - 12.0f);

         // rotate cube around itself
         glRotatef(
            mTime,
            mCubes[i].mRotX,
            mCubes[i].mRotY,
            mCubes[i].mRotZ
         );

         // scale cube
         glScalef(
            mCubes[i].mScale,
            mCubes[i].mScale,
            mCubes[i].mScale
         );

         drawVbos(
            mVertexBufferCube,
            mIndexBufferCube,
            mIndicesCube.size()
         );

         glPopMatrix();

      }
   }

   glColor4ub(255, 255, 255, 255);
}


void GameLogoDrawable::updateFadeAlpha()
{
   float alpha = mMainMenuVisible ? 1.0f : 0.0f;

   if (mFadeInEnd > mTime)
   {
      alpha = 1.0f - (mFadeInEnd - mTime)/FADE_IN_LENGTH;
   }
   else if (mFadeOutEnd > mTime)
   {
      alpha = (mFadeOutEnd - mTime)/FADE_OUT_LENGTH;
   }

   mAlpha = alpha;
}


void GameLogoDrawable::drawHighlight()
{
   if (mHighlightDuration < 0.75)
   {
      // float normSin = 1.0 + sin(mTime * 0.005) * 0.5f;
      mHighlightDuration = 0.75f + (qrand() % 100)*0.0025; // * 0.5f;
   }
   else
   {
      mHighlightDuration -= 0.01f;
   }

   if (mTextureHighlight)
   {
      mTextureHighlight->setOpacity(
         (mAlpha > 0.8f) ? qMax<float>(mHighlightDuration, 0.0) : 0.0
      );

      mTextureHighlight->render();
   }
}



void GameLogoDrawable::initFrustumGlParameters(bool front)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float znear = 0.0f;
   float zfar = 0.0f;

   if (front)
   {
      znear = 1.0f;
      zfar = 12.0f;
   }
   else
   {
      znear = 12.0f;
      zfar = 24.0f;
   }

   float scale= 0.5f;
   float aspect= 16.0f / 9.0f;

   float ymin = znear * scale;
   float ymax = -ymin;

   float xmax = ymax * aspect;
   float xmin = ymin * aspect;

   glFrustum( xmin, xmax, ymin, ymax, znear, zfar );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // disable blending
   glDisable(GL_BLEND);

   // enable z buffer
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);

   mDevice->setShader(0);
}

void GameLogoDrawable::initPointSpriteGlParameters()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float znear = 0.1f;
   float zfar = 5000.0f;

   float scale= 0.5f;
   float aspect= 16.0f / 9.0f;

   float ymin = znear * scale;
   float ymax = -ymin;

   float xmax = ymax * aspect;
   float xmin = ymin * aspect;

   glFrustum( xmin, xmax, ymin, ymax, znear, zfar );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // init blending
   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE);

   // for some reason that fucked up the cubes after resize
   // glDepthMask(false);

   // enable point sprite texture
   glEnable(GL_TEXTURE_2D);

   mDevice->setShader(0);
}



void GameLogoDrawable::initOrthoGlParameters()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(
      0.0f,
      mPsd.getWidth(),
      mPsd.getHeight(),
      0.0f,
      300.0f,
      -300.0f
   );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);

   mDevice->setShader(0);
}


void GameLogoDrawable::cleanupGlParameters()
{
   // enable blending
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void GameLogoDrawable::initializeLayers()
{
   mPsd.load(qPrintable(mFilename));

   // assign layers to menu page items
   for (int l = 0; l < mPsd.getLayerCount(); l++)
   {
      PSD::Layer* psdlayer= mPsd.getLayer(l);

      PSDLayer* layer= new PSDLayer( psdlayer );

      if (psdlayer->getName() == QString(LAYER_EARTH))
      {
         mLayerEarth = layer;
      }
      else if (psdlayer->getName() == QString(LAYER_DYNABLASTER))
      {
         mLayerDynablaster = layer;
      }
      else if (psdlayer->getName() == QString(LAYER_REVENGE))
      {
         mLayerRevenge = layer;
      }
      else if (psdlayer->getName() == QString(LAYER_HIGHLIGHT))
      {
         mTextureHighlight = layer;
      }

      mLayers << layer;
   }
}


void GameLogoDrawable::initializeCubes()
{
   FileStream::addPath("data/logo");
   TexturePool* pool= TexturePool::Instance();

//   {
//       Texture test= pool->getTexture("cube_1");
//   }

   for (int i = 0; i < CUBE_COUNT; i++)
   {
      QString textureName = QString("cube_%1").arg(i+1);

      mCubeTexture[i] = pool->getTexture(qPrintable(textureName));
   }

   // init
   int bytes = 0;
   mSceneGraphEarth->load("cube.hjb");
   Mesh* cube = (Mesh*)mSceneGraphEarth->getChild(0);
   Geometry* geo = cube->getPart(0);

   // create vertex and index buffer
   mVertexBufferCube = new QGLBuffer(QGLBuffer::VertexBuffer);
   mIndexBufferCube = new QGLBuffer(QGLBuffer::IndexBuffer);
   mIndexBufferCube->create();
   mVertexBufferCube->create();

   // combine geometry data
   mVerticesCube.resize(geo->getVertexCount());

   const List<Vector>& vertices = geo->getVertexList();
   const List<Vector>& normals = geo->getNormalList();
   UV* uvs = geo->getUV(1);
//   UV *uv = geo->getUVList()[0]->data();

   Vector vector;
   Vector normal;
   // UV* uv;
   for (int i = 0; i < vertices.size(); i++)
   {
      vector = vertices.get(i);
      Vector tmp= vector;
      tmp.normalize();
      normal = normals.get(i) + (tmp * 0.25);
      normal.normalize();

      mVerticesCube[i].mPosition = vector;
      mVerticesCube[i].mNormal = normal;
      mVerticesCube[i].mU = uvs[i].u;
      mVerticesCube[i].mV = uvs[i].v;
   }

   // copy indices
   Array<unsigned short> indices = geo->getIndicesList();
   mIndicesCube.resize(indices.size());

   for (int i = 0; i < indices.size(); i++)
   {
      mIndicesCube[i] = indices.get(i);
   }

   // init vbos
//   qDebug(
//      "GameLogoDrawable::initializeCube: vertex buffer id: %d",
//      mVertexBufferCube->bufferId()
//   );

   if (!mVertexBufferCube->bind())
      qWarning("GameLogoDrawable::initializeCube: vertex buffer bind failed");

   bytes = sizeof(Vertex3D) * mVerticesCube.size();

   mVertexBufferCube->setUsagePattern(QGLBuffer::StaticDraw);
   mVertexBufferCube->allocate(mVerticesCube.data(), bytes);

//   qDebug("GameLogoDrawable::initializeCube: allocated %d bytes", bytes);

//   qDebug(
//      "GameLogoDrawable::initializeCube: index buffer id: %d",
//      mIndexBufferCube->bufferId()
//   );

   if (!mIndexBufferCube->bind())
      qWarning("GameLogoDrawable::initializeCube: index buffer bind failed");

   bytes = sizeof(GLuint)*mIndicesCube.size();
   mIndexBufferCube->setUsagePattern(QGLBuffer::StaticDraw);
   mIndexBufferCube->allocate(mIndicesCube.data(), bytes);

//   qDebug("GameLogoDrawable::initializeCube: allocated %d bytes", bytes);

   // create a few cubes
   qsrand(mTime * 1000.0f);

   float offset = 0.0f;
   for (int i = 0; i < CUBE_COUNT; i++)
   {
      Cube cube;
      cube.mRotOffset = offset;
      cube.mRotX = (qrand() % 100) * 0.05f;;
      cube.mRotY = (qrand() % 100) * 0.05f;
      cube.mRotZ = (qrand() % 100) * 0.05f;
      cube.mScale = 0.5f + (qrand() % 100) * 0.005;
      mCubes.append(cube);

      offset += (1.0f / CUBE_COUNT) * (2.0f * (float)M_PI);
   }

   FileStream::removePath("data/logo");
}



void GameLogoDrawable::drawVbos(
   QGLBuffer* vertexBuffer,
   QGLBuffer* indexBuffer,
   int indicesCount
)
{
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   vertexBuffer->bind();

   // qDebug("err bind vtx: %d", glGetError());

   glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)0);
   glNormalPointer(GL_FLOAT, sizeof(Vertex3D), (GLvoid*)sizeof(Vector) );
   glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D), (GLvoid*)(sizeof(Vector)*2) );
   indexBuffer->bind();

   // qDebug("err bind idx: %d", glGetError());

   glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);

   // qDebug("err drawelem: %d", glGetError());

   glDisableClientState(GL_VERTEX_ARRAY);

   indexBuffer->release();
   vertexBuffer->release();
}


void GameLogoDrawable::initSpark(Spark& spark)
{
   spark.mOrigin = mSparkOrigin; // maybe vary a little
   spark.mPosition = mSparkOrigin;
   spark.mIntensity = SPARK_START_INTENSITY;
   spark.mScalar = 0.0f;

   float originRandX = 0.0f;
   float originRandY= 0.0f;
   float originRandZ= 0.0f;

   originRandX = -0.0f + (qrand() % 100) * 0.002f;
   originRandY = -0.0f + (qrand() % 100) * 0.002f;
   originRandZ = -0.2f + (qrand() % 100) * 0.002f;

   spark.mOrigin.x += originRandX;
   spark.mOrigin.y += originRandY;
   spark.mOrigin.z += originRandZ;

   float dirRandX = 0.0f;
   float dirRandY= 0.0f;

   dirRandX = -0.75f + (qrand() % 100) * 0.005f;
   dirRandY = -0.75f + (qrand() % 100) * 0.005f;

   // qDebug("%f, %f", dirRandX, dirRandY);

   spark.mDirection =
      Vector(
         -1.0f - dirRandX,
         -1.0f - dirRandY,
         -12.0f
      );

   spark.mLength = 1.0f;
}


void GameLogoDrawable::initializeSparks()
{
   GameLogoPointSprite::initialize();

   mSparkOrigin = Vector(-3.7f, -3.3f, -12.0f);

   qsrand(mTime * 1000.0f);

   for (int i = 0; i < SPARK_COUNT; i++)
   {
      Spark spark;
      initSpark(spark);
      mSparks.append(spark);
   }
}


