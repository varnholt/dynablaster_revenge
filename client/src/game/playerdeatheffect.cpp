#include "playerdeatheffect.h"
#include "deathflowfieldanimation.h"
#include "framework/maindrawable.h"
#include "framework/framebuffer.h"
#include "framework/gldevice.h"
#include "materials/material.h"
#include "nodes/node.h"
#include "nodes/mesh.h"
#include "render/geometry.h"
#include "tools/random.h"
#include "math/vector.h"
#include "math/matrix.h"

#include <math.h>

/*
   death flow field animation workflow:

   iterate through all players on every draw-call
      if player is dead
         if animation frame exceeds 10.000
            render player to fbo
            init new flow animation
            add flow animation to list
            set player to invisible (mesh->getPart(0)->setVisible(false);)

*/

PlayerDeathEffect::PlayerDeathEffect()
   : mParticleTextureId(0)
   , mPointsShader(0)
   , mPointsTexture(0)
{
   Image image;
   image.load("data/game/flowfield_pointsprite");

   mParticleTextureId = activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight());

   image.load("data/game/flowfield");
   mFlowFieldTextureId = activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight(), 1);

   mPointsShader= activeDevice->loadShader("deathparticles-vert.glsl", "deathparticles-frag.glsl");
   mPointsTexture= activeDevice->getParameterIndex("texturemap");
   mPointsColorMap= activeDevice->getParameterIndex("colormap");
   mPointsSize= activeDevice->getParameterIndex("particleSize");

   mFlowShader= activeDevice->loadShader("deathflowfield-vert.glsl", "deathflowfield-frag.glsl");
   mFlowVertexPosTexture= activeDevice->getParameterIndex("vertexPosTexture");
   mFlowVertexColTexture= activeDevice->getParameterIndex("vertexColTexture");
   mFlowFieldTexture= activeDevice->getParameterIndex("flowfieldTexture");
   mFlowCenter= activeDevice->getParameterIndex("center");
   mFlowFieldScale= activeDevice->getParameterIndex("fieldScale");
   mFlowTimeDelta= activeDevice->getParameterIndex("timeDelta");

   mFlowInitPosShader= activeDevice->loadShader("deathinitpositions-vert.glsl", "deathinitpositions-frag.glsl");
   mFlowInitPosDepth= activeDevice->getParameterIndex("depthmap");
   mFlowInitPosInvProj= activeDevice->getParameterIndex("invProj");

   mFlowInitParamShader= activeDevice->loadShader("deathinitparams-vert.glsl", "deathinitparams-frag.glsl");
   mFlowInitParamDepth= activeDevice->getParameterIndex("depthmap");
   mFlowInitParamInvProj= activeDevice->getParameterIndex("invProj");
   mFlowInitParamCenter= activeDevice->getParameterIndex("center");

//   mFlowInitColorShader= activeDevice->loadShader("deathflowfield-vert.glsl", "deathflowfield-frag.glsl");
}

PlayerDeathEffect::~PlayerDeathEffect()
{
   clear();
}


void PlayerDeathEffect::clear()
{
   qDeleteAll(mFlowAnimations);
   mFlowAnimations.clear();
}



void PlayerDeathEffect::add( Material* material )
{
   Vector min;
   Vector max;

   if (!material)
      return;

   FrameBuffer* deferred= MainDrawable::getInstance()->getRenderBuffer(1);
   FrameBuffer::push( deferred );

   // TODO: clear relevant area only!
   activeDevice->clear();

   Matrix proj;
   glGetFloatv(GL_PROJECTION_MATRIX, proj.data());
   Matrix invProj= proj.invert4x4();

   material->renderDiffuse();


   // get pivot in world space
   Vector center= material->getGeometry(0)->getTransform().translation();

   material->getBoundingRect(min, max, proj);


   DeathFlowFieldAnimation* animation = new DeathFlowFieldAnimation();
   animation->setCenter(center);

   animation->initialize(deferred, min, max);

//   deferred->unbind();

   activeDevice->setShader( mFlowInitPosShader );
   activeDevice->bindSampler( mFlowInitPosDepth, 0 );
   activeDevice->setParameter(mFlowInitPosInvProj, invProj );
   animation->initializePositions(deferred->depthTexture(), min, max);
   activeDevice->setShader( 0 );


   activeDevice->setShader( mFlowInitParamShader );
   activeDevice->bindSampler( mFlowInitParamDepth, 0 );
   activeDevice->setParameter(mFlowInitParamInvProj, invProj );
   activeDevice->setParameter(mFlowInitParamCenter, center );
   animation->initializeParams(deferred->depthTexture(), min, max);
   activeDevice->setShader( 0 );


   // img->save(QString("image_%1.png").arg(mScreenshotCounter++));

   FrameBuffer::pop();

   mFlowAnimations << animation;
}


// animate flow fields
void PlayerDeathEffect::animate(float delta)
{
   FrameBuffer::push();

   activeDevice->setShader( mFlowShader );

   glDepthMask(false);
   glDisable(GL_DEPTH_TEST);

   glActiveTexture(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mFlowFieldTextureId);
   activeDevice->bindSampler(mFlowFieldTexture, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   activeDevice->bindSampler(mFlowVertexPosTexture, 1);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   activeDevice->bindSampler(mFlowVertexColTexture, 2);

   // bool removed= false;
   QList<DeathFlowFieldAnimation*>::Iterator it;
   for (it = mFlowAnimations.begin(); it != mFlowAnimations.end(); )
   {
      DeathFlowFieldAnimation* flow= *it;
      if ( flow->isElapsed() )
      {
         it = mFlowAnimations.erase(it);
         delete flow;
         // removed= true;
      }
      else
      {
         activeDevice->setParameter(mFlowCenter, flow->getCenter());
         activeDevice->setParameter(mFlowFieldScale, flow->getScale());
         activeDevice->setParameter(mFlowTimeDelta, delta);

         flow->update(delta);
         it++;
      }
   }

   glDepthMask(true);
   glEnable(GL_DEPTH_TEST);

   glActiveTexture(GL_TEXTURE2_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0_ARB);

   activeDevice->setShader( 0 );

   /*
   if (removed)
      qDebug("PlayerDeathEffect::animate: active flowfields: %d", mFlowAnimations.size());
   */

   FrameBuffer::pop();
}

void PlayerDeathEffect::render()
{
   // player's modelview-transformation is already applied to the particle position
   glPushMatrix();
   glLoadIdentity();

   // init blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);
   glEnable(GL_ALPHA_TEST);

   activeDevice->setShader( mPointsShader );
   glEnable(GL_POINT_SPRITE);
   glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

   // bind point sprite texture
   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, mParticleTextureId);
   activeDevice->bindSampler(mPointsTexture, 0);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   activeDevice->bindSampler(mPointsColorMap, 1);

   float sizeFactor= 1.0f;
   FrameBuffer* fb= FrameBuffer::Instance();
   if (fb)
      sizeFactor= fb->width() * 1.25f / 1920.0f;

   foreach (DeathFlowFieldAnimation* flow, mFlowAnimations)
   {
      glBindTexture(GL_TEXTURE_2D, flow->getColorMap());
      activeDevice->setParameter(mPointsSize, flow->getPointSize() * sizeFactor);

      flow->draw();
   }

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0_ARB);

   glDisable(GL_POINT_SPRITE);

   glColor4f(1,1,1,1);

   // enable blending
   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   glDisable(GL_ALPHA_TEST);

   activeDevice->setShader( 0 );

   glPopMatrix();
}


