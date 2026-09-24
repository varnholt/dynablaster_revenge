// GLES3 port of client/src/game/playerdeatheffect.cpp. See project memory for the full
// GPGPU-particle-simulation background and deathflowfieldanimation.cpp's own header comment for
// the fullscreen-quad/glMapBuffer/client-state adaptations. This file's own adaptations:
//
// - MainDrawable::getInstance()->getRenderBuffer(1) doesn't exist in this port (no MainDrawable -
//   see project memory on the menu system's own equivalent fix) - PlayerDeathEffect now owns its
//   own dedicated FrameBuffer (mDeferredBuffer, DepthTexture-flagged), lazily sized to the current
//   game viewport, matching MenuDrawable's own established "own your FBO instead of borrowing
//   MainDrawable's" pattern.
// - glGetFloatv(GL_PROJECTION_MATRIX, ...) becomes activeDevice->getProjectionMatrix() (this
//   engine's own established equivalent, used everywhere else already).
// - gl_ProjectionMatrix in deathparticles-vert.glsl needed an explicit u_projection uniform (the
//   original relied on the fixed-function matrix stack) - mPointsProjection, set once per render()
//   call via activeDevice->getProjectionMatrix() (same value the particles' world-space positions
//   were captured against).
// - deathinitparams-frag.glsl's "center" uniform is a vec2 (only x/y are ever read) - passed as a
//   Vector2 rather than the full 3-component Vector center used everywhere else, to match the
//   shader's actual declared type exactly rather than relying on a GL driver tolerating a size
//   mismatch.
// - glPushMatrix/glLoadIdentity/glPopMatrix -> activeDevice->push(Matrix())/pop() (identity world
//   transform - particle positions are already baked into world space at capture time, matching
//   this port's established BitmapFont::draw() convention for the same situation).
// - glEnable/glDisable(GL_TEXTURE_2D), glEnable(GL_POINT_SPRITE)/glEnable(
//   GL_VERTEX_PROGRAM_POINT_SIZE) and glColor4f() have no GLES3 equivalent and are dropped -
//   textures are always "enabled" once sampled by a shader, point sprites/vertex-shader point
//   size are always active, and there is no fixed-function color to reset.

#include "playerdeatheffect.h"
#include "deathflowfieldanimation.h"
#include "framework/framebuffer.h"
#include "framework/gldevice.h"
#include "materials/material.h"
#include "nodes/node.h"
#include "nodes/mesh.h"
#include "render/geometry.h"
#include "math/vector.h"
#include "math/vector2.h"
#include "math/matrix.h"
#include "image/image.h"

PlayerDeathEffect::PlayerDeathEffect()
   : mParticleTextureId(0), mPointsShader(0), mPointsTexture(0), mDeferredBuffer(nullptr)
{
   Image image;
   image.load("data/game/flowfield_pointsprite");
   mParticleTextureId = activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight());

   image.load("data/game/flowfield");
   mFlowFieldTextureId = activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight(), 1);

   mPointsShader = activeDevice->loadShader("deathparticles-vert.glsl", "deathparticles-frag.glsl");
   mPointsTexture = activeDevice->getParameterIndex("texturemap");
   mPointsColorMap = activeDevice->getParameterIndex("colormap");
   mPointsSize = activeDevice->getParameterIndex("particleSize");
   mPointsProjection = activeDevice->getParameterIndex("u_projection");

   mFlowShader = activeDevice->loadShader("deathflowfield-vert.glsl", "deathflowfield-frag.glsl");
   mFlowVertexPosTexture = activeDevice->getParameterIndex("vertexPosTexture");
   mFlowVertexColTexture = activeDevice->getParameterIndex("vertexColTexture");
   mFlowFieldTexture = activeDevice->getParameterIndex("flowfieldTexture");
   mFlowCenter = activeDevice->getParameterIndex("center");
   mFlowFieldScale = activeDevice->getParameterIndex("fieldScale");
   mFlowTimeDelta = activeDevice->getParameterIndex("timeDelta");

   mFlowInitPosShader = activeDevice->loadShader("deathinitpositions-vert.glsl", "deathinitpositions-frag.glsl");
   mFlowInitPosDepth = activeDevice->getParameterIndex("depthmap");
   mFlowInitPosInvProj = activeDevice->getParameterIndex("invProj");

   mFlowInitParamShader = activeDevice->loadShader("deathinitparams-vert.glsl", "deathinitparams-frag.glsl");
   mFlowInitParamDepth = activeDevice->getParameterIndex("depthmap");
   mFlowInitParamInvProj = activeDevice->getParameterIndex("invProj");
   mFlowInitParamCenter = activeDevice->getParameterIndex("center");
}

