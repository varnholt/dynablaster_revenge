// GLES3 port of client/src/game/deathflowfieldanimation.cpp - see project memory for the full
// GPGPU-particle-simulation background (this is a "render positions into a float texture, ping-
// pong update them via fragment shader passes, read them back into a VBO via glReadPixels" style
// particle simulation, not a straightforward material port). Every glBegin(GL_QUADS)/glVertex3f/
// glTexCoord2f fullscreen-quad pass becomes a small dynamic attribute-array draw (2 triangles),
// same treatment already established elsewhere in this port (e.g. MenuPageListItem::drawRows()).
// glMapBuffer (no GLES3 equivalent) becomes glMapBufferRange. Fixed-function client-state arrays
// in draw() (glEnableClientState/glVertexPointer/glTexCoordPointer) become glVertexAttribPointer/
// glEnableVertexAttribArray, matching the deathparticles-vert.glsl attribute layout (location 0 =
// position.xyz + alpha.w, location 1 = static per-particle UV).

#include "deathflowfieldanimation.h"

#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "math/vector2.h"

#include <cmath>

// the number of particles actually used depends on the number of "empty" pixels which is typically 1/2.
#define PARTICLE_COUNT 8000

#define DISSOLVE_TIME 300

namespace
{
// one persistent VBO reused across every call (re-uploaded via glBufferData each time, not
// gen/delete'd) - this is called every frame for the ~5-second particle lifetime, and churning a
// fresh GL buffer object on every single call was a real performance/stability risk (buffer
// deletion can force an implicit GPU sync point on some drivers).
GLuint gQuadVertexBuffer = 0;

void drawQuad(const float* verts, int floatsPerVertex)
{
   const int order[6] = {0, 1, 2, 0, 2, 3};
   float buffer[6 * 4];  // up to 4 floats/vertex (pos.xy + uv.xy), 6 verts

   for (int i = 0; i < 6; i++)
   {
      const float* src = verts + order[i] * floatsPerVertex;
      float* dst = buffer + i * floatsPerVertex;
      for (int c = 0; c < floatsPerVertex; c++)
         dst[c] = src[c];
   }

   if (gQuadVertexBuffer == 0)
      glGenBuffers(1, &gQuadVertexBuffer);

   glBindBuffer(GL_ARRAY_BUFFER, gQuadVertexBuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * floatsPerVertex, buffer, GL_DYNAMIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * floatsPerVertex, (GLvoid*)0);

   if (floatsPerVertex > 2)
   {
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * floatsPerVertex, (GLvoid*)(sizeof(float) * 2));
   }

   glDrawArrays(GL_TRIANGLES, 0, 6);

   glDisableVertexAttribArray(0);
   if (floatsPerVertex > 2)
      glDisableVertexAttribArray(1);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}  // namespace

DeathFlowFieldAnimation::DeathFlowFieldAnimation()
   : mWidth(0),
     mHeight(0),
     mVertexPosBuffer(0),
     mVertexUVBuffer(0),
     mVertexParams(0),
     mTexture(0),
     mPage(0),
     mCenter(0.0f, 0.0f, 0.0f),
     mFlowScale(0.2f),
     mParticleSize(0.0f),
     mElapsed(0.0f)
{
   mPositions[0] = mPositions[1] = 0;
   mTarget[0] = mTarget[1] = 0;
}

DeathFlowFieldAnimation::~DeathFlowFieldAnimation()
{
   glDeleteFramebuffers(1, &mTarget[0]);
   glDeleteFramebuffers(1, &mTarget[1]);
   glDeleteTextures(1, &mVertexParams);
   glDeleteTextures(1, &mPositions[0]);
   glDeleteTextures(1, &mPositions[1]);
   glDeleteBuffers(1, &mVertexPosBuffer);
   glDeleteBuffers(1, &mVertexUVBuffer);
   if (mTexture)
      glDeleteTextures(1, &mTexture);
}

const Vector& DeathFlowFieldAnimation::getCenter() const
{
   return mCenter;
}

void DeathFlowFieldAnimation::setCenter(const Vector& center)
{
   mCenter = center;
}

unsigned int DeathFlowFieldAnimation::getColorMap() const
{
   return mTexture;
}

float DeathFlowFieldAnimation::getScale() const
{
   return mFlowScale;
}

void DeathFlowFieldAnimation::setScale(float scale)
{
   mFlowScale = scale;
}

float DeathFlowFieldAnimation::getPointSize() const
{
   return mParticleSize;
}

bool DeathFlowFieldAnimation::isElapsed() const
{
   return mElapsed > DISSOLVE_TIME;
}

