#include "castlematerialfactory.h"
#include "materials/material.h"
#include "engine/materials/environmenttexturematerial.h"
#include "engine/materials/texturematerial.h"
#include "engine/materials/environmentmaterial.h"
#include "engine/materials/displacementmaterial.h"
#include "engine/materials/environmentambientmaterial.h"
#include "engine/materials/environmentambientdiffusematerial.h"

Material* CastleMaterialFactory::createMaterial(SceneGraph*, int) const
{
   return nullptr;
}
