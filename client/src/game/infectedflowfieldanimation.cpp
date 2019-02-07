
// header
#include "infectedflowfieldanimation.h"

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
#define PARTICLE_COUNT 4000

//
#define DISSOLVE_TIME 300

//! constructor
InfectedFlowFieldAnimation::InfectedFlowFieldAnimation()
 : mInitialized(false),
   mWidth(0),
   mHeight(0),
   mVertexPosBuffer(0),
   mVertexColorBuffer(0),
   mPage(0),
   mCenter(0.0f, 0.0f, 0.0f),
   mFlowScale(1.0f),
   mParticleSize(0.0f),
   mElapsed(0.0f),
   mStop(false)
{
   // some random offset/scale to vary the flow field effect
   mFlowScale = 0.2f;
}


//! destructor
InfectedFlowFieldAnimation::~InfectedFlowFieldAnimation()
{
   // delete texture/vertex/fbo resources
   glDeleteFramebuffers(1, &mPosTarget[0]);
   glDeleteFramebuffers(1, &mPosTarget[1]);
   glDeleteTextures(1, &mVertexParamTexture);
   glDeleteTextures(1, &mVertexColorTexture);
   glDeleteTextures(1, &mPositions[0]);
   glDeleteTextures(1, &mPositions[1]);
   glDeleteBuffers( 1, &mVertexPosBuffer);
   glDeleteBuffers( 1, &mVertexColorBuffer);
}


bool InfectedFlowFieldAnimation::initialized() const
{
   return mInitialized;
}

void InfectedFlowFieldAnimation::setInitialized(bool init)
{
   mInitialized= init;
}

//! return center reference position
const Vector& InfectedFlowFieldAnimation::getCenter() const
{
   return mCenter;
}


//! set center reference position (in 3d world space)
void InfectedFlowFieldAnimation::setCenter(const Vector& center)
{
   mCenter= center;
}

//! get flow field offset (xy) and scale (z)
float InfectedFlowFieldAnimation::getScale() const
{
   return mFlowScale;
}


//! return size of particles in unprojected units
float InfectedFlowFieldAnimation::getPointSize() const
{
   return mParticleSize;
}

//! stop respawning of particles
void InfectedFlowFieldAnimation::stop()
{
   mStop= true;
}

bool InfectedFlowFieldAnimation::stopped() const
{
   return mStop;
}

//! returns if effect is elapsed (all particles have disappeared)
bool InfectedFlowFieldAnimation::isElapsed() const
{
   return mElapsed > DISSOLVE_TIME;
}

void InfectedFlowFieldAnimation::initialize()
{
   // 64*48= ~3000 particles
   mParticleSize= 60.0f;
   mWidth= 64;
   mHeight= 48;

   // generate texture with vertex attributes (for reading in fragment shader)
   glGenTextures(1, &mVertexParamTexture);
   glBindTexture(GL_TEXTURE_2D, mVertexParamTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   // target for vertex params
   glGenFramebuffers(1, &mParamTarget);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mParamTarget);
   glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mVertexParamTexture, 0);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

   // generate texture with vertex colors
   glGenTextures(1, &mVertexColorTexture);
   glBindTexture(GL_TEXTURE_2D, mVertexColorTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   // target for color texture
   glGenFramebuffers(1, &mColorTarget);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mColorTarget);
   glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mVertexColorTexture, 0);
   glClear(GL_COLOR_BUFFER_BIT);
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

   // create double buffer texture to store vertex attributes
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
      glGenFramebuffers(1, &mPosTarget[i]);
      glBindFramebuffer(GL_FRAMEBUFFER_EXT, mPosTarget[i]);
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
   glGenBuffers( 1, &mVertexColorBuffer );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexColorBuffer );
   glBufferData( GL_ARRAY_BUFFER_ARB, mWidth*mHeight*sizeof(Vector4), 0, GL_DYNAMIC_DRAW_ARB );
   glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );

}


