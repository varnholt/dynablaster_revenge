#pragma once

#include "materials/materialfactory.h"

class CastleMaterialFactory : public MaterialFactory
{
public:
   Material* createMaterial(SceneGraph* scene, int id) const;

private:
};
