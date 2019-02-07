#include "stdafx.h"
#include "invisibilitymaterial.h"
#include "textureslot.h"
#include "nodes/mesh.h"
#include "gldevice.h"
#include "render/renderbuffer.h"
#include "render/uv.h"
#include "render/vertexbuffer.h"
#include "render/texturepool.h"
#include "image/image.h"
#include "tools/stream.h"
#include "tools/profiling.h"
#include "animation/motionmixer.h"
#include "framework/globaltime.h"


InvisibilityMaterial::InvisibilityMaterial(SceneGraph *scene)
: PlayerMaterialBase(scene, MAP_DIFFUSE | MAP_REFLECT)
, mShader(0)
, mTextureMap(0)
, mGradientMap(0)
, mParamTexture(0)
, mParamGradient(0)
, mParamFadeThreshold(0)
, mParamCamera(0)
, mParamBones(0)
{
   addTexture(mGradientMap, "invisble-mask");
}


InvisibilityMaterial::~InvisibilityMaterial()
{
}


void InvisibilityMaterial::init()
{
   mShader= activeDevice->loadShader("invisibility-vert.glsl", "invisibility-frag.glsl");

   mParamTexture= activeDevice->getParameterIndex("texturemap");
   mParamGradient= activeDevice->getParameterIndex("gradientmap");
   mParamFadeThreshold= activeDevice->getParameterIndex("fadeThreshold");

   mParamCamera= activeDevice->getParameterIndex("camera");
   mParamBones= activeDevice->getParameterIndex("bones");
}

void InvisibilityMaterial::load(Stream *stream)
{
   Material::load(stream);
}


void InvisibilityMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mTextureMap);
   activeDevice->bindSampler(mParamTexture, 0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, mGradientMap);
   activeDevice->bindSampler(mParamGradient, 1);

   activeDevice->setShader( mShader );

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);

   glClientActiveTexture(GL_TEXTURE2_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE1_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE0_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void InvisibilityMaterial::end()
{
   activeDevice->setShader(0);

   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_NORMAL_ARRAY);

   glClientActiveTexture(GL_TEXTURE2_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   glDisable(GL_BLEND);
}


void InvisibilityMaterial::renderDiffuse()
{
   begin();

/*
   Matrix projMat;
   glGetFloatv(GL_PROJECTION_MATRIX, projMat.data());
   projMat= projMat.invert();
   Vector camPos= projMat.translation();
*/

   Matrix bones[8];
   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      VertexBuffer *vb= mVB[i].vb;
      Geometry *geo= mVB[i].geo;

      if (geo->isVisible())
      {
         Mesh* mesh= (Mesh*)geo->getParent();
         float time= mesh->getRenderParameter(1);
         activeDevice->setParameter(mParamFadeThreshold, time);

         MotionMixer *mixer= mesh->getMotionMixer();

         Matrix invView= (geo->getTransform() * mCamera).invert();
         Vector osCam= invView.translation();

//         Vector osCam= geo->getParent()->getWorld2Obj() * camPos;
         activeDevice->setParameter(mParamCamera, osCam);

         activeDevice->push(geo->getTransform());

         // draw mesh
         glBindBuffer( GL_ARRAY_BUFFER_ARB, vb->getVertexBuffer() );
         glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), (GLvoid*)0 );
         glNormalPointer(GL_FLOAT, sizeof(Vertex), (GLvoid*)sizeof(Vector) );

         glClientActiveTexture(GL_TEXTURE0_ARB);
         glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector)*2) );

         glClientActiveTexture(GL_TEXTURE1_ARB);
         glTexCoordPointer( 4, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector)*2+2*4) );

         glClientActiveTexture(GL_TEXTURE2_ARB);
         glTexCoordPointer( 4, GL_FLOAT, sizeof(Vertex), (GLvoid*)(sizeof(Vector)*2+6*4) );

         glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, vb->getIndexBuffer() );


         // render all clusters from the same vertex buffer
         unsigned short* offset= 0;
         int end= 0;
         for (int c=0; c<mClusters.size(); c++)
         {
            Cluster* cluster= mClusters[c];

            for (int i=0; i<cluster->mBones.size(); i++)
            {
               int idx= cluster->mBones[i];
               Node *node= mixer->getNode( idx );
               bones[i]= node->getTransform();
            }
            for (int i=cluster->mBones.size(); i<8; i++)
               bones[i]= Matrix();
            glUniformMatrix4fv(mParamBones, 8, false, (float*)bones);

            int start= end;
            end += cluster->mVertices.size();
            glDrawRangeElements( GL_TRIANGLES, start, end, cluster->mIndices.size(), GL_UNSIGNED_SHORT, (void*)offset ); // render
            offset += cluster->mIndices.size();
         }


         activeDevice->pop();
      }
   }

   end();

/*
   if (mVB.size()>0)
      printf("players: %f \n", (t2-t1)/1000000.0);
*/
}


void InvisibilityMaterial::setTexture(unsigned int textureMap)
{
   mTextureMap = textureMap;
}

