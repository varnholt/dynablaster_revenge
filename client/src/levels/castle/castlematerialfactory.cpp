#include "castlematerialfactory.h"
#include "materials/material.h"
#include "environmenttexturematerial.h"
#include "texturematerial.h"
#include "environmentmaterial.h"
#include "displacementmaterial.h"
#include "environmentambientmaterial.h"
#include "environmentambientdiffusematerial.h"

Material* CastleMaterialFactory::createMaterial(SceneGraph*, int) const
{
   return 0;
}
