#include "playerinvincibleeffect.h"
#include "framework/maindrawable.h"
#include "framework/framebuffer.h"
#include "framework/gldevice.h"
#include "materials/material.h"
#include "nodes/node.h"
#include "nodes/mesh.h"
#include "render/geometry.h"
#include "render/texturepool.h"
#include "tools/random.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "playerinvincibleinstance.h"

#include <math.h>

PlayerInvincibleEffect::PlayerInvincibleEffect()
   : mDisplacementTexture()
   , mRadius(0.0f)
{
//   Image image;
//   image.load("data/game/displace");
//   mDisplacementTexture= activeDevice->createTexture(image.getData(), image.getWidth(), image.getHeight());

/*
   Image* image= new Image("data/game/displace");
   image->buildDeltaMap();
   image->save("displace.tga");
*/

   mDisplacementTexture= TexturePool::Instance()->getTexture("data/game/displace");

   setRadius( 10.0f );

   mBlurHShader= activeDevice->loadShader(
      "playerinvincibleblurh-vert.glsl",
      "playerinvincibleblurh-frag.glsl" );
   mBlurHTexture= activeDevice->getParameterIndex("texture");
   mBlurHTexelOffset= activeDevice->getParameterIndex("texelOffset");
   mBlurHRadius= activeDevice->getParameterIndex("radius");
   mBlurHKernel= activeDevice->getParameterIndex("kernel");

   mBlurVShader= activeDevice->loadShader(
      "playerinvincibleblurv-vert.glsl",
      "playerinvincibleblurv-frag.glsl" );
   mBlurVTexture= activeDevice->getParameterIndex("texture");
   mBlurVTexelOffset= activeDevice->getParameterIndex("texelOffset");
   mBlurVRadius= activeDevice->getParameterIndex("radius");
   mBlurVKernel= activeDevice->getParameterIndex("kernel");

   mDisplaceShader= activeDevice->loadShader(
      "playerinvincibledisplace-vert.glsl",
      "playerinvincibledisplace-frag.glsl" );
   mDisplaceTexture1= activeDevice->getParameterIndex("texture");
   mDisplaceTexture2= activeDevice->getParameterIndex("displace");
   mDisplaceTexelOffset= activeDevice->getParameterIndex("texelOffset");
   mDisplaceOffset= activeDevice->getParameterIndex("displaceOffset");
   mDisplaceSourceRect= activeDevice->getParameterIndex("sourceRect");
   mDisplaceFade= activeDevice->getParameterIndex("fade");
   mDisplayCenter= activeDevice->getParameterIndex("center");
}


PlayerInvincibleEffect::~PlayerInvincibleEffect()
{
   clear();
}

void PlayerInvincibleEffect::setRadius( float radius )
{
   if (radius > 30.0f)
      radius= 30.0f;

   mRadius= radius;
   int size= ceil(mRadius);
   const double scale= -4.0/(mRadius*mRadius);
   float sum= 0.0f;
   for (int i=0;i<=size;i++)
   {
      float f= pow(2.718281828459045, i*i*scale);
      sum += f;
      mKernel[i]= f;
   }
   for (int i=size+1; i<32; i++)
       mKernel[i]= 0.0f;

   float t= 1.1f / (sum*2.0f-1.0f);
   for (int i=0;i<=size;i++)
      mKernel[i] *= t;
}

void PlayerInvincibleEffect::clear()
{
   qDeleteAll(mPlayers);
   mPlayers.clear();
}


void PlayerInvincibleEffect::add( Material* material )
{
   Vector min;
   Vector max;

   if (!material)
      return;

   QMap<Material*, PlayerInvincibleInstance*>::ConstIterator it;
   it= mPlayers.constFind(material);
   if (it == mPlayers.constEnd())
   {
      PlayerInvincibleInstance* player= new PlayerInvincibleInstance();
      player->setMaterial( material );

      mPlayers.insert( material, player );
   }
   else
   {
       PlayerInvincibleInstance* player= it.value();
       player->setRemove(false);
   }
}


void PlayerInvincibleEffect::remove( Material* playerMaterial )
{
   QMap<Material*, PlayerInvincibleInstance*>::ConstIterator it;
   it= mPlayers.constFind( playerMaterial );
   if (it != mPlayers.constEnd())
   {
      PlayerInvincibleInstance* player= it.value();
      player->remove();
   }
}

