#pragma once

// base
#include "../level.h"
#include "materials/materialfactory.h"

class SnowAnimation;

class LevelCastle : public Level, public MaterialFactory
{

public:

   LevelCastle();   

   virtual void initialize();
   virtual void draw();
   virtual void animate(float dt);
   virtual void reset();
   virtual void loadData();

   Material* createMaterial(SceneGraph* scene, int id) const;

protected:

   SnowAnimation* mSnowAnimation;

};
