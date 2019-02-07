
// header
#include "deathflowfieldanimation.h"

// engine
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "image/image.h"
#include "math/matrix.h"
#include "math/vector4.h"
#include "tools/random.h"
#include "tools/profiling.h"

#include <math.h>

// the number of particles actually used depends on the number of "empty" pixels which is typically 1/2.
#define PARTICLE_COUNT 8000

//
#define DISSOLVE_TIME 300

//! constructor
DeathFlowFieldAnimation::DeathFlowFieldAnimation()
 : mWidth(0),
   mHeight(0),
   mVertexPosBuffer(0),
   mVertexUVBuffer(0),
   mTexture(0),
   mPage(0),
   mCenter(0.0f, 0.0f, 0.0f),
   mFlowScale(1.0f),
   mParticleSize(0.0f),
   mElapsed(0.0f)
{
   // some random offset/scale to vary the flow field effect
   mFlowScale = 0.2f;
}


//! destructor
DeathFlowFieldAnimation::~DeathFlowFieldAnimation()
{
   // delete texture/vertex/fbo resources
   glDeleteFramebuffers(1, &mTarget[0]);
   glDeleteFramebuffers(1, &mTarget[1]);
   glDeleteTextures(1, &mVertexParams);
   glDeleteTextures(1, &mPositions[0]);
   glDeleteTextures(1, &mPositions[1]);
   glDeleteBuffers( 1, &mVertexPosBuffer);
   glDeleteBuffers( 1, &mVertexUVBuffer);
   if (mTexture)
      glDeleteTextures(1, &mTexture);
}


//! return center reference position
const Vector& DeathFlowFieldAnimation::getCenter() const
{
   return mCenter;
}


//! set center reference position (in 3d world space)
void DeathFlowFieldAnimation::setCenter(const Vector& center)
{
   mCenter= center;
}

//! get color texture
unsigned int DeathFlowFieldAnimation::getColorMap() const
{
   return mTexture;
}

//! get flow field offset (xy) and scale (z)
float DeathFlowFieldAnimation::getScale() const
{
   return mFlowScale;
}


//! set flow field offset (xy) and scale (z)
void DeathFlowFieldAnimation::setScale(float scale)
{
   mFlowScale= scale;
}


//! return size of particles in unprojected units
float DeathFlowFieldAnimation::getPointSize() const
{
   return mParticleSize;
}


//! returns if effect is elapsed (all particles have disappeared)
bool DeathFlowFieldAnimation::isElapsed() const
{
   return mElapsed > DISSOLVE_TIME;
}

void DeathFlowFieldAnimation::initialize(FrameBuffer* src, const Vector& min, const Vector& max)
{
   // get 2d bounding rect (in pixels)
   int screenWidth= src->width();
   int screenHeight= src->height();

   int x0 = (int)floor( (min.x + 1.0f) * 0.5f * screenWidth );
   int y0 = (int)floor( (min.y + 1.0f) * 0.5f * screenHeight );
   int x1 = (int)ceil( (max.x + 1.0f) * 0.5f * screenWidth );
   int y1 = (int)ceil( (max.y + 1.0f) * 0.5f * screenHeight );

   int rectWidth =  x1 - x0;
   int rectHeight = y1 - y0;


   // fix step size to approx. match number of particles
   float xSkip= sqrt( (double)rectWidth*rectHeight / PARTICLE_COUNT );
   float ySkip= xSkip;

   mParticleSize= 50.0f * xSkip;

   mWidth= (int)floor(rectWidth/xSkip);
   mHeight= (int)floor(rectHeight/ySkip);

   // copy color texture (rendering of player)
   glGenTextures(1, &mTexture);
   glBindTexture(GL_TEXTURE_2D, mTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rectWidth, rectHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//   glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, src->target() );
//   glReadBuffer( GL_COLOR_ATTACHMENT0 );
   glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x0, y0, rectWidth, rectHeight, 0);
//   glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, 0 );
   glBindTexture(GL_TEXTURE_2D, 0);

   // create texture to store vertex attributes
   for (int i=0; i<2; i++)
   {
      // double buffer for vertex positions (ping/pong rendering)
      glGenTextures(1, &mPositions[i]);
      glBindTexture(GL_TEXTURE_2D, mPositions[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   // create double buffer framebuffers
   for (int i=0; i<2; i++)
   {
      glGenFramebuffers(1, &mTarget[i]);
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mPositions[i], 0);
      glClear(GL_COLOR_BUFFER_BIT);
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
   }



   // create vertexbuffer
   glGenBuffers( 1, &mVertexPosBuffer );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexPosBuffer );
   glBufferData( GL_ARRAY_BUFFER_ARB, mWidth*mHeight*sizeof(Vector4), 0, GL_DYNAMIC_DRAW_ARB );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );

   // create vertex color buffer (static)
   glGenBuffers( 1, &mVertexUVBuffer );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexUVBuffer );
   glBufferData( GL_ARRAY_BUFFER_ARB, mWidth*mHeight*sizeof(Vector2), 0, GL_DYNAMIC_DRAW_ARB );
   Vector2* dst2= (Vector2*)glMapBuffer( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY );
   float dx= 1.0f / mWidth;
   for (int y=0;y<mHeight;y++)
   {
      float fx= 0.0f;
      float fy= (float)y / mHeight;
      for (int x=0;x<mWidth;x++)
      {
         dst2->x= fx;
         dst2->y= fy;
         dst2++;
         fx+=dx;
      }
   }
   glUnmapBuffer( GL_ARRAY_BUFFER_ARB );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );

}