void PlayerInvincibleEffect::setMaterialFade(PlayerInvincibleInstance* player, float fade)
{
   Material* mat= player->getMaterial();
   if (mat)
   {
      Geometry* geo= mat->getGeometry(0);
      if (geo)
      {
         Mesh* mesh= (Mesh*)geo->getParent();
         if (mesh)
         {
            mesh->setRenderParameter( 2, fade );
         }
      }
   }
}


// animate flow fields
void PlayerInvincibleEffect::animate(float dt)
{
   QMap<Material*, PlayerInvincibleInstance*>::Iterator it;
   for (it= mPlayers.begin(); it!= mPlayers.end(); )
   {
      PlayerInvincibleInstance* player= it.value();

      if (!player->update(dt))
      {
         setMaterialFade( player, 0.0f );
         it= mPlayers.erase(it);
         delete player;
      }
      else
      {
         it++;
      }
   }

}


// draws perspectively corrected solid texture-mapped polygon
void backProject( Vector* dst, const Vector& min2d, const Vector& max2d, const Matrix& objMat, const Matrix& projMat )
{
   struct Vertex
   {
      float x,y,z;
      float u,v,w;
   };

   int min_vtx;
   Vertex *v1, *v2;
   float  inv_height;

   Vector pos= objMat.translation();

   // reference triangle in world space
   Vector worldTri[3];
   worldTri[0]= Vector(-0.6f,-0.5f, 2.0f ) + pos;
   worldTri[1]= Vector(-0.6f,-0.5f, 0.0f ) + pos;
   worldTri[2]= Vector( 0.6f,-0.5f, 0.0f ) + pos;

   // reference triangle in projected camera space (2d)
   Vertex vtx[3];
   Matrix mat= projMat;
   for (int i=0; i<3; i++)
   {
      const Vector& v= worldTri[i];
      float x= mat.xx*v.x +  mat.xy*v.y +  mat.xz*v.z + mat.xw;
      float y= mat.yx*v.x +  mat.yy*v.y +  mat.yz*v.z + mat.yw;
//      float z= mat.zx*v.x +  mat.zy*v.y +  mat.zz*v.z + mat.zw;
      float w= mat.wx*v.x +  mat.wy*v.y +  mat.wz*v.z + mat.ww;
      float t= 1.0f / w;
      vtx[i].x= x * t;
      vtx[i].y= y * t;
      vtx[i].z= t;
      vtx[i].u= v.x*t;
      vtx[i].v= v.y*t;
      vtx[i].w= v.z*t;
   }

   // calc constant deltas
   float delta1=  vtx[1].y - vtx[2].y;
   float delta2=  vtx[0].y - vtx[2].y;
   float denom =  (vtx[0].x - vtx[2].x) * delta1 - (vtx[1].x - vtx[2].x) * delta2;

   denom =  1.0f / denom;
   delta1*= denom;
   delta2*= denom;

   float deltau=  (vtx[0].u - vtx[2].u)*delta1 - (vtx[1].u - vtx[2].u)*delta2;
   float deltav=  (vtx[0].v - vtx[2].v)*delta1 - (vtx[1].v - vtx[2].v)*delta2;
   float deltaw=  (vtx[0].w - vtx[2].w)*delta1 - (vtx[1].w - vtx[2].w)*delta2;
   float deltaz=  (vtx[0].z - vtx[2].z)*delta1 - (vtx[1].z - vtx[2].z)*delta2;

   // obersten und untersten eckpunkt aus Vertex-array suchen
   min_vtx = 0;
   for(int n=1; n<3; n++)
   {
     if (vtx[n].y < vtx[min_vtx].y) min_vtx = n;
   }

   // Walk forward trough the Vertex array
   v1= &vtx[min_vtx];
   min_vtx++;
   if (min_vtx > 2) min_vtx= 0;
   v2= &vtx[min_vtx];
   inv_height= 1.0f / (v2->y - v1->y);
   float left_dx = (v2->x - v1->x) * inv_height;
   float left_dz = (v2->z - v1->z) * inv_height;
   float left_du = (v2->u - v1->u) * inv_height;
   float left_dv = (v2->v - v1->v) * inv_height;
   float left_dw = (v2->w - v1->w) * inv_height;

   Vector rect2d[4];
   rect2d[0]= Vector(min2d.x, min2d.y);
   rect2d[1]= Vector(max2d.x, min2d.y);
   rect2d[2]= Vector(max2d.x, max2d.y);
   rect2d[3]= Vector(min2d.x, max2d.y);

   for (int i=0; i<4; i++)
   {
      float dx= rect2d[i].x - v1->x;
      float dy= rect2d[i].y - v1->y;

      float u= v1->u + dy * left_du - dy*left_dx*deltau + dx*deltau;
      float v= v1->v + dy * left_dv - dy*left_dx*deltav + dx*deltav;
      float w= v1->w + dy * left_dw - dy*left_dx*deltaw + dx*deltaw;
      float z= v1->z + dy * left_dz - dy*left_dx*deltaz + dx*deltaz;

      float t= 1.0f / z;

      dst[i].x= u * t;
      dst[i].y= v * t;
      dst[i].z= w * t;
   }
}


