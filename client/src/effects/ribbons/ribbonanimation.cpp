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
#include "ribbonanimation.h"

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

// static
Texture RibbonAnimation::sTexture;
unsigned int RibbonAnimation::mShader = 0;
int RibbonAnimation::mTimeParam = 0;
int RibbonAnimation::mFieldPositionParam = 0;
int RibbonAnimation::mCircleOffsetParam = 0;
unsigned int RibbonAnimation::mVertexBuffer;
unsigned int RibbonAnimation::mIndexBuffer;
QVector<RibbonVertex> RibbonAnimation::mVertices;
QVector<GLuint> RibbonAnimation::mIndices;

// defines
#define DURATION 8.0f;



//-----------------------------------------------------------------------------
/*!
*/
RibbonAnimation::RibbonAnimation()
 : mTime(0.0f)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimation::initializeTexture()
{
   TexturePool* pool= TexturePool::Instance();
   sTexture = pool->getTexture("ribbon");
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimation::initialize()
{
   FileStream::addPath("data/effects/ribbons/shaders");
   FileStream::addPath("data/effects/ribbons/images");

   initializeTexture();

   // init shader
   mShader = activeDevice->loadShader(
      "ribbon-vert.glsl",
      "ribbon-frag.glsl"
   );

   mTimeParam = activeDevice->getParameterIndex("time");
   mFieldPositionParam = activeDevice->getParameterIndex("fieldPosition");
   mCircleOffsetParam = activeDevice->getParameterIndex("circleOffset");

   initVbos();
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void RibbonAnimation::updateTime(float dt)
{
   mTime += dt;
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void RibbonAnimation::draw(float dt)
{
   dt *= 13.5f;
   updateTime(dt);

   // qDebug("anim: %x, time: %f", this, mTime);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, sTexture.getTexture());

   activeDevice->setShader(mShader);

   float offset = 0.0f;
   int count = 10;

   for (int i = 0; i < count; i++)
   {
      offset += (float)(2.0f * M_PI)/(double)count;

      activeDevice->setParameter(mTimeParam, mTime);
      activeDevice->setParameter(mFieldPositionParam, mFieldPosition);
      activeDevice->setParameter(mCircleOffsetParam, offset);
      drawVbos();
   }

   activeDevice->setShader(0);

   glDisable(GL_TEXTURE_2D);
}


//-----------------------------------------------------------------------------
/*!
   \param fieldPosition animation field position
*/
void RibbonAnimation::setFieldPosition(const Vector4 &fieldPosition)
{
   mFieldPosition = fieldPosition;
}



//-----------------------------------------------------------------------------
/*!
  \return animation field position
*/
Vector4 RibbonAnimation::getFieldPosition() const
{
   return mFieldPosition;
}


//-----------------------------------------------------------------------------
/*!
  \return \c true if time is elapsed
*/
bool RibbonAnimation::isElapsed() const
{
   return mTime > DURATION;
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimation::initVbos()
{
   int gridWidthSegments = 500;
   int gridHeightSegments = 2;

   float gridWidth = 6.0f * (float)M_PI;
   float gridHeight = 1.0f;

   int vertexCount = gridWidthSegments * gridHeightSegments;

   // init data
   mVertices.resize(vertexCount);

   for (int x = 0; x < gridWidthSegments; x++)
   {
      for (int y = 0; y < gridHeightSegments; y++)
      {
         RibbonVertex v;

         v.mPosition.x = (x / (float)(gridWidthSegments))  * gridWidth;
         v.mPosition.y = (y / (float)(gridHeightSegments)) * gridHeight;
         v.mPosition.z = 0.0f;

         v.mNormal.x = 0.0f;
         v.mNormal.y = 0.0f;
         v.mNormal.z = 0.0f;

         v.mU = x / (float)(gridWidthSegments - 1);
         v.mV = y / (float)(gridHeightSegments - 1);

         // qDebug("%f", v.mU);

         mVertices[y * gridWidthSegments + x] = v;
      }
   }

   /*

     x, y    x+1, y
       A ----- C
       |     / |
       |    /  |
       |   /   |
       |  /    |
       | /     |
       B ----- D
     x, y+1  x+1, y+1

     tri 1: A, B, C
     tri 2: C, B, D

   */

   int tris = 0;

   for (int x = 0; x < gridWidthSegments-1; x++)
   {
      for (int y = 0; y < gridHeightSegments-1; y++)
      {
         GLuint a = (y * gridWidthSegments) + x;
         GLuint b = ((y+1) * gridWidthSegments) + x;
         GLuint c = (y * gridWidthSegments) + (x + 1);
         GLuint d = ((y+1) * gridWidthSegments) + (x + 1);

         mIndices.push_back(a);
         mIndices.push_back(b);
         mIndices.push_back(c);

         mIndices.push_back(c);
         mIndices.push_back(b);
         mIndices.push_back(d);

         tris+=2;
      }
   }

   // create vertex and index buffer
   mVertexBuffer = activeDevice->createVertexBuffer(vertexCount * sizeof(RibbonVertex));
   mIndexBuffer = activeDevice->createIndexBuffer( mIndices.count() * sizeof(unsigned short) );


   // fill vertex buffer
   RibbonVertex* vtx= (RibbonVertex*)activeDevice->lockVertexBuffer( mVertexBuffer );

   for (int i=0; i < mVertices.count(); i++)
   {
      vtx[i].mPosition= mVertices[i].mPosition;
      vtx[i].mNormal= mVertices[i].mNormal;
      vtx[i].mU= mVertices[i].mU;
      vtx[i].mV= mVertices[i].mV;
   }

   activeDevice->unlockVertexBuffer(mVertexBuffer);

   // fill vertex buffer
   unsigned short* idx= (unsigned short*)activeDevice->lockIndexBuffer( mIndexBuffer );

   for (int i=0; i<mIndices.count(); i++)
   {
      *idx++= mIndices[i];
   }

   activeDevice->unlockIndexBuffer(mIndexBuffer);
}


//-----------------------------------------------------------------------------
/*!
*/
void RibbonAnimation::drawVbos()
{
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexBuffer );
   glVertexPointer( 3, GL_FLOAT, sizeof(RibbonVertex), (GLvoid*)0 );
   glNormalPointer(GL_FLOAT, sizeof(RibbonVertex), (GLvoid*)sizeof(Vector) );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(RibbonVertex), (GLvoid*)(sizeof(Vector)*2) );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, mIndexBuffer );
   glDrawElements( GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, 0 ); // render

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