PlayerDeathEffect::~PlayerDeathEffect()
{
   clear();
   delete mDeferredBuffer;
}

void PlayerDeathEffect::clear()
{
   qDeleteAll(mFlowAnimations);
   mFlowAnimations.clear();
}

void PlayerDeathEffect::add(Material* material)
{
   Vector min;
   Vector max;

   if (!material)
      return;

   const int width = activeDevice->getWidth();
   const int height = activeDevice->getHeight();

   if (!mDeferredBuffer)
      mDeferredBuffer = new FrameBuffer(width, height, 0, FrameBuffer::DepthTexture);
   else if (mDeferredBuffer->resolutionChanged(width, height))
      mDeferredBuffer->setResolution(width, height);

   FrameBuffer::push(mDeferredBuffer);

   // TODO: clear relevant area only!
   activeDevice->clear();

   Matrix proj = static_cast<GLDevice*>(activeDevice)->getProjectionMatrix();
   Matrix invProj = proj.invert4x4();

   material->renderDiffuse();

   // get pivot in world space
   Vector center = material->getGeometry(0)->getTransform().translation();

   material->getBoundingRect(min, max, proj);

   DeathFlowFieldAnimation* animation = new DeathFlowFieldAnimation();
   animation->setCenter(center);

   animation->initialize(mDeferredBuffer, min, max);

   activeDevice->setShader(mFlowInitPosShader);
   activeDevice->bindSampler(mFlowInitPosDepth, 0);
   activeDevice->setParameter(mFlowInitPosInvProj, invProj);
   animation->initializePositions(mDeferredBuffer->depthTexture(), min, max);
   activeDevice->setShader(0);

   activeDevice->setShader(mFlowInitParamShader);
   activeDevice->bindSampler(mFlowInitParamDepth, 0);
   activeDevice->setParameter(mFlowInitParamInvProj, invProj);
   activeDevice->setParameter(mFlowInitParamCenter, Vector2(center.x, center.y));
   animation->initializeParams(mDeferredBuffer->depthTexture(), min, max);
   activeDevice->setShader(0);

   FrameBuffer::pop();

   mFlowAnimations << animation;
}

void PlayerDeathEffect::animate(float delta)
{
   FrameBuffer::push();

   activeDevice->setShader(mFlowShader);

   glDepthMask(false);
   glDisable(GL_DEPTH_TEST);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFlowFieldTextureId);
   activeDevice->bindSampler(mFlowFieldTexture, 0);

   glActiveTexture(GL_TEXTURE1);
   activeDevice->bindSampler(mFlowVertexPosTexture, 1);

   glActiveTexture(GL_TEXTURE2);
   activeDevice->bindSampler(mFlowVertexColTexture, 2);

   for (auto it = mFlowAnimations.begin(); it != mFlowAnimations.end();)
   {
      DeathFlowFieldAnimation* flow = *it;
      if (flow->isElapsed())
      {
         it = mFlowAnimations.erase(it);
         delete flow;
      }
      else
      {
         activeDevice->setParameter(mFlowCenter, flow->getCenter());
         activeDevice->setParameter(mFlowFieldScale, flow->getScale());
         activeDevice->setParameter(mFlowTimeDelta, delta);

         flow->update(delta);
         ++it;
      }
   }

   glDepthMask(true);
   glEnable(GL_DEPTH_TEST);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);

   FrameBuffer::pop();
}

void PlayerDeathEffect::render()
{
   // particle positions are already baked into world space (captured once at death time) - push
   // an identity world transform, matching BitmapFont::draw()'s established convention for the
   // same situation.
   activeDevice->setShader(mPointsShader);
   activeDevice->setParameter(mPointsProjection, static_cast<GLDevice*>(activeDevice)->getProjectionMatrix());
   activeDevice->push(Matrix());

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mParticleTextureId);
   activeDevice->bindSampler(mPointsTexture, 0);

   glActiveTexture(GL_TEXTURE1);
   activeDevice->bindSampler(mPointsColorMap, 1);

   float sizeFactor = 1.0f;
   FrameBuffer* fb = FrameBuffer::Instance();
   if (fb)
      sizeFactor = fb->getSizeFactor(1920.0f);

   for (DeathFlowFieldAnimation* flow : mFlowAnimations)
   {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, flow->getColorMap());
      activeDevice->setParameter(mPointsSize, flow->getPointSize() * sizeFactor);

      flow->draw();
   }

   glActiveTexture(GL_TEXTURE0);

   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);

   activeDevice->pop();
   activeDevice->setShader(0);
}