void PlayerInvincibleEffect::blurPlayers(FrameBuffer* temp, const Matrix& proj)
{
   FrameBuffer::push();

   foreach (PlayerInvincibleInstance* player, mPlayers)
   {
      Material* mat= player->getMaterial();
      if (mat)
      {
         Vector min2d(0.0f, 0.0f, 0.0f);
         Vector max2d(0.0f, 0.0f, 0.0f);

         mat->getBoundingRect(min2d, max2d, proj);
         // add blur radius
         float bx= mRadius * 2.0f / temp->width();
         float by= mRadius * 2.0f / temp->height();
         min2d.x -= bx;
         min2d.y -= by;
         max2d.x += bx;
         max2d.y += by;

         player->setRect(min2d, max2d);

         player->setCenter( mat->getCenter2d(proj) );

         min2d.x= (min2d.x+1.0f) * 0.5f;
         min2d.y= (min2d.y+1.0f) * 0.5f;
         max2d.x= (max2d.x+1.0f) * 0.5f;
         max2d.y= (max2d.y+1.0f) * 0.5f;

         temp->bind();
         activeDevice->clear();
         setMaterialFade(player, player->getFade());
         mat->renderDiffuse();
         temp->unbind();

         // horizontal blur
         // from temp framebuffer to destination texture
         player->bind(0);
         activeDevice->clear();
         activeDevice->setShader( mBlurHShader );
         glBindTexture(GL_TEXTURE_2D, temp->texture() );
         activeDevice->bindSampler( mBlurHTexture, 0 );
         activeDevice->setParameter( mBlurHTexelOffset, Vector2( 1.0f / temp->width()) );
         activeDevice->setParameter( mBlurHRadius, mRadius );
         activeDevice->setParameter( mBlurHKernel, mKernel, 32 );

         float x= (max2d.x - min2d.x) * 2.0f * temp->width() / player->width();
         float y= (max2d.y - min2d.y) * 2.0f * temp->height() / player->height();

         glBegin(GL_QUADS);
         glTexCoord2f(min2d.x, min2d.y); glVertex2f(-1,  -1);
         glTexCoord2f(max2d.x, min2d.y); glVertex2f(-1+x,-1);
         glTexCoord2f(max2d.x, max2d.y); glVertex2f(-1+x,-1+y);
         glTexCoord2f(min2d.x, max2d.y); glVertex2f(-1,  -1+y);
         glEnd();

         player->unbind();


         // vertical blur
         // from texture0 to texture1
         player->bind(1);
         activeDevice->clear();
         activeDevice->setShader( mBlurVShader );
         glBindTexture(GL_TEXTURE_2D, player->texture(0) );
         activeDevice->setParameter( mBlurVTexelOffset, Vector2( 0.0f, 1.0f / player->height()) );
         activeDevice->setParameter( mBlurVRadius, mRadius );
         activeDevice->setParameter( mBlurVKernel, mKernel, 32 );

         glBegin(GL_QUADS);
         glTexCoord2f(0, 0); glVertex2f(-1,   -1);
         glTexCoord2f(x*0.5, 0); glVertex2f(-1+x, -1);
         glTexCoord2f(x*0.5, y*0.5); glVertex2f(-1+x, -1+y);
         glTexCoord2f(0, y*0.5); glVertex2f(-1,   -1+y);
         glEnd();

         player->unbind();

         activeDevice->setShader( 0 );
      }
   }

   FrameBuffer::pop();
}


