// header
#include "fuseparticle.h"

// gl
#include "gldevice.h"

// Qt
#include <QImage>

#define POSITION_OFFSET_X 0.36f
#define POSITION_OFFSET_Y 0.15f
#define POSITION_OFFSET_Z 0.48f


// static variables
Array<FuseParticle*> FuseParticle::sParticles;
Array<FuseParticle*> FuseParticle::sDeletedParticles;
int FuseParticle::sTexture = 0;
Vector FuseParticle::sBombOffset;

//-----------------------------------------------------------------------------
/*!
*/
FuseParticle::FuseParticle()
 : mPointSize(1.0f),
   mScalar(0.0f),
   mDeleted(false),
   mElapsed(0.0f),
   mRandomStartTime(0.0f),
   mStarted(false)
{
   sParticles.add(this);
}


//-----------------------------------------------------------------------------
/*!
*/
FuseParticle::~FuseParticle()
{
}


//-----------------------------------------------------------------------------
/*!
   \param origin new fuse origin
*/
void FuseParticle::reset(const Vector& origin)
{         
   mOrigin = origin;
   mPosition = origin;
   mNextOrigin = origin;
   mPointSize = 1.0f;
   mScalar = 0.0f;

   float originRandX = 0.0f;
   float originRandY= 0.0f;
   float originRandZ= 0.0f;

   float dirRandX = 0.0f;
   float dirRandY= 0.0f;
   float dirRandZ= 0.0f;

   originRandX = (qrand() % 100) * 0.001f;
   originRandY = (qrand() % 100) * 0.001f;
   originRandZ = (qrand() % 100) * 0.001f;

   dirRandX = (-0.75f + (qrand() % 100) * 0.02f) * 0.3f;
   dirRandY = (-0.75f + (qrand() % 100) * 0.02f) * 0.3f;
   dirRandZ = (-0.75f + (qrand() % 100) * 0.015f) * 0.3f;

   mOrigin.x += originRandX;
   mOrigin.y += originRandY;
   mOrigin.z += originRandZ;

   mDirection =
      Vector(
         dirRandX,
         dirRandY,
         dirRandZ
      );

   mElapsed = 0.0f;
   mRandomStartTime = (qrand() % 100);
   mStarted = false;
}


//-----------------------------------------------------------------------------
/*!
   \param dt delta time
*/
void FuseParticle::animate(float dt)
{
   FuseParticle* particle = 0;

   for (int i = 0; i < sParticles.size(); i++)
   {
      particle = sParticles[i];

      particle->mElapsed += dt;

      // if the bomb is kicked away, let all particles burn down quickly
      float dist = (particle->getOrigin() - particle->getNextOrigin()).length();
      if (dist > 1.0f)
      {
         particle->mRandomStartTime = 0.0f;
      }

      if (
            particle->mElapsed > particle->mRandomStartTime
         || particle->isDeleted()
      )
      {
         particle->mStarted = true;

         particle->mDirection.y -= dt * 0.02f;
         particle->mScalar      += dt * 0.02f;
         particle->mPointSize   -= dt * 0.025f;

         particle->mPosition =
            particle->mOrigin + (particle->mDirection * particle->mScalar);

         // reset spark if intensity threshold exceeded
         if (particle->mPointSize < 0.01)
         {
            particle->reset(particle->getNextOrigin());

            // particle shall not respawn
            if (particle->isDeleted())
            {
               sDeletedParticles.add(particle);
            }
         }
      }
   }

   // delete obsolete particles
   for (int i = 0; i < sDeletedParticles.size(); i++)
   {
      particle = sDeletedParticles[i];
      sParticles.remove(particle);
      delete particle;
   }

   sDeletedParticles.clear();
}


//-----------------------------------------------------------------------------
/*!
*/
void FuseParticle::draw()
{
   // init blending
   glDepthMask(GL_FALSE);   // do not write to z buffer
   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE);

   // enable point sprite texture
   glEnable(GL_TEXTURE_2D);

   // bind point sprite texture
   glBindTexture(GL_TEXTURE_2D, sTexture);

   float pointSize = 0.0f;
   Vector pos;

   glBegin(GL_QUADS);

   FuseParticle* particle = 0;
   for (int i = 0; i < sParticles.size(); i++)
   {
      particle = sParticles[i];

      if (particle->mStarted)
      {
         pointSize = sParticles[i]->mPointSize;
         pos = sParticles[i]->mPosition;

         pointSize *= 0.05f;

         // qDebug("particle at: %f, %f, %f", pos.x, pos.y, pos.z);

         glColor4f(1,1,1,1);

         glTexCoord2f(0.0f, 0.0f);
         glVertex3f(
            pos.x - pointSize,
            pos.y - pointSize,
            pos.z
         );

         glTexCoord2f(1.0f, 0.0f);
         glVertex3f(
            pos.x + pointSize,
            pos.y - pointSize,
            pos.z
         );

         glTexCoord2f(1.0f, 1.0f);
         glVertex3f(
            pos.x + pointSize,
            pos.y + pointSize,
            pos.z
         );

         glTexCoord2f(0.0f, 1.0f);
         glVertex3f(
            pos.x - pointSize,
            pos.y + pointSize,
            pos.z
         );
      }
   }

   glEnd();

   // reset
   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//-----------------------------------------------------------------------------
/*!
*/
void FuseParticle::initialize()
{
   QImage image = QImage("data/logo/pointsprite.png");

   if (!image.isNull())
   {
      // load layers to texture
      sTexture =
         ((QGLContext*)QGLContext::currentContext())->bindTexture(
            image,
            GL_TEXTURE_2D,
            GL_RGBA,
            QGLContext::LinearFilteringBindOption
         );
   }
   else
   {
      qWarning("FuseParticle::initialize(): particle texture missing");
   }

   sBombOffset = Vector(
      POSITION_OFFSET_X,
      POSITION_OFFSET_Y,
      POSITION_OFFSET_Z
   );
}


//-----------------------------------------------------------------------------
/*!
   \return bomb offset
*/
const Vector &FuseParticle::getBombOffset()
{
   return sBombOffset;
}


//-----------------------------------------------------------------------------
/*!
*/
void FuseParticle::clear()
{
   for (int i = 0; i < sParticles.size(); i++)
      delete sParticles[i];

   sParticles.clear();
}


//-----------------------------------------------------------------------------
/*!
   \param deleted deleted flag
*/
void FuseParticle::setDeleted(bool deleted)
{
   mDeleted = deleted;
}


//-----------------------------------------------------------------------------
/*!
   \return \c true if particle is deleted
*/
bool FuseParticle::isDeleted() const
{
   return mDeleted;
}


//-----------------------------------------------------------------------------
/*!
   \param origin origin
*/
void FuseParticle::setOrigin(const Vector &origin)
{
   mOrigin = origin;
}


//-----------------------------------------------------------------------------
/*!
   \return origin
*/
const Vector &FuseParticle::getOrigin() const
{
   return mOrigin;
}


//-----------------------------------------------------------------------------
/*!
   \param origin next origin
*/
void FuseParticle::setNextOrigin(const Vector &origin)
{
   mNextOrigin = origin;
}


//-----------------------------------------------------------------------------
/*!
   \return next origin
*/
const Vector &FuseParticle::getNextOrigin() const
{
   return mNextOrigin;
}

