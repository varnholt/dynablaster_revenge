// header
#include "startalers.h"

// engine
#include "framework/globaltime.h"
#include "gldevice.h"
#include "image/image.h"
#include "render/texturepool.h"
#include "tools/filestream.h"

// Qt
#include <QGLContext>
#include <QImage>

// cmath
#include <math.h>

// shader parameters
Texture StarTalers::sTexture;
unsigned int StarTalers::mShader = 0;
int StarTalers::mTimeParam = -1;
int StarTalers::sFieldParam = -1;
int StarTalers::sHeightmapParam = -1;
int StarTalers::sCameraParam = -1;
int StarTalers::sColorParam = -1;

// other static
Matrix StarTalers::sCamera;

// defines
#define DURATION 10.0f;


//-----------------------------------------------------------------------------
/*!
*/
StarTalers::StarTalers()
 : mTime(0.0f),
   mHeightmap(0),
   mBufferVertices(0),
   mBufferIndices(0),
   mInitialized(false),
   mIndexCount(0)
{
   mHeightmap = new float[49];
}


//-----------------------------------------------------------------------------
/*!
*/
StarTalers::~StarTalers()
{
   activeDevice->deleteBuffer(mBufferVertices);
   activeDevice->deleteBuffer(mBufferIndices);

   delete [] mHeightmap;
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::initializeStatic()
{
   FileStream::addPath("data/effects/startalers/shaders");
   FileStream::addPath("data/effects/startalers/images");

   TexturePool* pool= TexturePool::Instance();

   sTexture = pool->getTexture("startalers_particles");

   // init shader
   mShader = activeDevice->loadShader(
      "startalers-vert.glsl",
      "startalers-frag.glsl"
   );

   mTimeParam = activeDevice->getParameterIndex("time");
   sFieldParam = activeDevice->getParameterIndex("field");
   sHeightmapParam = activeDevice->getParameterIndex("heightmap");
   sCameraParam = activeDevice->getParameterIndex("camera");
   sColorParam = activeDevice->getParameterIndex("color");
}


//-----------------------------------------------------------------------------
/*!
   \param initialized set ray to initialized
*/
void StarTalers::setInitialized(bool initialized)
{
   mInitialized = initialized;
}


//-----------------------------------------------------------------------------
/*!
   \param cam ref to camera
*/
void StarTalers::setCamera(const Matrix &cam)
{
   sCamera = cam;
}


//-----------------------------------------------------------------------------
/*!
   \return camera
*/
const Matrix& StarTalers::getCamera()
{
   return sCamera;
}



//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::prepareCamera()
{
   sCamera = sCamera.get3x3().normalized();
}


//-----------------------------------------------------------------------------
/*!
*/
float StarTalers::getTime() const
{
   return mTime;
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::initializeInstance()
{
   // QTime elapsed;
   // elapsed.start();

   createBuffers(0.5f);
   generateHeightmap();
   qsrand(QTime::currentTime().msec());

   // qDebug("%d msecs elapsed", elapsed.elapsed());
   setInitialized(true);
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void StarTalers::updateTime(float dt)
{
   mTime += dt;
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::bindTexture()
{
   glBindTexture(GL_TEXTURE_2D, sTexture.getTexture());
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if animation is already initialized
*/
bool StarTalers::isInitialized() const
{
   return mInitialized;
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::pre()
{
   glAlphaFunc(GL_GREATER,0.0f);
   glEnable(GL_ALPHA_TEST);
   glEnable(GL_TEXTURE_2D);

   bindTexture();

   activeDevice->setShader(mShader);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::post()
{
   activeDevice->setShader(0);

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_ALPHA_TEST);
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void StarTalers::draw(float dt)
{
   updateTime(dt);

   activeDevice->setParameter(mTimeParam, mTime);
   activeDevice->setParameter(sFieldParam, getFieldPosition());
   // activeDevice->setParameter(sHeightmapParam, mHeightmap, 49);
   activeDevice->setParameter(sColorParam, getColor());
   activeDevice->setParameter(sCameraParam, getCamera().z());

   drawBuffers();
}


//-----------------------------------------------------------------------------
/*!
   \param fieldPosition animation field position
*/
void StarTalers::setFieldPosition(const Vector &fieldPosition)
{
   mFieldPosition = fieldPosition;
}


//-----------------------------------------------------------------------------
/*!
  \return animation field position
*/
Vector StarTalers::getFieldPosition() const
{
   return mFieldPosition;
}



//-----------------------------------------------------------------------------
/*!
  \param color startaler color
*/
void StarTalers::setColor(const Vector &color)
{
   mColor = color;
}


//-----------------------------------------------------------------------------
/*!
  \return startaler color
*/
const Vector &StarTalers::getColor() const
{
   return mColor;
}


//-----------------------------------------------------------------------------
/*!
  \return \c true if time is elapsed
*/
bool StarTalers::isElapsed() const
{
   return mTime > DURATION;
}


//-----------------------------------------------------------------------------
/*!
   \param width direction width
   \param depth direction depth
   \param height direction height
   \return direction vector
*/
Vector StarTalers::computeDirection(float width, float depth, float height)
{
   // make the top-face (z face circular)
   float alpha = 2.0f * (float)M_PI * ((qrand() % RAND_MAX)/(float)RAND_MAX);
   float x = cos(alpha); // -1..1
   float y = sin(alpha); // -1..1
   float randX = ((qrand() % RAND_MAX)/(float)RAND_MAX) * x;
   float randY = ((qrand() % RAND_MAX)/(float)RAND_MAX) * y;

   // 0.75f is a request by hfr
   // "sterne gehen zu sehr ab."
   // original value is 1.0f
   Vector normalizedDirection = Vector(
      randX,
      randY,
      0.75f
   );

   normalizedDirection.normalize();

   Vector direction;
   direction.x = width * normalizedDirection.x;
   direction.y = depth * normalizedDirection.y;
   direction.z = normalizedDirection.z * height;

   return direction;
}


//-----------------------------------------------------------------------------
/*!
   \param min minimum speed
   \param range speed range
   \return computed speed
*/
float StarTalers::computeSpeed(float min, float range)
{
   float speed = (qrand() % RAND_MAX)/(float)RAND_MAX;
   speed *= range;

   speed += min;

   float rand = (qrand() % RAND_MAX)/ (double)RAND_MAX;

   if (rand > 0.8)
      speed *= 1.5f;

   return speed;
}


//-----------------------------------------------------------------------------
/*!
*/
Vector2 StarTalers::computeTextureOffset() const
{
   Vector2 offset;

   float w = (qrand() % RAND_MAX)/(float)RAND_MAX;

   if (w < 0.75)
      offset = Vector2(0.0f, 0.0f);
   else
      offset = Vector2(0.5f, 0.0f);

   return offset;
}


/*


                 |
                 |    /y
                 |   /
                 |  /
                 | /
                 |/
  ---------------+---------------
                /|              x
               / |
              /  |
             /   |
            /    |
                 |z


   x quad

      x0: 0,  0.5,  0,5
      x1: 0, -0.5,  0.5
      x2: 0, -0.5, -0.5
      x3: 0,  0.5, -0.5
      normal: 1,0,0

   y quad

      y0:  0.5, 0,  0.5
      y1: -0.5, 0,  0.5
      y2: -0.5, 0, -0.5
      y3:  0.5, 0, -0.5
      normal: 0,1,0

   z quad

      z0:  0.5,  0.5, 0
      z1: -0.5,  0.5, 0
      z2: -0.5, -0.5, 0
      z3:  0.5, -0.5, 0
      normal: 0,0,1


*/

//-----------------------------------------------------------------------------
/*!
   \param scale star scale
*/
void StarTalers::createBuffers(float scale)
{
   int starCount = 500;
   int vertexCount = starCount * 12;
   mIndexCount = starCount * 18;

   // create vertex and index buffer
   mBufferVertices = activeDevice->createVertexBuffer(
         vertexCount * sizeof(StarTalersVertex)
      );

   mBufferIndices = activeDevice->createIndexBuffer(
         mIndexCount * sizeof(unsigned short)
      );

   // fill vertex buffer
   StarTalersVertex* vtx= (StarTalersVertex*)activeDevice->lockVertexBuffer( mBufferVertices );

   int index = 0;

   for (int i=0; i < vertexCount; i+=12)
   {
      Vector direction = computeDirection(1.3f, 1.3f, 2.75f);
      float speed = computeSpeed(0.90f, 0.2f);
      Vector2 uvOffset = computeTextureOffset();

      // set positions (center: 0,0,0)
      vtx[i].mPosition = Vector(0.0f,  0.5f * scale,  0.5f * scale);
      vtx[i+1].mPosition = Vector(0.0f, -0.5f * scale,  0.5f * scale);
      vtx[i+2].mPosition = Vector(0.0f, -0.5f * scale, -0.5f * scale);
      vtx[i+3].mPosition = Vector(0.0f,  0.5f * scale, -0.5f * scale);

      vtx[i+4].mPosition = Vector( 0.5f * scale, 0.0f,  0.5f * scale);
      vtx[i+5].mPosition = Vector(-0.5f * scale, 0.0f,  0.5f * scale);
      vtx[i+6].mPosition = Vector(-0.5f * scale, 0.0f, -0.5f * scale);
      vtx[i+7].mPosition = Vector( 0.5f * scale, 0.0f, -0.5f * scale);

      vtx[i+8].mPosition = Vector( 0.5f * scale,  0.5f * scale, 0.0f);
      vtx[i+9].mPosition = Vector(-0.5f * scale,  0.5f * scale, 0.0f);
      vtx[i+10].mPosition = Vector(-0.5f * scale, -0.5f * scale, 0.0f);
      vtx[i+11].mPosition = Vector( 0.5f * scale, -0.5f * scale, 0.0f);

      // set UVs
      vtx[i].mU = uvOffset.x + 0.5f;
      vtx[i].mV = uvOffset.y + 0.5f;
      vtx[i+1].mU = uvOffset.x + 0.0f;
      vtx[i+1].mV = uvOffset.y + 0.5f;
      vtx[i+2].mU = uvOffset.x + 0.0f;
      vtx[i+2].mV = uvOffset.y + 0.0f;
      vtx[i+3].mU = uvOffset.x + 0.5f;
      vtx[i+3].mV = uvOffset.y + 0.0f;

      vtx[i+4].mU = uvOffset.x + 0.5f;
      vtx[i+4].mV = uvOffset.y + 0.5f;
      vtx[i+5].mU = uvOffset.x + 0.0f;
      vtx[i+5].mV = uvOffset.y + 0.5f;
      vtx[i+6].mU = uvOffset.x + 0.0f;
      vtx[i+6].mV = uvOffset.y + 0.0f;
      vtx[i+7].mU = uvOffset.x + 0.5f;
      vtx[i+7].mV = uvOffset.y + 0.0f;

      vtx[i+8].mU = uvOffset.x + 0.5f;
      vtx[i+8].mV = uvOffset.y + 0.5f;
      vtx[i+9].mU = uvOffset.x + 0.0f;
      vtx[i+9].mV = uvOffset.y + 0.5f;
      vtx[i+10].mU = uvOffset.x + 0.0f;
      vtx[i+10].mV = uvOffset.y + 0.0f;
      vtx[i+11].mU = uvOffset.x + 0.5f;
      vtx[i+11].mV = uvOffset.y + 0.0f;

      // set normals
      vtx[i].mNormal = Vector(1, 0, 0);
      vtx[i+1].mNormal = Vector(1, 0, 0);
      vtx[i+2].mNormal = Vector(1, 0, 0);
      vtx[i+3].mNormal = Vector(1, 0, 0);

      vtx[i+4].mNormal = Vector(0, 1, 0);
      vtx[i+5].mNormal = Vector(0, 1, 0);
      vtx[i+6].mNormal = Vector(0, 1, 0);
      vtx[i+7].mNormal = Vector(0, 1, 0);

      vtx[i+8].mNormal = Vector(0, 0, 1);
      vtx[i+9].mNormal = Vector(0, 0, 1);
      vtx[i+10].mNormal = Vector(0, 0, 1);
      vtx[i+11].mNormal = Vector(0, 0, 1);

      // set direction for every vertex
      vtx[i].mDirection = direction;
      vtx[i+1].mDirection = direction;
      vtx[i+2].mDirection = direction;
      vtx[i+3].mDirection = direction;

      vtx[i+4].mDirection = direction;
      vtx[i+5].mDirection = direction;
      vtx[i+6].mDirection = direction;
      vtx[i+7].mDirection = direction;

      vtx[i+8].mDirection = direction;
      vtx[i+9].mDirection = direction;
      vtx[i+10].mDirection = direction;
      vtx[i+11].mDirection = direction;

      // set speed for every vertex
      vtx[i].mSpeed = speed;
      vtx[i+1].mSpeed = speed;
      vtx[i+2].mSpeed = speed;
      vtx[i+3].mSpeed = speed;

      vtx[i+4].mSpeed = speed;
      vtx[i+5].mSpeed = speed;
      vtx[i+6].mSpeed = speed;
      vtx[i+7].mSpeed = speed;

      vtx[i+8].mSpeed = speed;
      vtx[i+9].mSpeed = speed;
      vtx[i+10].mSpeed = speed;
      vtx[i+11].mSpeed = speed;

      // assign every vertex group with an index
      vtx[i].mIndex = index;
      vtx[i+1].mIndex = index;
      vtx[i+2].mIndex = index;
      vtx[i+3].mIndex = index;

      vtx[i+4].mIndex = index;
      vtx[i+5].mIndex = index;
      vtx[i+6].mIndex = index;
      vtx[i+7].mIndex = index;

      vtx[i+8].mIndex = index;
      vtx[i+9].mIndex = index;
      vtx[i+10].mIndex = index;
      vtx[i+11].mIndex = index;

      index++;
   }

   activeDevice->unlockVertexBuffer(mBufferVertices);

   // fill vertex buffer
   unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer(mBufferIndices);

   index = 0;
   for (int i=0; i < mIndexCount; i+= 18)
   {
      // make indices
      idx[i] = 0 + index;
      idx[i+1] = 1 + index;
      idx[i+2] = 3 + index;

      idx[i+3] = 3 + index;
      idx[i+4] = 1 + index;
      idx[i+5] = 2 + index;

      idx[i+6] = 4 + index;
      idx[i+7] = 5 + index;
      idx[i+8] = 7 + index;

      idx[i+9] = 7 + index;
      idx[i+10] = 5 + index;
      idx[i+11] = 6 + index;

      idx[i+12] = 8 + index;
      idx[i+13] = 9 + index;
      idx[i+14] = 11 + index;

      idx[i+15] = 11 + index;
      idx[i+16] = 9 + index;
      idx[i+17] = 10 + index;

      index += 12;
   }

   activeDevice->unlockIndexBuffer(mBufferIndices);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::drawBuffers()
{
   drawVbos(&mBufferVertices, &mBufferIndices, mIndexCount);
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::generateHeightmap()
{
   /*

       ^
       |          |
       |          |     6,6
       |          |
       |          |
       |   -------+-------
       |          | p
       |     * r  |
       |   0,0    |
       +------------------->

        r absolute = (2,2)
        p absolute = (4,4)
        radius = 3

        r in absolute p coordinates
         = (2,2) - (4,4)
         = (-2,-2)

        shift r into p coordinate system
         = (-2,-2) + (radius,radius)
         = (1,1)

        location of r => 1,1

   */

   generateTestHeightmap();
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::generateTestHeightmap()
{
   memset(mHeightmap, 0, 49 * sizeof(float));

   /*
   double rand = 0.0;
   double val = 0.0;

   for (int i = 0; i < 49; i++)
   {
      val = 0.0;

      rand = (qrand() % RAND_MAX) / (double)RAND_MAX;

      if (rand > 0.8)
         val = 0.8;
      else if (rand >0.6)
         val = 1.0;

      mHeightmap[i] = val;
   }
   */

   // debugHeightmap();
}


//-----------------------------------------------------------------------------
/*!
*/
void StarTalers::debugHeightmap()
{
   QString line;

   float val = 0.0f;
   for (int i = 0; i < 49; i++)
   {
      val = mHeightmap[i];

      if (val == 0.8f)
         line.append("1");
      else if (val == 1.0)
         line.append("2");
      else
         line.append("0");

      if ( (i+1) % 7 == 0)
         line.append("\n");
   }

   qDebug("%s", qPrintable(line));
}


//-----------------------------------------------------------------------------
/*!
  \param vertexBuffer pointer to vertex buffer
  \param indexBuffer pointer to index buffer
  \param indices indices
*/
void StarTalers::drawVbos(
   unsigned int* vertexBuffer,
   unsigned int* indexBuffer,
   int mIndexCount
)
{
   // vertex structure:
   //
   //    vertex       at 0vtx[i]
   //    normal       at 0vtx[i] + sizeof(1 vector)
   //    1st texcoord at 0vtx[i] + sizeof(2 vectors)
   //    2nd texcoord at 0vtx[i] + sizeof(2 vectors) + 2 floats
   //    3rd texcoord at 0vtx[i] + sizeof(3 vectors) + 2 floats
   char* ptr = 0;

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   // vertex
   glBindBuffer( GL_ARRAY_BUFFER_ARB, *vertexBuffer );

   glVertexPointer( 3, GL_FLOAT, sizeof(StarTalersVertex), (GLvoid*)ptr );
   ptr += sizeof(Vector);

   glNormalPointer(GL_FLOAT, sizeof(StarTalersVertex), (GLvoid*)ptr );
   ptr += sizeof(Vector);

   glClientActiveTexture(GL_TEXTURE0);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer( 2, GL_FLOAT, sizeof(StarTalersVertex), (GLvoid*)ptr );
   ptr += sizeof(float) * 2;

   glClientActiveTexture(GL_TEXTURE1);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer( 3, GL_FLOAT, sizeof(StarTalersVertex), (GLvoid*)ptr);
   ptr += sizeof(Vector);

   glClientActiveTexture(GL_TEXTURE2);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer( 2, GL_FLOAT, sizeof(StarTalersVertex), (GLvoid*)ptr);
   glClientActiveTexture(GL_TEXTURE0);

   // indicies
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, *indexBuffer );

   // render
   glDrawElements( GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0 );

   glClientActiveTexture(GL_TEXTURE0);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTexture(GL_TEXTURE1);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTexture(GL_TEXTURE2);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTexture(GL_TEXTURE0);

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