void PlayerInvincibleEffect::render()
{
   Matrix proj;
   glGetFloatv(GL_PROJECTION_MATRIX, proj.data());
//   Matrix invProj= proj.invert4x4();

   float time= GlobalTime::Instance()->getTime() * 0.1f;

   FrameBuffer* temp= MainDrawable::getInstance()->getRenderBuffer(1);

/*
   // simulate fade in/out
   float fade= sin(time*10.0)+0.5f;
   if (fade<0.0f) fade=0.0f;
   if (fade>1.0f) fade=1.0f;
   foreach (PlayerInvincibleInstance* player, mPlayers)
   {
      player->setFade( fade );
   }
*/

   blurPlayers(temp, proj);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

//   glDisable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   glEnable(GL_ALPHA_TEST);
   glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   activeDevice->setShader( mDisplaceShader );

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture( GL_TEXTURE_2D, mDisplacementTexture );
   activeDevice->bindSampler(mDisplaceTexture2, 1);

   glActiveTexture(GL_TEXTURE0_ARB);
   activeDevice->bindSampler(mDisplaceTexture1, 0);

   foreach (PlayerInvincibleInstance* player, mPlayers)
   {
      Material* mat= player->getMaterial();
      if (mat)
      {
         Geometry* geo= mat->getGeometry(0);
         if (geo)
         {
            const Matrix& tm= geo->getTransform();

            Vector min2d= player->min2d();
            Vector max2d= player->max2d();

            activeDevice->setParameter(
               mDisplaceTexelOffset,
               Vector2(
                     temp->width() / 1920.0f,
                     temp->height() / 1080.0f
               )
            );

            activeDevice->setParameter(
               mDisplaceSourceRect,
               Vector4(
                   min2d.x,
                   min2d.y,
                   0.5f * temp->width() / player->width(),
                   0.5f * temp->height() / player->height()
               )
            );

            activeDevice->setParameter(
                     mDisplaceOffset,
                     Vector4(
                        sin(time*2.69*0.025)*0.5       * 1920.0 / temp->width(),
                       -(time*0.5+sin(time*2.81*0.025)*0.5)  * 1920.0 / temp->width(),
                       -cos(time*3.11*0.025)*0.5       * 1920.0 / temp->width(),
                       -(time*0.5-cos(time*3.59*0.025)*0.5)  * 1920.0 / temp->width()
                    )
            );

            activeDevice->setParameter(
                     mDisplaceFade,
                     player->getFade()
            );

            activeDevice->setParameter(
                     mDisplayCenter,
                     player->getCenter()
            );

            // project bounding rect to 3d world space
            Vector pos[4];
            backProject(pos, min2d, max2d, tm, proj);

            glBindTexture( GL_TEXTURE_2D, player->texture(1) );

            glBegin(GL_QUADS);
            glVertex3f(pos[0].x, pos[0].y, pos[0].z);
            glVertex3f(pos[1].x, pos[1].y, pos[1].z);
            glVertex3f(pos[2].x, pos[2].y, pos[2].z);
            glVertex3f(pos[3].x, pos[3].y, pos[3].z);
            glEnd();
         }
      }
   }

   glActiveTexture(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0_ARB);
   glBindTexture(GL_TEXTURE_2D, 0);

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_TRUE);
   glDisable(GL_ALPHA_TEST);
//   glEnable(GL_DEPTH_TEST);

   activeDevice->setShader( 0 );

}




/*
   // draw bounding rects
   glColor4f(1,0,0,1);
   foreach (PlayerInvincibleInstance* player, mPlayers)
   {
      Material* mat= player->getMaterial();
      Geometry* geo= mat->getGeometry(0);
      const Matrix& tm= geo->getTransform();

      Vector min2d= player->min2d();
      Vector max2d= player->max2d();

      // project bounding rect to 3d world space
      Vector pos[4];
      backProject(pos, min2d, max2d, tm, proj);

      glBegin(GL_LINE_LOOP);
      glVertex3f(pos[0].x, pos[0].y, pos[0].z);
      glVertex3f(pos[1].x, pos[1].y, pos[1].z);
      glVertex3f(pos[2].x, pos[2].y, pos[2].z);
      glVertex3f(pos[3].x, pos[3].y, pos[3].z);
      glEnd();
   }


   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   glColor4f(0,1,0,1);
   foreach (PlayerInvincibleInstance* player, mPlayers)
   {
      Vector min2d= player->min2d();
      Vector max2d= player->max2d();

      glBegin(GL_LINE_LOOP);
      glVertex2f(min2d.x, min2d.y);
      glVertex2f(max2d.x, min2d.y);
      glVertex2f(max2d.x, max2d.y);
      glVertex2f(min2d.x, max2d.y);
      glEnd();
   }
   glColor4f(1,1,1,1);

   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
*/


