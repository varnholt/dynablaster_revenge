#ifndef LEVELMANSION_H
#define LEVELMANSION_H

#include "../level.h"
#include "materials/materialfactory.h"

class LevelMansion : public Level, public MaterialFactory
{
public:

   //! constructor
   LevelMansion();

   //! initialize mansion level
   virtual void initialize();
   virtual void loadData();

   Material* createMaterial(SceneGraph* scene, int id) const;
};

#endif // LEVELMANSION_H
