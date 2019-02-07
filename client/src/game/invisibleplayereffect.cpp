#include "invisibleplayereffect.h"
#include "playeritem.h"
#include "nodes/scenegraph.h"
#include "nodes/mesh.h"
#include "materials/material.h"
#include "framework/globaltime.h"

InvisiblePlayerEffect::InvisiblePlayerEffect()
 : mScene( 0 )
{
}


void InvisiblePlayerEffect::setPlayerScene( SceneGraph* players )
{
   mScene= players;
}


void InvisiblePlayerEffect::update(float globalTime)
{
   QMap<PlayerItem*, float>::Iterator it= mPlayers.begin();
   while ( it!= mPlayers.end() )
   {
      float startTime= it.value();
      Mesh* mesh= it.key()->getMesh();

      float param= 0.0f;
      float time= globalTime - startTime;

      if (time < 20.0f)
      {
         if (time < 3.0f)
            param= time - 1.0f;
         else if (time > 17.0f)
            param= 2.0f - (time - 17.0f);
         else
            param= 2.0f;

         mesh->setRenderParameter( 1, -param );
         it++;
      }
      else
      {
         Material* mat= mScene->getMaterial(10);
         mesh->setRenderParameter( 1, 0 );
         mat->removeMesh( mesh );
         it= mPlayers.erase( it );
//         *it= globalTime;
      }
   }
}


void InvisiblePlayerEffect::addPlayer(PlayerItem* player)
{
   if (!mPlayers.contains(player))
   {
      float startTime= GlobalTime::Instance()->getTime();
      mPlayers.insert(player, startTime);

      Material* mat= mScene->getMaterial(10);
      mat->addMesh(player->getMesh());
   }
}


void InvisiblePlayerEffect::removePlayer(PlayerItem* player)
{
   if (mPlayers.contains(player))
   {
      Material* mat= mScene->getMaterial( 10 );
      mat->removeMesh( player->getMesh() );
      mPlayers.remove(player);
   }
}


void InvisiblePlayerEffect::removeAllPlayers()
{
   foreach (PlayerItem* player, mPlayers.keys())
   {
      removePlayer(player);
   }
}

