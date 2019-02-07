// game
#include "levelmansion.h"

// engine/nodes
#include "nodes/scenegraph.h"
#include "nodes/camera.h"
#include "nodes/mesh.h"
#include "nodes/dummy.h"
#include "nodes/dummy.h"

// engine/materials
#include "materials/texturematerial.h"
#include "materials/displacementmaterial.h"
#include "materials/environmentmaterial.h"
#include "materials/environmenttexturematerial.h"
#include "materials/environmentambientdiffusematerial.h"

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
#include "gldevice.h"
#include "nodes/scenegraph.h"

// effects/lensflare
#include "effects/lensflare/lensflarefactory.h"

// cmath
#include <math.h>

//-----------------------------------------------------------------------------
/*!
*/
LevelMansion::LevelMansion()
   : Level(Level::LevelMansion)
{
}


//-----------------------------------------------------------------------------
/*!
*/
void LevelMansion::initialize()
{
   // activate castle lens flare
   LensFlareFactory::getInstance()->activate("mansion");
}



Material* LevelMansion::createMaterial(SceneGraph* scene, int id) const
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
      case (MAP_AMBIENT | MAP_DIFFUSE | MAP_REFLECT):
         mat= new EnvironmentAmbientDiffuseMaterial(scene);
         break;
      default:
         mat= 0;
         break;
   }
   return mat;
}


void LevelMansion::loadData()
{
   Camera *shadowCam = 0;
   Camera *cam = 0;

   if (!isAborted())
   {
      mLevel = new SceneGraph();
      mScene = new SceneGraph();
      mPlayers = new SceneGraph();

      mLevel->load("level.hjb", this);
   }

   if (!isAborted())
   {
      shadowCam= (Camera*)mLevel->getNode("Shadow Cam");
      shadowCam->setPerspectiveMode(false);

      cam= (Camera*)mLevel->getCamera();
      cam->setNear(5.0f);
      cam->setFar(200.0f);

      Camera* center= (Camera*)mLevel->getNode("Camera001");
      Camera* upper= (Camera*)mLevel->getNode("Upper");
      Camera* lower= (Camera*)mLevel->getNode("Lower");
      mCamInterp= new CameraInterpolation(center, upper, lower);
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
      ((ShadowBillboard*)mShadowBillboards)->setOffset(0.0f, 0.3f);
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
         "specular_level2",
         "shadow-cookie",
         shadowCam
      );

      mBlocks= new BlockMaterial(
         mScene,
         "block-unwrap",
         "diffuse_level",
         "specular_level2",
         "shadow-cookie",
         shadowCam
      );

      mSkulls= new SkullMaterial(
         mScene,
         "skull",
         "diffuse_level",
         "specular_level",
         "shadow-cookie",
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