void DeathFlowFieldAnimation::initialize(FrameBuffer* src, const Vector& min, const Vector& max)
{
   // get 2d bounding rect (in pixels)
   int screenWidth = src->width();
   int screenHeight = src->height();

   int x0 = (int)floor((min.x + 1.0f) * 0.5f * screenWidth);
   int y0 = (int)floor((min.y + 1.0f) * 0.5f * screenHeight);
   int x1 = (int)ceil((max.x + 1.0f) * 0.5f * screenWidth);
   int y1 = (int)ceil((max.y + 1.0f) * 0.5f * screenHeight);

   int rectWidth = x1 - x0;
   int rectHeight = y1 - y0;

   // fix step size to approx. match number of particles
   float xSkip = std::sqrt((double)rectWidth * rectHeight / PARTICLE_COUNT);
   float ySkip = xSkip;

   mParticleSize = 50.0f * xSkip;

   mWidth = (int)floor(rectWidth / xSkip);
   mHeight = (int)floor(rectHeight / ySkip);

   if (mWidth < 1)
      mWidth = 1;
   if (mHeight < 1)
      mHeight = 1;

   // copy color texture (rendering of player) - src is already bound as the current framebuffer
   // by PlayerDeathEffect::add() at this point.
   glGenTextures(1, &mTexture);
   glBindTexture(GL_TEXTURE_2D, mTexture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x0, y0, rectWidth, rectHeight, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   // double buffer for vertex positions (ping/pong rendering)
   for (int i = 0; i < 2; i++)
   {
      glGenTextures(1, &mPositions[i]);
      glBindTexture(GL_TEXTURE_2D, mPositions[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   // create double buffer framebuffers
   for (int i = 0; i < 2; i++)
   {
      glGenFramebuffers(1, &mTarget[i]);
      glBindFramebuffer(GL_FRAMEBUFFER, mTarget[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPositions[i], 0);
      glClear(GL_COLOR_BUFFER_BIT);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   // create vertexbuffer (filled later, per frame, via draw()'s glReadPixels)
   glGenBuffers(1, &mVertexPosBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, mVertexPosBuffer);
   glBufferData(GL_ARRAY_BUFFER, mWidth * mHeight * sizeof(Vector4), 0, GL_DYNAMIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // create vertex UV buffer (static) - one UV per particle, addressing the position/param textures
   glGenBuffers(1, &mVertexUVBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, mVertexUVBuffer);
   const GLsizeiptr uvBufferSize = (GLsizeiptr)mWidth * mHeight * sizeof(Vector2);
   glBufferData(GL_ARRAY_BUFFER, uvBufferSize, 0, GL_DYNAMIC_DRAW);
   Vector2* dst2 = (Vector2*)glMapBufferRange(GL_ARRAY_BUFFER, 0, uvBufferSize, GL_MAP_WRITE_BIT);
   float dx = 1.0f / mWidth;
   for (int y = 0; y < mHeight; y++)
   {
      float fx = 0.0f;
      float fy = (float)y / mHeight;
      for (int x = 0; x < mWidth; x++)
      {
         dst2->x = fx;
         dst2->y = fy;
         dst2++;
         fx += dx;
      }
   }
   glUnmapBuffer(GL_ARRAY_BUFFER);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DeathFlowFieldAnimation::initializePositions(unsigned int depthMap, const Vector& min, const Vector& max)
{
   glBindFramebuffer(GL_FRAMEBUFFER, mTarget[1]);
   glViewport(0, 0, mWidth, mHeight);
   glBindTexture(GL_TEXTURE_2D, depthMap);

   const float quad[4 * 4] = {
      -1.0f, -1.0f, min.x, min.y, 1.0f, -1.0f, max.x, min.y, 1.0f, 1.0f, max.x, max.y, -1.0f, 1.0f, min.x, max.y,
   };
   drawQuad(quad, 4);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeathFlowFieldAnimation::initializeParams(unsigned int depthMap, const Vector& min, const Vector& max)
{
   unsigned int target = 0;

   glGenTextures(1, &mVertexParams);
   glBindTexture(GL_TEXTURE_2D, mVertexParams);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   glGenFramebuffers(1, &target);
   glBindFramebuffer(GL_FRAMEBUFFER, target);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mVertexParams, 0);

   glViewport(0, 0, mWidth, mHeight);
   glBindTexture(GL_TEXTURE_2D, depthMap);

   const float quad[4 * 4] = {
      -1.0f, -1.0f, min.x, min.y, 1.0f, -1.0f, max.x, min.y, 1.0f, 1.0f, max.x, max.y, -1.0f, 1.0f, min.x, max.y,
   };
   drawQuad(quad, 4);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDeleteFramebuffers(1, &target);
}

void DeathFlowFieldAnimation::update(float delta)
{
   mElapsed += delta;

   // move particles using double buffer: render to [mPage] and read from [mPage^1]
   glBindFramebuffer(GL_FRAMEBUFFER, mTarget[mPage]);
   glViewport(0, 0, mWidth, mHeight);
   mPage ^= 1;  // flip pages

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, mPositions[mPage]);

   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, mVertexParams);

   glActiveTexture(GL_TEXTURE0);

   const float quad[4 * 2] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
   drawQuad(quad, 2);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeathFlowFieldAnimation::draw()
{
   FrameBuffer* prev = FrameBuffer::Instance();

   // read the GPU-computed positions straight back into mVertexPosBuffer - no transform feedback
   // in this GL version, so this manually does what transform feedback would.
   glBindFramebuffer(GL_FRAMEBUFFER, mTarget[1 - mPage]);
   glViewport(0, 0, mWidth, mHeight);
   glBindBuffer(GL_PIXEL_PACK_BUFFER, mVertexPosBuffer);
   glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_FLOAT, 0);
   glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

   if (prev)
   {
      glBindFramebuffer(GL_FRAMEBUFFER, prev->target());
      glViewport(0, 0, prev->width(), prev->height());
   }
   else
   {
      // no FrameBuffer currently pushed (this port has no MainDrawable owning a top-level FBO,
      // unlike the original desktop game where `prev` was never null here) - restore the
      // viewport to the real screen size, matching FrameBuffer::pop()'s own established
      // convention for this exact case, instead of leaving it stuck at mWidth x mHeight (the
      // tiny particle-texture size) for the rest of this frame and every frame after it.
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(activeDevice->getBorderLeft(), activeDevice->getBorderBottom(), activeDevice->getWidth(), activeDevice->getHeight());
   }

   glBindBuffer(GL_ARRAY_BUFFER, mVertexPosBuffer);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), (GLvoid*)0);

   glBindBuffer(GL_ARRAY_BUFFER, mVertexUVBuffer);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (GLvoid*)0);

   glDrawArrays(GL_POINTS, 0, mWidth * mHeight);

   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}