void DeathFlowFieldAnimation::initializePositions(unsigned int depthMap, const Vector& min, const Vector& max)
{
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget[1]);
   glViewport(0,0,mWidth,mHeight);
   glBindTexture(GL_TEXTURE_2D, depthMap);
   glBegin(GL_QUADS);
   glTexCoord2f(min.x, min.y); glVertex3f(-1,-1,-1);
   glTexCoord2f(max.x, min.y); glVertex3f( 1,-1,-1);
   glTexCoord2f(max.x, max.y); glVertex3f( 1, 1,-1);
   glTexCoord2f(min.x, max.y); glVertex3f(-1, 1,-1);
   glEnd();
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}


void DeathFlowFieldAnimation::initializeParams(unsigned int depthMap, const Vector& min, const Vector& max)
{
   unsigned int target;

   // generate texture with vertex attributes (for reading in fragment shader)
   glGenTextures(1, &mVertexParams);
   glBindTexture(GL_TEXTURE_2D, mVertexParams);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   glGenFramebuffers(1, &target);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, target);
   glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mVertexParams, 0);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

   glBindFramebuffer(GL_FRAMEBUFFER_EXT, target);
   glViewport(0,0,mWidth,mHeight);
   glBindTexture(GL_TEXTURE_2D, depthMap);
   glBegin(GL_QUADS);
   glTexCoord2f(min.x, min.y); glVertex3f(-1,-1,-1);
   glTexCoord2f(max.x, min.y); glVertex3f( 1,-1,-1);
   glTexCoord2f(max.x, max.y); glVertex3f( 1, 1,-1);
   glTexCoord2f(min.x, max.y); glVertex3f(-1, 1,-1);
   glEnd();

   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
   glDeleteFramebuffers(1, &target);
}

/*
Vector DeathFlowFieldAnimation::getFlowDir(const Image& image, int x, int y) const
{
   unsigned int* src1= mFlowField.getScanline(y);
   unsigned int* src2= mFlowField.getScanline((y+1) & 511);
   int color= (int)src[x];

   float vx = ((color & 0xff) - 128) / 128.0f - 0.04f;
   float vy = ((color >> 8 & 0xff) - 128) / 128.0f;
   float vz = ((color >> 16 & 0xff) - 128) / 256.0f;

   return Vector(vx, vz, vy);
}
*/

//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void DeathFlowFieldAnimation::update(float delta)
{      
   mElapsed += delta;


/*
   // test: copy positions to texture
   glBindTexture(GL_TEXTURE_2D, mPositions[mPage]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, mParticlePos.data());
   glBindTexture(GL_TEXTURE_2D, 0);
*/

//   glFinish();
//   double t1= getCpuTick();

   // move particles using double buffer
   // move shader doesn't use modelview/projection matrices
   // render to [mPage] and read from [mPage^1]
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget[mPage]);
   glViewport(0,0,mWidth,mHeight);
   mPage^=1; // flip pages

   glActiveTexture(GL_TEXTURE1_ARB);
   glBindTexture(GL_TEXTURE_2D, mPositions[mPage]);

   glActiveTexture(GL_TEXTURE2_ARB);
   glBindTexture(GL_TEXTURE_2D, mVertexParams);

   glActiveTexture(GL_TEXTURE0_ARB);

   glBegin(GL_QUADS);
   glVertex2f(-1,-1);
   glVertex2f( 1,-1);
   glVertex2f( 1, 1);
   glVertex2f(-1, 1);
   glEnd();


//   glFinish();
//   double t2= getCpuTick();
//   qDebug("DeathFlowFieldAnimation::update= %.3f ms", (t2-t1)/1000000.0);

/*
   // update vertex position
   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexPosBuffer );
   glBufferData( GL_ARRAY_BUFFER_ARB, mParticleCol.size()*sizeof(VertexPos), 0, GL_DYNAMIC_DRAW_ARB );
   VertexPos* dst= (VertexPos*)glMapBuffer( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY );
   memcpy(dst, mParticlePos.data(), mParticlePos.size()*sizeof(VertexPos));
   glUnmapBuffer( GL_ARRAY_BUFFER_ARB );
*/

   glActiveTexture(GL_TEXTURE2_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0_ARB);
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void DeathFlowFieldAnimation::draw()
{

   FrameBuffer* prev= FrameBuffer::Instance();
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mTarget[1-mPage]);
   glViewport(0,0, mWidth, mHeight);
   glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mVertexPosBuffer);
   glReadPixels(0,0,mWidth,mHeight,GL_RGBA,GL_FLOAT,0);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, prev->target());
   glViewport(0,0, prev->width(), prev->height());

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexPosBuffer );
   glVertexPointer( 4, GL_FLOAT, sizeof(Vector4), (GLvoid*)0 );

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexUVBuffer );
   glTexCoordPointer( 2, GL_FLOAT, sizeof(Vector2), (GLvoid*)0 );

   glDrawArrays( GL_POINTS, 0, mWidth*mHeight );

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
}

