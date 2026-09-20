#include "demofactory.h"

#include "engine/materials/material.h"
#include "engine/materials/texturematerial.h"
#include "engine/materials/environmentmaterial.h"
#include "engine/materials/environmenttexturematerial.h"
#include "engine/materials/environmentambientdiffusematerial.h"
#include "engine/materials/displacementmaterial.h"

Material* DemoMaterialFactory::createMaterial(SceneGraph* scene, int materialId) const
{
   switch (materialId)
   {
      case MAP_DIFFUSE:
         return new TextureMaterial(scene);
      case MAP_REFLECT:
         return new EnvironmentMaterial(scene);
      case (MAP_DIFFUSE | MAP_REFLECT):
         return new EnvironmentTextureMaterial(scene);
      case (MAP_AMBIENT | MAP_DIFFUSE | MAP_REFLECT):
         return new EnvironmentAmbientDiffuseMaterial(scene);
      case (MAP_DIFFUSE | MAP_DISPLACE):
         return new DisplacementMaterial(scene);
      default:
         return 0;
   }
}