void InfectedFlowFieldAnimation::initializePositions(const Vector& min, const Vector& max)
{
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mPosTarget[1]);
   glViewport(0,0,mWidth,mHeight);
   glBegin(GL_QUADS);
   glTexCoord2f(min.x, min.y); glVertex3f(-1,-1,-1);
   glTexCoord2f(max.x, min.y); glVertex3f( 1,-1,-1);
   glTexCoord2f(max.x, max.y); glVertex3f( 1, 1,-1);
   glTexCoord2f(min.x, max.y); glVertex3f(-1, 1,-1);
   glEnd();
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}


void InfectedFlowFieldAnimation::initializeParams(const Vector& min, const Vector& max)
{

   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mParamTarget);
   glViewport(0,0,mWidth,mHeight);
   glBegin(GL_QUADS);
   glTexCoord2f(min.x, min.y); glVertex3f(-1,-1,-1);
   glTexCoord2f(max.x, min.y); glVertex3f( 1,-1,-1);
   glTexCoord2f(max.x, max.y); glVertex3f( 1, 1,-1);
   glTexCoord2f(min.x, max.y); glVertex3f(-1, 1,-1);
   glEnd();

   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}


/*
Vector InfectedFlowFieldAnimation::getFlowDir(const Image& image, int x, int y) const
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

void InfectedFlowFieldAnimation::updatePositions(float delta)
{      
   if (mStop)
      mElapsed += delta;

   // move particles using double buffer
   // move shader doesn't use modelview/projection matrices
   // render to [mPage] and read from [mPage^1]
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mPosTarget[mPage]);
   glViewport(0,0,mWidth,mHeight);
   mPage^=1; // flip pages

   glActiveTexture(GL_TEXTURE1_ARB);
   glBindTexture(GL_TEXTURE_2D, mPositions[mPage]);

   glActiveTexture(GL_TEXTURE2_ARB);
   glBindTexture(GL_TEXTURE_2D, mVertexParamTexture);

   glActiveTexture(GL_TEXTURE0_ARB);

   glBegin(GL_QUADS);
   glVertex2f(-1,-1);
   glVertex2f( 1,-1);
   glVertex2f( 1, 1);
   glVertex2f(-1, 1);
   glEnd();


   glActiveTexture(GL_TEXTURE2_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0_ARB);
}


void InfectedFlowFieldAnimation::updateColors()
{
   // move particles using double buffer
   // move shader doesn't use modelview/projection matrices
   // render to [mPage] and read from [mPage^1]
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mColorTarget);
   glViewport(0,0,mWidth,mHeight);

   glBindTexture(GL_TEXTURE_2D, mPositions[1-mPage]);
   glBegin(GL_QUADS);
   glVertex3f(-1,-1, -1);
   glVertex3f( 1,-1, -1);
   glVertex3f( 1, 1, -1);
   glVertex3f(-1, 1, -1);
   glEnd();
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}

//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void InfectedFlowFieldAnimation::draw()
{
   FrameBuffer* prev= FrameBuffer::Instance();
   // copy particle positions from texture to vertex buffer
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mColorTarget);
   glViewport(0,0, mWidth, mHeight);
   glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mVertexColorBuffer);
   glReadPixels(0,0,mWidth,mHeight,GL_RGBA,GL_FLOAT,0);

   // copy particle colors from texture to vertex buffer
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, mPosTarget[1-mPage]);
   glViewport(0,0, mWidth, mHeight);
   glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, mVertexPosBuffer);
   glReadPixels(0,0,mWidth,mHeight,GL_RGBA,GL_FLOAT,0);

   // restore previous buffer and size
   glBindFramebuffer(GL_FRAMEBUFFER_EXT, prev->target());
   glViewport(0,0, prev->width(), prev->height());

   // draw vertex buffer
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexPosBuffer );
   glVertexPointer( 4, GL_FLOAT, sizeof(Vector4), (GLvoid*)0 );

   glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertexColorBuffer );
   glColorPointer( 4, GL_FLOAT, sizeof(Vector4), (GLvoid*)0 );

   glDrawArrays( GL_POINTS, 0, mWidth*mHeight );

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
}

