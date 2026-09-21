#pragma once

// base
#include "../level.h"
#include "materials/materialfactory.h"

// Deferred (see project memory - Phase 5): SnowAnimation uses raw fixed-function
// glPushMatrix/glTranslatef/glScalef GL calls (GLES3-incompatible) and is pure atmosphere with
// zero gameplay impact - dropped, not ported. Same for the one LensFlareFactory::activate("castle")
// call the real LevelCastle::initialize() makes - pure visual polish.
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
};
