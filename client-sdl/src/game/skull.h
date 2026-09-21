#ifndef SKULL_H
#define SKULL_H

#include "nodes/mesh.h"
#include "math/matrix.h"


class Skull : public Mesh
{

public:

   //! constructor
   Skull(Mesh* reference, float x, float y);

   //! destructor
   virtual ~Skull();

   //! getter for reference mesh
   Mesh* getReference() const;

   //! getter for translation matrix
   Matrix getTranslation() const;

   //! getter for start time
   float getStartTime() const;


private:

   //! animation time
   float mStartTime;

   //! reference
   Mesh* mReference;

   //! translation matrix
   Matrix mTranslation;
};


#endif // SKULLCYCLEANIMATION_H
