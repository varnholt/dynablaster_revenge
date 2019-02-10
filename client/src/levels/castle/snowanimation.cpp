// header
#include "snowanimation.h"

// snow
#include "snowparticle.h"

// shared
#include "constants.h"


// framework
#include "framework/gldevice.h"
#include "framework/globaltime.h"
#include "framework/framebuffer.h"
#include "tools/random.h"
#include "tools/profiling.h"
#include "render/texturepool.h"
#include <math.h>

// defines
#define OFFSET_X 0.2f
#define OFFSET_Y 0.2f


//-----------------------------------------------------------------------------
/*!
*/
SnowAnimation::SnowAnimation()
 : mShader(0),
   mVertices(0),
   mLimitUp(1.0f),
   mLimitDown(0.0f),
   mWindScalar(0.0f)
{
   mWindScalar = 0.1f;

   createPointsprites();
}


//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::initParticle(
   SnowParticle* sprite,
   float zPos
)
{
   //
   //     x-width
   //     * * *  *  *  * **   * * *   *   **  *
   //  -q 0                                   1 + q
   //
   float xPos = frands(-OFFSET_X, 1.0f + OFFSET_X);
   float yPos = frands(-OFFSET_Y, 1.0f + OFFSET_Y);

   // set origin
   sprite->mPosition.set(xPos, yPos, zPos);

   // normalize from 30.0f .. 50.0f (for a 1920x1080 screen)
   sprite->mSize = frands(30.0f, 50.0f);

   // normalize speed from 0.9f to 1.0f
   sprite->mSpeed = 0.8f + frand(0.2f);
}


void SnowAnimation::createPointsprites()
{
   SnowParticle* sprite = 0;
   for (int i = 0; i < SNOW_PARTICLES_COUNT; i++)
   {
      sprite = new SnowParticle();
      mSnowParticles[i] = sprite;
   }
}


void SnowAnimation::initPointsprites()
{
    SnowParticle* sprite = 0;
    for (int i = 0; i < SNOW_PARTICLES_COUNT; i++)
    {
       sprite = mSnowParticles[i];

       float startZ = frands(1.0f, 2.0f);
       initParticle(sprite, startZ);
    }
}


//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::initialize()
{
   // init
   mPointspriteTexture = TexturePool::Instance()->getTexture("game/snowflake");

   mShader= activeDevice->loadShader("snow-vert.glsl", "snow-frag.glsl");
   mParamTexture= activeDevice->getParameterIndex("texturemap");
   mParamSizeFactor= activeDevice->getParameterIndex("sizeFactor");

   // 1 vertex per quad
   int vertices= SNOW_PARTICLES_COUNT;

   // 4x float for vertex position (x,y,z) and size (w)
   mVertices=  activeDevice->createVertexBuffer(vertices * 4 * sizeof(float), true);

   // dynamic vertex buffer has (x,y,z,size) per vertex
   float *pos= (float*)activeDevice->lockVertexBuffer(mVertices);
   for (int i=0;i<vertices*4;i++) *pos++= 0.0f;
   activeDevice->unlockVertexBuffer(mVertices);

   initPointsprites();
}


//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::reset()
{
   mAnimationTime.restart();

   SnowParticle* snowParticle = 0;
   for (int i = 0; i < SNOW_PARTICLES_COUNT; i++)
   {
       snowParticle = mSnowParticles[i];
       snowParticle->mVisible = false;
   }
}



//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::updateSpritePosition(
   int elapsed,
   float dt,
   SnowParticle* snowParticle
)
{
   if (snowParticle->mPosition.z < mLimitDown)
   {
      float startZ = frands(1.0f, 2.0f);
      initParticle(snowParticle, startZ);

      if (elapsed > ((SERVER_PREPARATION_TIME + 1) * 1000) )
      {
         snowParticle->mVisible = true;
      }
   }

   snowParticle->mPosition += mWindDirection * snowParticle->mSpeed * (dt * 0.003f);
}


//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::animate(float dt)
{
    updateWind();

    SnowParticle* snowParticle = 0;

    double t1= getCpuTick();

    int elapsed= mAnimationTime.elapsed();

    for (int i = 0; i < SNOW_PARTICLES_COUNT; i++)
    {
       snowParticle = mSnowParticles[i];
       updateSpritePosition(elapsed, dt, snowParticle);
    }

    double t2= getCpuTick();

    t2= (t2-t1)/1000000.0;

//    printf("snow anim time: %f \n", t2);
}


//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::drawPointSprites()
{
    double t1= getCpuTick();

    int count= 0;
    volatile float *pos= (float*)activeDevice->lockVertexBuffer(mVertices);
    for (int i = 0; i < SNOW_PARTICLES_COUNT; i++)
    {
      SnowParticle* sprite = mSnowParticles[i];

      if (sprite->mVisible)
      {
         *pos++= sprite->mPosition.x;
         *pos++= sprite->mPosition.y;
         *pos++= sprite->mPosition.z;
         *pos++= sprite->mSize;
         count++;
       }
    }

    // clear rest of buffer
    for (int i = count; i < SNOW_PARTICLES_COUNT; i++)
    {
       *pos++= 0.0f;
       *pos++= 0.0f;
       *pos++= 0.0f;
       *pos++= 0.0f;
    }

    activeDevice->unlockVertexBuffer(mVertices);

    double t2= getCpuTick();

    t2= (t2-t1) / 1000000.0;
//    printf("snow transform time: %f \n", t2);

    // draw it

    // set up render states
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    float sizeFactor= 1.0f;
    FrameBuffer* fb= FrameBuffer::Instance();
    if (fb)
       sizeFactor= fb->getSizeFactor(1920.0f);

    activeDevice->setShader( mShader );
    activeDevice->setParameter(mParamSizeFactor, sizeFactor );

    // bind point sprite texture
    glBindTexture(GL_TEXTURE_2D, mPointspriteTexture);
    activeDevice->bindSampler(mParamTexture, 0);

    // bind vbos
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer( GL_ARRAY_BUFFER_ARB, mVertices );
    glVertexPointer( 4, GL_FLOAT, 4*sizeof(float), (GLvoid*)0 );

    glDrawArrays( GL_POINTS, 0, count );

    glDisableClientState(GL_VERTEX_ARRAY);


    // restore states
    glDisable(GL_POINT_SPRITE);
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);
}


//------------------------------------------A-----------------------------------
/*!
*/
void SnowAnimation::updateWind()
{
   int time = mAnimationTime.elapsed();

   float val = time * 0.0001f;

   mWindDirection = Vector(0.5f, 0.0f, -1.0f);
   mWindDirection.x += sin(val) * 0.004f;
   mWindDirection.y += cos(val) * 0.004f;

   // not used
   // mWindScalar = sin(val) * 0.1f;
}


//-----------------------------------------------------------------------------
/*!
*/
void SnowAnimation::draw()
{
   if (mShader == 0)
      initialize();
   // init blending
   drawPointSprites();
}



