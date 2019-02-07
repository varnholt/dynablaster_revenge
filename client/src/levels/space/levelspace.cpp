// game
#include "levelspace.h"

// level/space
#include "earthbackground.h"
#include "starfieldbackground.h"

// effects/spherefragments
#include "effects/spherefragments/duplicatealpha.h"
#include "effects/spherefragments/blendquad.h"

// engine/nodes
#include "nodes/scenegraph.h"
#include "nodes/camera.h"
#include "nodes/mesh.h"
#include "nodes/dummy.h"
#include "nodes/scenegraph.h"

// engine/materials
#include "materials/texturematerial.h"
#include "materials/environmentmaterial.h"
#include "materials/environmenttexturematerial.h"

#include "materials/blockmaterial.h"
#include "materials/environmenttexturematerial.h"
#include "materials/bombexplosionshader.h"
#include "materials/extramapping.h"
#include "materials/invisibilitymaterial.h"
#include "materials/shadowbillboard.h"
#include "materials/destructionmaterial.h"
#include "materials/playermaterial.h"
#include "materials/skullmaterial.h"

// engine/render
#include "render/texturepool.h"

// framework
#include "framework/gldevice.h"
#include "framework/framebuffer.h"
#include "maindrawable.h"

// effects/lensflare
#include "effects/lensflare/lensflarefactory.h"
#include "postproduction/blurfilter.h"
#include "tools/filestream.h"

// cmath
#include <math.h>

//-----------------------------------------------------------------------------
/*!
*/
LevelSpace::LevelSpace()
   : Level( Level::LevelSpace ),
     mEarth(0),
     mEarthRotation(0.0f),
     mStarField(0)
{
}

//-----------------------------------------------------------------------------
/*!
*/
LevelSpace::~LevelSpace()
{
   delete mEarth;
   delete mStarField;
   mEarth = 0;
   mStarField = 0;
}


//-----------------------------------------------------------------------------
/*!
*/
void LevelSpace::initialize()
{
   // activate castle lens flare
   LensFlareFactory::getInstance()->activate("space");
}



//-----------------------------------------------------------------------------
/*!
*/
void LevelSpace::drawBackground()
{
   activeDevice->clear();

   float aspect = 9.0f/16.0f;
   float scale= 0.05f;

   glDepthMask(GL_FALSE);
   glDisable(GL_DEPTH_TEST);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   mStarField->draw();


   glFrustum(
      -scale,  scale,
      -aspect*scale, aspect*scale,
       0.1f, 10.0f
   );
   glTranslatef(-mCamera.x, -mCamera.y, -mCamera.z);

   glMatrixMode(GL_MODELVIEW);
   glRotatef(mEarthRotation, 0,1,0);
   glRotatef(20.0f, 1,0,0);

   mEarth->draw( mCamera );

   glDepthMask(GL_TRUE);
   glEnable(GL_DEPTH_TEST);

   glLoadIdentity();
}


//-----------------------------------------------------------------------------
/*!
*/
void LevelSpace::animate(float dt)
{
   mEarthRotation += dt*0.01f;
}


//-----------------------------------------------------------------------------
/*!
*/
void LevelSpace::reset()
{
}


//-----------------------------------------------------------------------------
/*!
*/
Material* LevelSpace::createMaterial(SceneGraph* scene, int id) const
{
   Material* mat= 0;
   switch(id)
   {
      case (MAP_DIFFUSE):
         mat= new TextureMaterial(scene);
         break;
      case (MAP_REFLECT):
         mat= new EnvironmentMaterial(scene);
         break;
      case (MAP_DIFFUSE | MAP_REFLECT):
         mat= new EnvironmentTextureMaterial(scene);
         break;
      default:
         mat= 0;
         break;
   }
   return mat;
}


