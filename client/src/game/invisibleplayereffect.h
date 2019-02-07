#pragma once

#include <QMap>

class SceneGraph;
class PlayerItem;

class InvisiblePlayerEffect
{
public:
   InvisiblePlayerEffect();

   void update(float time);

   void setPlayerScene( SceneGraph* players );

   void addPlayer(PlayerItem* player);
   void removePlayer(PlayerItem* player);
   void removeAllPlayers();

private:
   SceneGraph* mScene;
   QMap<PlayerItem*, float> mPlayers;
};
