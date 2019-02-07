#pragma once

#include "material.h"

class SceneGraph;

class MaterialFactory
{
public:
   MaterialFactory();
   virtual ~MaterialFactory();

   virtual Material* createMaterial(SceneGraph* scene, int materialId) const = 0;

private:
};

