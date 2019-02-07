#include "playerinfectedeffect.h"
#include "infectedflowfieldanimation.h"
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

PlayerInfectedEffect::PlayerInfectedEffect()
   : mParticleTextureId(0)
   , mPointsShader(0)
   , mPointsTexture(0)
   , mDelta(0.0f)
{
   Image image;
   image.load("data/game/flowfield_pointsprite");

   mParticleTextureId = activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight());

   image.load("data/game/flowfield");
   mFlowFieldTextureId = activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight(), 1);

   // draws a set of particle positions/colors as point sprites
   mPointsShader= activeDevice->loadShader(
      "infectedparticles-vert.glsl",
      "infectedparticles-frag.glsl" );
   mPointsTexture= activeDevice->getParameterIndex("texturemap");
   mPointsSize= activeDevice->getParameterIndex("particleSize");

   // moves particle positions along flowfield and creates new random position when faded out
   mFlowUpdatePosShader= activeDevice->loadShader(
      "infectedflowfield-vert.glsl",
      "infectedflowfield-frag.glsl" );
   mFlowDepthTexture= activeDevice->getParameterIndex("depthTexture");
   mFlowVertexPosTexture= activeDevice->getParameterIndex("vertexPosTexture");
   mFlowVertexParamTexture= activeDevice->getParameterIndex("vertexParamTexture");
   mFlowFieldTexture= activeDevice->getParameterIndex("flowfieldTexture");
   mFlowCenter= activeDevice->getParameterIndex("center");
   mFlowFieldScale= activeDevice->getParameterIndex("fieldScale");
   mFlowTimeDelta= activeDevice->getParameterIndex("timeDelta");
   mFlowSrcRect= activeDevice->getParameterIndex("srcRect");
   mFlowInvProj= activeDevice->getParameterIndex("invProj");
   mFlowStop= activeDevice->getParameterIndex("stop");

   // init particle positions to random point on player surface
   mFlowInitPosShader= activeDevice->loadShader(
      "infectedinitpositions-vert.glsl",
      "infectedinitpositions-frag.glsl" );
   mFlowInitPosDepth= activeDevice->getParameterIndex("depthmap");
   mFlowInitPosInvProj= activeDevice->getParameterIndex("invProj");
   mFlowInitSrcRect= activeDevice->getParameterIndex("srcRect");

   // init particle parameters to random values
   mFlowInitParamShader= activeDevice->loadShader(
      "infectedinitparams-vert.glsl",
      "infectedinitparams-frag.glsl" );
   mFlowInitParamDepth= activeDevice->getParameterIndex("depthmap");
   mFlowInitParamInvProj= activeDevice->getParameterIndex("invProj");
   mFlowInitParamCenter= activeDevice->getParameterIndex("center");

   // update colors of timed-out particles
   mFlowUpdateColShader= activeDevice->loadShader(
      "infectedupdatecolors-vert.glsl",
      "infectedupdatecolors-frag.glsl" );
   mFlowUpdateColColorMap= activeDevice->getParameterIndex("colormap");
   mFlowUpdateColPositionMap= activeDevice->getParameterIndex("positionmap");
   mFlowUpdateColProj= activeDevice->getParameterIndex("proj");
   mFlowUpdateColStop= activeDevice->getParameterIndex("stop");

}

PlayerInfectedEffect::~PlayerInfectedEffect()
{
   clear();
}


void PlayerInfectedEffect::clear()
{
   QMap<InfectedFlowFieldAnimation*, Material*>::Iterator it;
   it= mFlowAnimations.begin();
   while (it != mFlowAnimations.end())
   {
      InfectedFlowFieldAnimation* flow= it.key();
      it= mFlowAnimations.erase(it);
      delete flow;
   }
}


void PlayerInfectedEffect::add( Material* material )
{
   if (!material)
      return;

   InfectedFlowFieldAnimation* animation = new InfectedFlowFieldAnimation();
   mFlowAnimations.insert(animation, material);
}

void PlayerInfectedEffect::remove( Material* playerMaterial )
{
   QMap<InfectedFlowFieldAnimation*, Material*>::Iterator it;
   for (it= mFlowAnimations.begin(); it!= mFlowAnimations.end(); it++)
   {
      Material* mat= it.value();
      InfectedFlowFieldAnimation* flow= it.key();
      if (mat == playerMaterial)
      {
         flow->stop();
         *it= 0;
      }
   }
}

