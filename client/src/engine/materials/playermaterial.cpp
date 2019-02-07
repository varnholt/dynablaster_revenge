#include "stdafx.h"
#include "playermaterial.h"
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


PlayerMaterial::PlayerMaterial(SceneGraph *scene)
: PlayerMaterialBase(scene, MAP_DIFFUSE | MAP_REFLECT)
, mColorMap(0)
, mDiffuseMap(0)
, mSpecularMap(0)
, mShader(0)
, mParamSpecular(0)
, mParamDiffuse(0)
, mParamTexture(0)
, mParamCamera(0)
, mParamBones(0)
, mParamFlash(0)
, mParamFade(0)
{
}


PlayerMaterial::PlayerMaterial(SceneGraph *scene, const char *colormap, const char *envmap, const char *specmap, const char *aomap)
: PlayerMaterialBase(scene, MAP_DIFFUSE | MAP_REFLECT)
{
   addTexture(mColorMap, colormap);
   addTexture(mDiffuseMap, envmap);
   addTexture(mSpecularMap, specmap, 1|2|4);
   addTexture(mAmbientMap, aomap);
}


PlayerMaterial::~PlayerMaterial()
{
}


void PlayerMaterial::init()
{
   mShader= activeDevice->loadShader("playermaterial-vert.glsl", "playermaterial-frag.glsl");

   mParamSpecular= activeDevice->getParameterIndex("specularmap");
   mParamDiffuse= activeDevice->getParameterIndex("diffusemap");
   mParamTexture= activeDevice->getParameterIndex("texturemap");
   mParamAmbient= activeDevice->getParameterIndex("ambientmap");
   mParamCamera= activeDevice->getParameterIndex("camera");
   mParamBones= activeDevice->getParameterIndex("bones");
   mParamFlash= activeDevice->getParameterIndex("flash");
   mParamFade= activeDevice->getParameterIndex("fade");
}

void PlayerMaterial::load(Stream *stream)
{
   Material::load(stream);

   addTexture(mColorMap, getTextureSlot(0)->name());
   addTexture(mDiffuseMap, "diffuse_level");
   addTexture(mSpecularMap, getTextureSlot(1)->name(), 1|2|4);
}


void PlayerMaterial::begin()
{
   Material::begin();

   // set material parameters
   //   activeDevice->setMaterial(mAmbient, mDiffuse, mSpecular, mShininess);
   glBindTexture(GL_TEXTURE_2D, mSpecularMap);

   glActiveTexture(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mDiffuseMap);

   glActiveTexture(GL_TEXTURE2_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mColorMap);

   glActiveTexture(GL_TEXTURE3_ARB);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mAmbientMap);

   activeDevice->setShader( mShader );
   activeDevice->bindSampler(mParamSpecular, 0);
   activeDevice->bindSampler(mParamDiffuse, 1);
   activeDevice->bindSampler(mParamTexture, 2);
   activeDevice->bindSampler(mParamAmbient, 3);

   // enable required vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY); // vertex data
   glEnableClientState(GL_NORMAL_ARRAY);

   glClientActiveTexture(GL_TEXTURE2_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE1_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE0_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void PlayerMaterial::end()
{
   glDisableClientState(GL_VERTEX_ARRAY); // vertex data
   glDisableClientState(GL_NORMAL_ARRAY);

   glClientActiveTexture(GL_TEXTURE2_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glClientActiveTexture(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glActiveTexture(GL_TEXTURE3);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE2);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE1);
   glDisable(GL_TEXTURE_2D);

   glActiveTexture(GL_TEXTURE0);

   activeDevice->setShader(0);
}


void PlayerMaterial::renderDiffuse()
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
         MotionMixer *mixer= mesh->getMotionMixer();

         float flash= mesh->getRenderParameter(0);
         float fade= mesh->getRenderParameter(2);

         Matrix invView= (geo->getTransform() * mCamera).invert();
         Vector osCam= invView.translation();

//         Vector osCam= geo->getParent()->getWorld2Obj() * camPos;
         activeDevice->setParameter(mParamFlash, flash);
         activeDevice->setParameter(mParamFade, fade);
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


void PlayerMaterial::setColorMap(const Texture& colorMap)
{
   mColorMap = colorMap;
}



void PlayerMaterial::exportOBJ(Stream* stream, int& indexOffset)
{
   for (int i=0;i<mVB.size();i++)
   {
      // get vertex buffer
      Geometry *geo= mVB[i].geo;

      if (!geo->isVisible())
         continue;

      Array<Vector> vtx= geo->getSkinVertices();

      exportGeo(
         stream,
         geo->getParent()->name(),
         geo->getTransform(),
         vtx.data(),
         geo->getNormals(),
         geo->getUV(1),
         geo->getVertexCount(),
         geo->getIndices(),
         geo->getIndexCount(),
         indexOffset
      );

      indexOffset+=geo->getVertexCount();
   }
}
