#pragma once

#include "engine/materials/materialfactory.h"

/// \brief concrete MaterialFactory for the demo scene, mirroring the real material-ID
/// dispatch the original game used for actual levels (see LevelCastle::createMaterial in
/// client/src/levels/castle/levelcastle.cpp - levels implement MaterialFactory directly on
/// the Level subclass; the separate, unrelated castlematerialfactory.cpp file is the dead
/// stub, not this dispatch logic).
class DemoMaterialFactory : public MaterialFactory
{
public:
   Material* createMaterial(SceneGraph* scene, int materialId) const override;
};