void PlayerInfectedEffect::update(InfectedFlowFieldAnimation* flow, Material* material, float delta)
{
   // get 2d bounding rect from material
   Vector min2d(0.0f, 0.0f, 0.0f);
   Vector max2d(0.0f, 0.0f, 0.0f);

   FrameBuffer* deferred= MainDrawable::getInstance()->getRenderBuffer(1);
   FrameBuffer::push( deferred );
   activeDevice->clear();

   if (material)
   {
      material->getBoundingRect(min2d, max2d, mProj);
      // get pivot in world space
      Vector center= material->getGeometry(0)->getTransform().translation();
      flow->setCenter(center);

      // render player
      // TODO: clear relevant area only!
      material->renderDiffuse();
   }

   deferred->unbind();

   glDepthMask(false);
   glDisable(GL_DEPTH_TEST);

   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, deferred->depthTexture());

   if (!flow->initialized())
   {
      flow->initialize();
      flow->setInitialized( true );

      activeDevice->setShader( mFlowInitPosShader );
      activeDevice->bindSampler( mFlowInitPosDepth, 0 );
      activeDevice->setParameter( mFlowInitPosInvProj, mInvProj );
      activeDevice->setParameter( mFlowInitSrcRect, Vector4(min2d.x,min2d.y, max2d.x,max2d.y));
      flow->initializePositions( min2d, max2d );

      activeDevice->setShader( mFlowInitParamShader );
      activeDevice->bindSampler( mFlowInitParamDepth, 0 );
      activeDevice->setParameter( mFlowInitParamInvProj, mInvProj );
      activeDevice->setParameter( mFlowInitParamCenter, flow->getCenter() );
      flow->initializeParams( min2d, max2d );
   }

   activeDevice->setShader( 0 );


   // update positions
   activeDevice->setShader( mFlowUpdatePosShader );
   activeDevice->bindSampler( mFlowDepthTexture, 0 );

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   activeDevice->bindSampler( mFlowVertexPosTexture, 1 );

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   activeDevice->bindSampler( mFlowVertexParamTexture, 2 );

   glActiveTexture(GL_TEXTURE3_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mFlowFieldTextureId);
   activeDevice->bindSampler(mFlowFieldTexture, 3);

   activeDevice->setParameter(mFlowCenter, flow->getCenter());
   activeDevice->setParameter(mFlowFieldScale, flow->getScale());
   activeDevice->setParameter(mFlowTimeDelta, delta);
   activeDevice->setParameter(mFlowSrcRect, Vector4(min2d.x,min2d.y, max2d.x,max2d.y));
   activeDevice->setParameter(mFlowInvProj, mInvProj);
   activeDevice->setParameter(mFlowStop, flow->stopped()?1:0);
   flow->updatePositions(delta);

   glActiveTexture(GL_TEXTURE3_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE2_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0_ARB);



   // update colors
   glEnable(GL_ALPHA_TEST); // don't write color with alpha=0
   activeDevice->setShader( mFlowUpdateColShader );
   activeDevice->bindSampler( mFlowUpdateColColorMap, 0 );
   glBindTexture(GL_TEXTURE_2D, deferred->texture());

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   activeDevice->bindSampler( mFlowUpdateColPositionMap, 1 );

   activeDevice->setParameter(mFlowUpdateColProj, mProj);
   activeDevice->setParameter(mFlowUpdateColStop, flow->stopped()?1:0);
   flow->updateColors();

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0_ARB);
   glDisable(GL_ALPHA_TEST);


   // img->save(QString("image_%1.png").arg(mScreenshotCounter++));

   FrameBuffer::pop();

   glDepthMask(true);
   glEnable(GL_DEPTH_TEST);
}


// animate flow fields
void PlayerInfectedEffect::animate(float delta)
{
   mDelta= delta;

   // erase elapsed entries
   QMap<InfectedFlowFieldAnimation*, Material*>::Iterator it;
   for (it = mFlowAnimations.begin(); it != mFlowAnimations.end(); )
   {
      InfectedFlowFieldAnimation* flow= it.key();
//      Material* material= it.value();
      if ( flow->isElapsed() )
      {
         it = mFlowAnimations.erase(it);
         delete flow;
      }
      else
      {
         it++;
      }
   }


   /*
   if (removed)
      qDebug("PlayerInfectedEffect::animate: active flowfields: %d", mFlowAnimations.size());
   */

}

void PlayerInfectedEffect::render()
{
   FrameBuffer::push();

   glGetFloatv(GL_PROJECTION_MATRIX, mProj.data());
   mInvProj= mProj.invert4x4();

   QMap<InfectedFlowFieldAnimation*, Material*>::Iterator it;
   for (it = mFlowAnimations.begin(); it != mFlowAnimations.end(); it++)
   {
      InfectedFlowFieldAnimation* flow= it.key();
      Material* material= it.value();
      update(flow, material, mDelta);
   }

   FrameBuffer::pop();

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

   float sizeFactor= 1.0f;
   FrameBuffer* fb= FrameBuffer::Instance();
   if (fb)
      sizeFactor= fb->width() * 1.25f / 1920.0f;

   for (QMap<InfectedFlowFieldAnimation*, Material*>::ConstIterator it= mFlowAnimations.constBegin(); it!= mFlowAnimations.constEnd(); it++)
   {
      InfectedFlowFieldAnimation* flow= it.key();
      activeDevice->setParameter(mPointsSize, flow->getPointSize() * sizeFactor);

      flow->draw();
   }

   glActiveTexture(GL_TEXTURE0_ARB);
   glDisable(GL_POINT_SPRITE);

   // enable blending
   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   glDisable(GL_ALPHA_TEST);

   activeDevice->setShader( 0 );

   glPopMatrix();
}