//-----------------------------------------------------------------------------
/*!
*/
void LevelSpace::loadData()
{
   Camera *shadowCam = 0;

   if (!isAborted())
   {
      mEarth = new EarthBackground();
      mStarField = new StarFieldBackground();

      mCamera= Vector(-0.15f, 0.45f, 2.0f);
   }

   if (!isAborted())
   {
      mLevel = new SceneGraph();
      mScene = new SceneGraph();
      mPlayers = new SceneGraph();

      mLevel->load("level.hjb", this);

      Camera* center= (Camera*)mLevel->getNode("Camera001");
      Camera* upper= (Camera*)mLevel->getNode("Upper");
      Camera* lower= (Camera*)mLevel->getNode("Lower");
      mCamInterp= new CameraInterpolation(center, upper, lower);
   }

   if (!isAborted())
   {
      shadowCam= (Camera*)mLevel->getNode("Shadow Cam");
      shadowCam->setPerspectiveMode(false);

/*
      cam= (Camera*)mLevel->getCamera();
      cam->setNear(5.0f);
      cam->setFar(200.0f);
*/
   }

   if (!isAborted())
   {
      mScene->load("block.hjb");
      mScene->load("bomb.hjb");
      mScene->load("extra.hjb");
      mScene->load("skull.hjb");
   }

   if (!isAborted())
   {
      mDestruction= new DestructionMaterial(
         mScene,
         "stone-unwrap",
         "diffuse_level",
         "specular_level",
         "shadow-cookie",
         shadowCam
      );

      const char* destructions[4]= {
         "block-destruct0.hjb",
         "block-destruct1.hjb",
         "block-destruct2.hjb",
         "block-destruct.hjb"
      };

      for (int i = 0; i < 4; i++)
      {
         SceneGraph *scene= new SceneGraph();
         if (scene->load( destructions[i] ))
         {
            // precalc tracks
            for (int i=0; i<scene->getChildCount(); i++)
            {
               Node *node= scene->getChild(i);
               node->bakeAnimationTrack(160.0f);
            }
         }
         else
         {
            delete scene;
            scene= 0;
         }

         if (scene)
         {
            // create for copies, each 90 degrees rotated
            Dummy *node= new Dummy(0);
            for (int rot=0; rot<4; rot++)
            {
               float angle= (rot-1) * M_PI * 0.5f;
               //printf("angle: %f \n", angle);
               Matrix tm= Matrix::rotateZ( angle );

               Dummy* dummy= new Dummy(node);

               for (int i=0; i<scene->getChildCount(); i++)
               {
                  Node* child= scene->getChild(i);

                  if (child->id() == Node::idMesh)
                  {
                     Mesh *ref= (Mesh*)child;
                     Mesh *mesh= new Mesh(dummy);
                     mesh->copy(*ref);
                     mesh->transform(0.0f);
                     mesh->createBoxMapping(
                        true,
                        Vector(-0.5f,-0.5f,0.0f),
                        Vector(0.5f,0.5f,1.0f),
                        tm
                     );
                     mesh->setFrame(0.0f);
                     mDestruction->addMesh(mesh);
                  }
               }

            }
            mDestructAnim.add( node );
         }
      }
   }

   if (!isAborted())
   {
      mShadowBillboards= new ShadowBillboard(mScene, "shadowdrop-3x3");
      mShadowBlocks= new ShadowBillboard(mScene, "shadow_block");

      mBombs= new EnvironmentTextureMaterial(
         mScene,
         "bomb",
         "diffuse_level",
         "diffuse_bomb"
      );

      mStones= new BlockMaterial(
         mScene,
         "stone-unwrap",
         "diffuse_level",
         "specular_level",
         "shadow-cookie",
         shadowCam
      );

      mBlocks= new BlockMaterial(
         mScene,
         "block-unwrap",
         "diffuse_level",
         "specular_level",
         "shadow-cookie",
         shadowCam
      );

      mSkulls= new SkullMaterial(
         mScene,
         "skull",
         "diffuse_level",
         "specular_level",
         "",
         shadowCam
      );

      Material* material = 0;
      for (int i = 0; i < MAX_PLAYERS; i++)
      {
         QString filename= QString("player_%1").arg(i+1);
         material= new PlayerMaterial(
               mPlayers,
               qPrintable(filename),
               "diffuse_level",
               "specular_level",
               "player-ao"
         );
         Q_UNUSED(material);
      }

      material= new InvisibilityMaterial( mPlayers );

      mExtraFlame   = new ExtraMapping(mScene, "extra_flame");
      mExtraBomb    = new ExtraMapping(mScene, "extra_bomb");
      mExtraSpeedup = new ExtraMapping(mScene, "extra_speedup");
      mExtraKick    = new ExtraMapping(mScene, "extra_kick");
      mExtraSkull   = new ExtraMapping(mScene, "extra_skull");
   }

   TexturePool::Instance()->update();


   if (!isAborted())
   {
      mLevel->setCamera(0);
      mScene->setCamera(0);
      mPlayers->setCamera(0);

//      mLevel->prepare();
//      mScene->prepare();
   }
}
